#include "ExtractorFactory.h"
#include "../common/LabelledData.h"
#include "../common/TypeCommon.h"
#include "../common/CommonDefines.h"

#include <QtCore/QCoreApplication>
#include <QDir>

#ifdef HAS_ELAPSED_TIMER
#include <QElapsedTimer>
#else
#include <QTime>
#endif

#include <QDebug>

void saveFeatures(QDataStream &outstream, const QString &extractorName,
                  const QStringList &extractorArgs, const int &extractorSize,
                  const QVector<LabelledData> &data)
{
    outstream.setVersion(QDataStream::Qt_4_6);
    outstream.setByteOrder(QDataStream::BigEndian);

    int size = sizeof(nnreal);
    outstream << size;
    outstream << extractorName;
    outstream << extractorArgs;
    outstream << extractorSize;
    outstream << data;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (a.arguments().size() < 3) {
        qCritical() << "this application requires more arguments!";
        return -1;
    }

    const QString trainDirName = a.arguments().at(1);
    const QString testDirName = a.arguments().at(2);
    const QString outputName = a.arguments().at(3);
    const QString extractorName = a.arguments().at(4);
    const QStringList extractorArgs = a.arguments().mid(5);
    ExtractorInterface *extractor =
            ExtractorFactory::getExtractor(extractorName, extractorArgs);
    if (extractor == NULL) {
        qCritical() << "failed to initialise extractor" << extractorName;
        return -2;
    }

    QDir trainDir(trainDirName);
    QStringList subdirs = QStringList() << "wood" << "straw" << "salt" << "linen";
    QList<quint8> labels = QList<quint8>() << 32 << 96 << 160 << 224;
    QVector<LabelledData> trainData;
#ifdef HAS_ELAPSED_TIMER
    QElapsedTimer extractionTimer;
#else
    QTime extractionTimer;
#endif
    extractionTimer.start();
    unsigned int imagesCount = 0;
    for (int j = 0; j < subdirs.size(); j++) {
        trainDir.cd(subdirs.at(j));
        const QFileInfoList fileList = trainDir.entryInfoList(QStringList() << "*.png");
#ifdef HAS_ELAPSED_TIMER
        QElapsedTimer extractorTimer;
#else
        QTime extractorTimer;
#endif
        extractorTimer.start();
        for (int i = 0; i < qMin(fileList.size(), 5); i++) {
            imagesCount++;
            const QString filename = fileList.at(i).filePath();
            const QImage image(filename);
            if (image.format() != QImage::Format_Indexed8) {
                qCritical("Image is not greyscale!");
                return -1;
            }
            extractor->preprocess(image);
            if (extractor->extracts()) {
                unsigned int count = trainData.size();
                trainData.resize(trainData.size() + image.width() * image.height());
                LabelledData *trainDataPtr = trainData.data();
#pragma omp parallel for
                for (int x = 0; x < image.width(); x++) {
                    for (int y = 0; y < image.height(); y++) {
                        const QVector<nnreal> res = extractor->extract(image, x, y);
                        Q_ASSERT(res.size() == extractor->size());
                        LabelledData li(res, labels.at(j));
                        const unsigned int idx = count + x * image.height() + y;
                        trainDataPtr[idx] = li;
                    }
                }
            }
            const QVector<QVector<nnreal> > ppFeatures = extractor->postprocess(image);
            const int ppCount = ppFeatures.size();
            if (ppCount > 0) {
                const int count = trainData.size();
                trainData.resize(trainData.size() + ppFeatures.size());
                LabelledData *trainDataPtr = trainData.data();
#pragma omp parallel for
                for (int k = 0; k < ppCount; k++) {
                    Q_ASSERT(ppFeatures.at(k).size() == extractor->size());
                    LabelledData ld(ppFeatures.at(k), labels.at(j));
                    trainDataPtr[count + k] = ld;
                }
            }
            qDebug() << fileList.at(i).filePath() << extractorTimer.restart();
        }
        trainDir.cdUp();
    }

    const int msecs = extractionTimer.elapsed();
    qDebug() << "trainSize:" << trainData.size() << "extraction of "
             << imagesCount << "images took" << msecs << "msecs, average"
             << float(msecs) / imagesCount << "msecs per image";
    const QString trainOutFilename(outputName + "_" + extractorName + "_train.out");
    QFile trainOutput(trainOutFilename);
    if (!trainOutput.open(QIODevice::WriteOnly)) {
        qCritical() << "failed to open output file" << trainOutFilename;
        return -3;
    }
    {
#ifdef HAS_ELAPSED_TIMER
        QElapsedTimer saveTimer;
#else
        QTime saveTimer;
#endif
        saveTimer.start();
        QDataStream outstream(&trainOutput);
        saveFeatures(outstream, extractorName, extractorArgs, extractor->size(), trainData);
        int msecs = saveTimer.elapsed();
        qDebug() << "saving took" << msecs << "msecs";
    }
    trainOutput.close();
    trainData.clear();

    {
        QDir testDir(testDirName);
        const QFileInfoList dataFileList  = testDir.entryInfoList(QStringList() << "test*.png");
        const QFileInfoList labelFileList = testDir.entryInfoList(QStringList() << "label*.png");
        Q_ASSERT(dataFileList.size() == labelFileList.size());
#ifdef HAS_ELAPSED_TIMER
        QElapsedTimer extractorTimer;
#else
        QTime extractorTimer;
#endif
        extractorTimer.start();
        QTextStream out(stdout);
        for (int i = 0; i < dataFileList.size(); i++) {
            unsigned int count = 0;
            const QImage dataImage(dataFileList.at(i).filePath());
            const QImage labelImage(labelFileList.at(i).filePath());
            QVector<LabelledData> testData(dataImage.width() * dataImage.height());
            LabelledData *testDataPtr = testData.data();
            int cnt = 0;
#pragma omp parallel for
            for (int x = 0; x < dataImage.width(); x++) {
#pragma omp critical
                {
                    cnt++;
                    out << cnt * 100 / dataImage.width() << "%" << '\r';
                    out.flush();
                }
                for (int y = 0; y < dataImage.height(); y++) {
                    const QVector<nnreal> res = extractor->extract(dataImage, x, y);
                    const quint8 c = labelImage.pixelIndex(x, y);
                    LabelledData li(res, c);
                    li.squeeze();
                    const unsigned int idx = count + x * dataImage.height() + y;
                    testDataPtr[idx] = li;
                }
            }
            count += dataImage.width() * dataImage.height();
            qDebug() << dataFileList.at(i).filePath() << extractorTimer.restart();
            const QString testOutFilename(outputName + "_" + extractorName + "_test" + QString::number(i) + ".out");
            QFile testOutput(testOutFilename);
            if (!testOutput.open(QIODevice::WriteOnly)) {
                qCritical() << "failed to open output file" << testOutFilename;
                return -3;
            }
            {
#ifdef HAS_ELAPSED_TIMER
                QElapsedTimer saveTimer;
#else
                QTime saveTimer;
#endif
                saveTimer.start();
                QDataStream outstream(&testOutput);
                saveFeatures(outstream, extractorName, extractorArgs, extractor->size(), testData);
                int msecs = saveTimer.elapsed();
                qDebug() << "saving took" << msecs << "msecs";
            }
            testOutput.close();
        }
    }

    delete extractor;

    return 0;
}
