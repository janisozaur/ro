#include "ExtractorFactory.h"
#include "../common/LabelledData.h"
#include "../common/TypeCommon.h"

#include <QtCore/QCoreApplication>
#include <QDir>

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (a.arguments().size() < 3) {
        qCritical() << "this application requires more arguments!";
        return -1;
    }


    const QString dirname = a.arguments().at(1);
    const QString extractorName = a.arguments().at(2);
    const QStringList extractorArgs = a.arguments().mid(3);
    ExtractorInterface *extractor =
            ExtractorFactory::getExtractor(extractorName, extractorArgs);
    if (extractor == nullptr) {
        qCritical() << "failed to initialise extractor" << extractorName;
        return -2;
    }

    const QString outFilename(extractorName + "_train.out");
    QFile output(outFilename);
    if (!output.open(QIODevice::WriteOnly)) {
        qCritical() << "failed to open output file" << outFilename;
        return -3;
    }

    QDir d(dirname);
    QStringList subdirs = QStringList() << "wood" << "straw" << "salt" << "linen";
    QList<quint8> labels = QList<quint8>() << 32 << 96 << 160 << 224;
    QVector<LabelledData> trainData;
    for (int j = 0; j < subdirs.size(); j++) {
        d.cd(subdirs.at(j));
        const QFileInfoList fileList = d.entryInfoList(QStringList() << "*.png");
        trainData.reserve(trainData.size() + fileList.size());
        for (int i = 0; i < fileList.size(); i++) {
            const QImage image(fileList.at(i).filePath());
            const QVector<nnreal> res = extractor->extract(image, 32, 32);
            LabelledData li(res, labels.at(j));
            trainData.append(li);
        }
        d.cdUp();
    }
    QDataStream outstream(&output);
    outstream.setVersion(QDataStream::Qt_4_6);
    outstream.setByteOrder(QDataStream::BigEndian);
    qDebug() << "trainSize:" << trainData.size();

    int size = sizeof(nnreal);
    outstream << size;
    outstream << extractorName;
    outstream << extractorArgs;
    outstream << extractor->size();
    outstream << trainData;
    output.close();

    delete extractor;

    return 0;
}
