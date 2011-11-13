#include "FeatureImporter.h"
#include "CpuClassifier.h"
#include "NaiveClassifier.h"

#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QElapsedTimer>
#include <QHostInfo>
#include <QTextCodec>

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

	QFile trainFile(args.at(1));
	if (!trainFile.open(QIODevice::ReadOnly)) {
		qFatal("Failed to open train file %s.\n", trainFile.fileName().toStdString().c_str());
	}
	QFile testFile(args.at(2));
	if (!testFile.open(QIODevice::ReadOnly)) {
		qFatal("Failed to open test file %s.\n", testFile.fileName().toStdString().c_str());
	}

    QElapsedTimer loadTimer;
    loadTimer.start();
    FeatureImporter trainFeatures;
    FeatureImporter testFeatures;
#pragma omp sections
    {
#pragma omp section
        {
            trainFeatures.open(&trainFile);
        }
#pragma omp section
        {
            testFeatures.open(&testFile);
        }
    }
    int loadMsecs = loadTimer.elapsed();
    qDebug() << "loading took" << loadMsecs << "msecs";

	trainFile.close();
	testFile.close();

	QVector<QString> hash;
    QVector<qint8> trainClasses;
	for (int i = 0; i < trainFeatures.labels().size(); i++) {
        qint8 index = hash.indexOf(trainFeatures.labels().at(i));
		if (index == -1) {
			QString dbg("Appending label \"%1\" to hash at position %2. It has now value \"%3\"");
			hash.append(trainFeatures.labels().at(i));
			index = hash.size() - 1;
			//qDebug() << dbg.arg(trainFeatures.labels().at(i), QString::number(index), hash.at(index));
		}
		trainClasses.append(index);
	}

    ClassifierInterface *ci = new NaiveClassifier();
    QVector<QVector<int> > classes;
    qDebug() << "starting classification";
    QList<int> k;

    bool ok = true;
    int i = 50;
    if (args.size() >= 4) {
        i = qMax(0, args.at(3).toInt(&ok));
        qDebug() << "i =" << i;
    } else {
        ok = false;
    }
    if (!ok) {
        qDebug() << "no k given, assuming k = 50";
        i = 50;
    }
    qDebug() << "initial k:" << i;
    for (; i >= 1; i--) {
        k.append(i);
    }
    QElapsedTimer timer;
    timer.start();
	classes = ci->classify(trainFeatures.features(), testFeatures.features(),
						   trainClasses.constData(), NULL,
						   testFeatures.featuresPerItem(),
						   trainFeatures.itemCount(), testFeatures.itemCount(),
                           k);
    delete ci;
	int msecs = timer.elapsed();
	qDebug() << "calculations took" << msecs << "msecs";
    qDebug() << "there are" << classes.size() << "k-s";
    QString filenameBase("gnuplot_%1_%2_%3_%4.dat");
    QString hostname(QHostInfo::localHostName());
    QString filename(filenameBase.arg(testFeatures.name(), QString::number(trainFeatures.itemCount()), QString::number(testFeatures.itemCount()), hostname));
    QFile gnuplotFile(filename);
    if (!gnuplotFile.open(QIODevice::WriteOnly)) {
        qFatal("failed to open file %s for writing.\n", filename.toStdString().c_str());
    }
    QTextStream gnuplotStream(&gnuplotFile);
    for (int w = 0; w < classes.size(); w++) {
        //qDebug() << "w:" << w;
        int correct = 0;
        QVector<QVector<qreal> > confusionMatrix;
        confusionMatrix.resize(hash.size());
        for (int i = 0; i < confusionMatrix.size(); i++) {
            confusionMatrix[i].resize(hash.size());
        }
        for (int i = 0; i < classes.at(w).size(); i++) {
            /*qDebug() << i;
            qDebug() << classes.at(i);
            qDebug() << hash.at(classes.at(i));
            qDebug() << testFeatures.labels().at(i);*/
            const QString key(testFeatures.labels().at(i));
            const int keyIdx = hash.indexOf(key);
            const int classIdx = classes.at(w).at(i);
            confusionMatrix[keyIdx][classIdx]++;
            /*if (hash.at(classes.at(w).at(i)) == QString("5")) {
                qDebug() << "is 5, should be " << testFeatures.labels().at(i);
            }*/
            if (hash.at(classIdx) == key) {
                correct++;
            }
        }
        QVector<QPair<QString, int> > sorter;
        for (int i = 0; i < hash.size(); i++) {
            sorter << qMakePair(hash.at(i), i);
        }
        qSort(sorter);
        QStringList l;
        for (int i = 0; i < hash.size(); i++) {
            l << sorter.at(i).first; //hash.at(i);
        }
        QVector<QVector<qreal> > tempConfusionMatrix;
        tempConfusionMatrix.resize(hash.size());
        for (int j = 0; j < confusionMatrix.size(); j++) {
            for (int i = 0; i < sorter.size(); i++) {
                tempConfusionMatrix[j] << confusionMatrix.at(j).at(sorter.at(i).second);
            }
        }
        confusionMatrix = tempConfusionMatrix;
        for (int j = 0; j < confusionMatrix.size(); j++) {
            tempConfusionMatrix[j] = confusionMatrix.at(sorter.at(j).second);
        }
        confusionMatrix = tempConfusionMatrix;
#ifdef PERCENTAGE_CONFUSION
        for (int i = 0; i < confusionMatrix.size(); i++) {
            qreal sum = 0;
            for (int j = 0; j < confusionMatrix.at(i).size(); j++) {
                sum += confusionMatrix.at(i).at(j);
            }
            if (sum != 0) {
                for (int j = 0; j < confusionMatrix.at(i).size(); j++) {
                    confusionMatrix[i][j] = confusionMatrix.at(i).at(j) / sum * 100.0;
                }
            } else {
                for (int j = 0; j < confusionMatrix.at(i).size(); j++) {
                    confusionMatrix[i][j] = 0;
                }
            }
        }
#endif
        QTextStream stream(stdout);
        //stream << "k: " << k.at(w) << endl;
        stream << "\\begin{table}" << endl
               << "\\centering" << endl
               << QString("\\caption{Macierz wyników ekstraktora \\texttt{%1} dla $k = %2$. Ogólna skuteczność: %3%}").arg(testFeatures.name(), QString::number(k.at(w)), QString::number(((float)correct / (float)classes.at(w).size()) * 100)) << endl
               << QString("\\label{tab:confusion_%1_%2}").arg(testFeatures.name(), QString::number(k.at(w))) << endl
               << "\\begin{tabular}{|c|c|c|c|c|c|c|c|c|c|c|}" << endl;
        stream << "\t&\t" << l.join("\t&\t") << "\\\\" << endl << "\\hline" << endl;
        for (int i = 0; i < confusionMatrix.size(); i++) {
            QStringList list;
            list << sorter.at(i).first; //hash.at(i);
            for (int j = 0; j < confusionMatrix.size(); j++) {
                list << QString::number(confusionMatrix[j][i], 'g', 4);
            }
            const QString joined(list.join("\t&\t"));
            stream << joined << "\\\\" << endl << "\\hline" << endl;
        }
        stream << "\\end{tabular}" << endl
               << "\\end{table}" << endl << endl;
        //stream << "correct: " << ((float)correct / (float)classes.at(w).size()) * 100 << "%" << endl;
        qDebug() << "gnuplot file: " << filename;
        gnuplotStream << k.at(w) << ' ' << ((float)correct / (float)classes.at(w).size()) * 100 << endl;

    }
    gnuplotFile.close();
    msecs = timer.elapsed();
    qDebug() << "everything took" << msecs << "msecs";
	return 0;
}
