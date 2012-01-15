#include "../classifier/NeuralNetwork/NeuralNetwork.h"
#include "../classifier/FeatureImporter.h"
#include "../common/CommonDefines.h"

#include <QtCore/QCoreApplication>
#include <QFile>
#include <QVector>
#include <QDateTime>
#include <QBuffer>

#include <QDebug>

QVector<nnreal> classToVector(const quint8 &classId, const nnreal &initial)
{
	QVector<nnreal> result(4, initial);
	int idx;
	switch (classId) {
		case 32:
			idx = 0;
			break;
		case 96:
			idx = 1;
			break;
		case 160:
			idx = 2;
			break;
		default:
		case 224:
			idx = 3;
			break;
	}
	result[idx] = 0.75;
	return result;
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	qsrand(QDateTime::currentDateTime().toTime_t());
	const QStringList args = a.arguments();

	if (args.size() < 4) {
		QStringList usage;
		usage << args.at(0) << "[input file]" << "[output file]" << "[epochs]"
			  << "[lr]" << "[momentum]" << "[desired error]";
		qCritical() << usage.join(" ");
		return -1;
	}
	const QString inputFilename = args.at(1);
	bool ok;
	const nnreal maxEpochs = args.at(3).toInt(&ok);
	if (!ok) {
		const int argIdx = 3;
		qCritical() << "failed to parsee arg" << argIdx << ":" << args.at(argIdx) << "as int";
		return -1;
	}
	const nnreal lr = args.at(4).toFloat(&ok);
	if (!ok) {
		const int argIdx = 4;
		qCritical() << "failed to parsee arg" << argIdx << ":" << args.at(argIdx) << "as float";
		return -1;
	}
	const nnreal momentum = args.at(5).toFloat(&ok);
	if (!ok) {
		const int argIdx = 5;
		qCritical() << "failed to parsee arg" << argIdx << ":" << args.at(argIdx) << "as float";
		return -1;
	}
	const nnreal desiredError = args.at(6).toFloat(&ok);
	if (!ok) {
		const int argIdx = 6;
		qCritical() << "failed to parsee arg" << argIdx << ":" << args.at(argIdx) << "as float";
		return -1;
	}
	const QString outputFilename = args.at(2) + QString::number(maxEpochs) + "_" +
								   QString::number(lr) + "_" +
								   QString::number(momentum) + "_" +
								   QString::number(desiredError) + ".ann";
	QFile f(inputFilename);
	if (!f.open(QIODevice::ReadOnly)) {
		qCritical() << "failed to open file" << f.fileName();
		return -2;
	}
	FeatureImporter td(&f);
	f.close();
	qDebug() << "teaching ann from extractor" << td.name()
			 << "it was run with params" << td.args().join(" ") << ", has"
			 << td.itemCount() << "items, each of which has"
			 << td.featuresPerItem() << "features";

	NeuralNetwork nn;
	nn.addLayer(td.featuresPerItem(), Neuron::Tanh);
	nn.addLayer(td.featuresPerItem(), Neuron::Tanh);
	nn.addLayer(td.featuresPerItem() / 2, Neuron::Tanh);
	nn.addLayer(4, Neuron::Sigmoid);
	QVector<QVector<nnreal> > input;
	QVector<QVector<nnreal> > output;
#ifdef HAS_VECTOR_RESERVE
	input.reserve(td.itemCount());
	output.reserve(td.itemCount());
#endif
	quint32 itemCount = td.itemCount();
	qDebug() << "training on" << itemCount << "elements";
	for (quint32 i = 0; i < itemCount; i++) {
		const QVector<nnreal> inData = td.featuresForItem(i);
		input.append(inData);
		const QVector<nnreal> outData = classToVector(td.classIdForItem(i), 0.25);
		output.append(outData);
	}
	QFile outputFile(outputFilename);
	if (!outputFile.open(QIODevice::WriteOnly)) {
		qCritical() << "failed to open file" << outputFile.fileName();
		return -2;
	}
	QDataStream ds(&outputFile);
	const QVector<float> errors = nn.train(input, output, maxEpochs, lr, momentum, desiredError, ds);
	QBuffer buf;
	buf.open(QIODevice::WriteOnly);
	QDataStream ds2(&buf);
	ds2 << nn;
	buf.close();
	NeuralNetwork nn2;
	buf.open(QIODevice::ReadOnly);
	QDataStream ds3(&buf);
	ds3 >> nn2;
	buf.close();
	outputFile.close();
	{
		QFile errorsFile("errors_" + outputFilename);
		if (!errorsFile.open(QIODevice::WriteOnly)) {
			qCritical() << "failed to open file" << errorsFile.fileName();
			return -2;
		}
		QTextStream ts(&errorsFile);
		for (int i = 0; i < errors.size(); i++) {
			ts << errors.at(i) << endl;
		}
		errorsFile.close();
	}
	QList<int> indices;
#ifdef HAS_VECTOR_RESERVE
	indices.reserve(input.size());
#endif
	for (int i = 0; i < input.size(); i++) {
		indices << i;
	}
	if (!outputFile.open(QIODevice::ReadOnly)) {
		qCritical() << "failed to open file" << outputFile.fileName();
		return -2;
	} else {
		QDataStream inds(&outputFile);
		//inds >> nn;
		outputFile.close();
	}
	int correct = 0;
	int max = qMin(1000, indices.size());
	for (int i = 0; i < max; i++) {
		const int idx = qrand() % indices.size();
		const int itemIdx = indices.takeAt(idx);
		//qDebug() << "classifying" << input.at(itemIdx);
		const QVector<nnreal> was = nn2.classifyVec(input.at(itemIdx));
		const QVector<nnreal> expected = output.at(itemIdx);
		const int wc = nn2.outputVectorToLabel(was);
		const int ec = nn2.outputVectorToLabel(expected);
		if (wc == ec) {
			correct++;
		}
		qDebug() << itemIdx << "was" << was << "(" << wc << "), expected:" << expected << "(" << ec << ")";
	}
	qDebug() << "correct:" << correct << "out of" << max << "=" << qreal(correct) / qreal(max) * 100;
	qDebug() << "network saved to" << outputFilename;
	QFile trainCompareFile("train_compare.log");
	trainCompareFile.open(QIODevice::WriteOnly);
	QTextStream out(&trainCompareFile);
	out << nn;
	trainCompareFile.close();
	return 0;
}
