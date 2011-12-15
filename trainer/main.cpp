#include "../classifier/NeuralNetwork/NeuralNetwork.h"
#include "../classifier/FeatureImporter.h"
#include "../common/CommonDefines.h"

#include <QtCore/QCoreApplication>
#include <QFile>
#include <QVector>
#include <QDateTime>

#include <QDebug>

QVector<nnreal> classToVector(const quint8 &classId, const int &initial)
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
    result[idx] = 1;
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
    const QString outputFilename = args.at(2);
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
    nn.addLayer(td.featuresPerItem(), Neuron::Sigmoid);
    nn.addLayer(td.featuresPerItem() / 2, Neuron::Sigmoid);
    nn.addLayer(4, Neuron::Sigmoid);
    QVector<QVector<nnreal> > input;
    QVector<QVector<nnreal> > output;
#ifdef HAS_VECTOR_RESERVE
    input.reserve(td.itemCount());
    output.reserve(td.itemCount());
#endif
    for (quint32 i = 0; i < td.itemCount(); i++) {
        const QVector<nnreal> inData = td.featuresForItem(i);
        input.append(inData);
        const QVector<nnreal> outData = classToVector(td.classIdForItem(i), 0);
        output.append(outData);
    }
    const QVector<float> errors = nn.train(input, output, maxEpochs, lr, momentum, desiredError);
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
    int correct = 0;
    for (int i = 0; i < 100; i++) {
        const int idx = qrand() % indices.size();
        const int itemIdx = indices.takeAt(idx);
        const QVector<nnreal> was = nn.classifyVec(input.at(itemIdx));
        const QVector<nnreal> expected = output.at(itemIdx);
        const int wc = nn.outputVectorToLabel(was);
        const int ec = nn.outputVectorToLabel(expected);
        if (wc == ec) {
            correct++;
        }
        qDebug() << itemIdx << "was" << was << "(" << wc << "), expected:" << expected << "(" << ec << ")";
    }
    qDebug() << "correct:" << correct;
    QFile outputFile(outputFilename);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        qCritical() << "failed to open file" << outputFile.fileName();
        return -2;
    }
    QDataStream ds(&outputFile);
    ds << nn;
    outputFile.close();
    qDebug() << "network saved to" << outputFilename;

    return 0;
}
