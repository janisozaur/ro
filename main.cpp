#include "FeatureImporter.h"
#include "CpuClassifier.h"

#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QElapsedTimer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
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

	FeatureImporter trainFeatures(&trainFile);
	FeatureImporter testFeatures(&testFile);

	trainFile.close();
	testFile.close();

	QVector<QString> hash;
	QVector<int> trainClasses;
	for (int i = 0; i < trainFeatures.labels().size(); i++) {
		int index = hash.indexOf(trainFeatures.labels().at(i));
		if (index == -1) {
			QString dbg("Appending label \"%1\" to hash at position %2. It has now value \"%3\"");
			hash.append(trainFeatures.labels().at(i));
			index = hash.size() - 1;
			//qDebug() << dbg.arg(trainFeatures.labels().at(i), QString::number(index), hash.at(index));
		}
		trainClasses.append(index);
	}

	ClassifierInterface *ci = new CpuClassifier();
	QVector<int> classes;
	QElapsedTimer timer;
	timer.start();
	classes = ci->classify(trainFeatures.features(), testFeatures.features(),
						   trainClasses.constData(), NULL,
						   testFeatures.featuresPerItem(),
						   trainFeatures.itemCount(), testFeatures.itemCount(),
						   QList<int>() << 10);
	int msecs = timer.elapsed();
	qDebug() << "calculations took" << msecs << "msecs";
	int correct = 0;
	for (int i = 0; i < classes.size(); i++) {
		/*qDebug() << i;
		qDebug() << classes.at(i);
		qDebug() << hash.at(classes.at(i));
		qDebug() << testFeatures.labels().at(i);*/
		if (hash.at(classes.at(i)) == testFeatures.labels().at(i)) {
			correct++;
		}
	}
	qDebug() << "correct: " << ((float)correct / (float)classes.size()) * 100 << "%";
	return 0;
}
