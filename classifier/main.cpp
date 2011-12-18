#include "FeatureImporter.h"
#include "KnnClassifier.h"
#include "NeuralNetwork/NeuralNetwork.h"
#include "../common/CommonDefines.h"

#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QFile>
#ifdef HAS_ELAPSED_TIMER
#include <QElapsedTimer>
#else
#include <QTime>
#endif
#include <QHostInfo>
#include <QTextCodec>
#include <omp.h>
#include <QBuffer>
#include <QImage>

#define PERCENTAGE_CONFUSION

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // set encoding
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QStringList args = a.arguments();
    if (args.size() < 3) {
        QStringList usage;
        usage << args.at(0)
              << "[train data]"
              << "[test data]";
        qFatal("Too few arguments. Usage:\n%s\n", usage.join(" ").toStdString().c_str());
    }

    QFile testFile(args.at(1));
    if (!testFile.open(QIODevice::ReadOnly)) {
        qFatal("Failed to open test file %s.\n", testFile.fileName().toStdString().c_str());
    }
    FeatureImporter testFeatures(&testFile);
    testFile.close();

    QSize size;
    {
        bool ok;
        const int width = args.at(2).toInt(&ok);
        if (!ok) {
            const int argNum = 2;
            qCritical() << "failed to parse arg" << argNum << ":"
                        << args.at(argNum) << "as int";
            return -1;
        }
        const int height = args.at(3).toInt(&ok);
        if (!ok) {
            const int argNum = 3;
            qCritical() << "failed to parse arg" << argNum << ":"
                        << args.at(argNum) << "as int";
            return -1;
        }
        size = QSize(width, height);
    }

    QImage resultImg(size, QImage::Format_RGB32);
    const QString imageName(args.at(4));

    int threadCount = 1;
#ifdef _OPENMP
#pragma omp parallel
    {
#pragma omp single
        {
            threadCount = omp_get_num_threads();
        }
    }
#endif
    qDebug() << "using" << threadCount << "threads.";
    ClassifierInterface **ci = new ClassifierInterface *[threadCount];

    const QString classifierName = args.at(5);
    const QStringList classifierArgs = args.mid(6);
    if (classifierName == "knn") {
        QFile trainFile(classifierArgs.at(0));
        if (!trainFile.open(QIODevice::ReadOnly)) {
            qFatal("Failed to open train file %s.\n", trainFile.fileName().toStdString().c_str());
        }
        FeatureImporter trainFeatures(&trainFile);
        trainFile.close();
        bool ok = true;
        int k = 50;
        if (classifierArgs.size() >= 2) {
            k = qMax(0, classifierArgs.at(1).toInt(&ok));
            qDebug() << "k =" << k;
        } else {
            ok = false;
        }
        if (!ok) {
            qDebug() << "no k given, assuming k = 50";
            k = 50;
        }
        qDebug() << "initial k:" << k;
        for (int i = 0; i < threadCount; i++) {
            ci[i] = new KnnClassifier(k, trainFeatures);
        }
    } else if (classifierName == "ann") {
        QFile networkFile(classifierArgs.at(0));
        if (!networkFile.open(QIODevice::ReadOnly)) {
            qFatal("Failed to open network file %s.\n", networkFile.fileName().toStdString().c_str());
        }
        QByteArray qba = networkFile.readAll();
        networkFile.close();
        for (int i = 0; i < threadCount; i++) {
            QBuffer buf(&qba);
            buf.open(QIODevice::ReadOnly);
            NeuralNetwork *nn = new NeuralNetwork();
            QDataStream ds(&buf);
            ds >> *nn;
            buf.close();
            ci[i] = nn;
        }
    } else {
        qCritical() << "unrecognised classifier:" << classifierName;
        return -1;
    }

    const int testItemCount = testFeatures.itemCount();
    QVector<quint8> classes(testItemCount);
    quint8 *classesPtr = classes.data();
    qDebug() << "starting classification of" << testItemCount << "items";
#ifdef HAS_ELAPSED_TIMER
    QElapsedTimer timer;
#else
    QTime timer;
#endif
    timer.start();
#pragma omp parallel
    {
        const int threadNum = omp_get_thread_num();
        #pragma omp for
        for (int i = 0; i < testItemCount; i++) {
            classesPtr[i] = ci[threadNum]->classify(testFeatures.featuresForItem(i));
        }
    }
    int msecs = timer.elapsed();
    for (int i = 0; i < threadCount; i++) {
        delete ci[i];
    }
    delete [] ci;
    qDebug() << "calculations took" << msecs << "msecs";

    quint32 correctCount = 0;
    for (quint32 i = 0; i < quint32(classes.size()); i++) {
        const int c = classes.at(i);
        if (c == testFeatures.classIdForItem(i)) {
            correctCount++;
        }
        const int x = i % size.width();
        const int y = i / size.width();
        resultImg.setPixel(x, y, qRgb(c, c, c));
    }
    qDebug() << "correctness:" << float(correctCount) / float(testItemCount) * 100;
    resultImg.save(imageName + ".png");

//    QString filenameBase("gnuplot_%1_%2_%3_%4.dat");
//    QString hostname(QHostInfo::localHostName());
//    QString filename(filenameBase.arg(testFeatures.name(), QString::number(trainFeatures.itemCount()), QString::number(testFeatures.itemCount()), hostname));
//    QFile gnuplotFile(filename);
//    if (!gnuplotFile.open(QIODevice::WriteOnly)) {
//        qFatal("failed to open file %s for writing.\n", filename.toStdString().c_str());
//    }
//    QTextStream gnuplotStream(&gnuplotFile);
//    for (int w = 0; w < classes.size(); w++) {
//        //qDebug() << "w:" << w;
//        int correct = 0;
//        QVector<QVector<qreal> > confusionMatrix;
//        confusionMatrix.resize(hash.size());
//        for (int i = 0; i < confusionMatrix.size(); i++) {
//            confusionMatrix[i].resize(hash.size());
//        }
//        for (int i = 0; i < classes.at(w).size(); i++) {
//            const QString key(testFeatures.labels().at(i));
//            const int keyIdx = hash.indexOf(key);
//            const int classIdx = classes.at(w).at(i);
//            confusionMatrix[keyIdx][classIdx]++;
//            if (hash.at(classIdx) == key) {
//                correct++;
//            }
//        }
//        QVector<QPair<QString, int> > sorter;
//        for (int i = 0; i < hash.size(); i++) {
//            sorter << qMakePair(hash.at(i), i);
//        }
//        qSort(sorter);
//        QStringList l;
//        for (int i = 0; i < hash.size(); i++) {
//            l << sorter.at(i).first; //hash.at(i);
//        }
//        QVector<QVector<qreal> > tempConfusionMatrix;
//        tempConfusionMatrix.resize(hash.size());
//        for (int j = 0; j < confusionMatrix.size(); j++) {
//            for (int i = 0; i < sorter.size(); i++) {
//                tempConfusionMatrix[j] << confusionMatrix.at(j).at(sorter.at(i).second);
//            }
//        }
//        confusionMatrix = tempConfusionMatrix;
//        for (int j = 0; j < confusionMatrix.size(); j++) {
//            tempConfusionMatrix[j] = confusionMatrix.at(sorter.at(j).second);
//        }
//        confusionMatrix = tempConfusionMatrix;
//#ifdef PERCENTAGE_CONFUSION
//        for (int i = 0; i < confusionMatrix.size(); i++) {
//            qreal sum = 0;
//            for (int j = 0; j < confusionMatrix.at(i).size(); j++) {
//                sum += confusionMatrix.at(i).at(j);
//            }
//            if (sum != 0) {
//                for (int j = 0; j < confusionMatrix.at(i).size(); j++) {
//                    confusionMatrix[i][j] = confusionMatrix.at(i).at(j) / sum * 100.0;
//                }
//            } else {
//                for (int j = 0; j < confusionMatrix.at(i).size(); j++) {
//                    confusionMatrix[i][j] = 0;
//                }
//            }
//        }
//#endif
//        QTextStream stream(stdout);
//        //stream << "k: " << k.at(w) << endl;
//        stream << "\\begin{table}" << endl
//               << "\\centering" << endl
//               << QString("\\caption{Macierz wyników ekstraktora \\texttt{%1} dla $k = %2$. Ogólna skuteczność: %3\\%}").arg(testFeatures.name(), QString::number(k.at(w)), QString::number(((float)correct / (float)classes.at(w).size()) * 100)) << endl
//               << QString("\\label{tab:confusion_%1_%2}").arg(testFeatures.name(), QString::number(k.at(w))) << endl
//               << "\\begin{tabular}{|c|c|c|c|c|c|c|c|c|c|c|}" << endl
//               << "\\hline" << endl;
//        stream << "\t&\t" << l.join("\t&\t") << "\\\\" << endl << "\\hline" << endl;
//        for (int i = 0; i < confusionMatrix.size(); i++) {
//            QStringList list;
//            list << sorter.at(i).first; //hash.at(i);
//            for (int j = 0; j < confusionMatrix.size(); j++) {
//                list << QString::number(confusionMatrix[j][i], 'g', 3);
//            }
//            const QString joined(list.join("\t&\t"));
//            stream << joined << "\\\\" << endl << "\\hline" << endl;
//        }
//        stream << "\\end{tabular}" << endl
//               << "\\end{table}" << endl << endl;
//        //stream << "correct: " << ((float)correct / (float)classes.at(w).size()) * 100 << "%" << endl;
//        qDebug() << "gnuplot file: " << filename;
//        gnuplotStream << k.at(w) << ' ' << ((float)correct / (float)classes.at(w).size()) * 100 << endl;

//    }
//    gnuplotFile.close();
//    msecs = timer.elapsed();
//    qDebug() << "everything took" << msecs << "msecs";
    return 0;
}
