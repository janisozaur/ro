#include "../classifier/NeuralNetwork/NeuralNetwork.h"
#include "../classifier/FeatureImporter.h"

#include <QtCore/QCoreApplication>
#include <QFile>
#include <QVector>

#include <QDebug>

QVector<nnreal> classToVector(const quint8 &classId)
{
    QVector<nnreal> result(4, -1);
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
    const QStringList args = a.arguments();

    QFile f(args.at(1));
    if (!f.open(QIODevice::ReadOnly)) {
        qCritical("failed to open file");
    }
    FeatureImporter td(&f);
    f.close();
    qDebug() << "teaching ann from extractor" << td.name()
             << "it was run with params" << td.args().join(" ") << ", has"
             << td.itemCount() << "items, each of which has"
             << td.featuresPerItem() << "features";

    NeuralNetwork nn;
    nn.addLayer(td.featuresPerItem());
    nn.addLayer(8);
    nn.addLayer(4);
    QVector<QVector<nnreal> > input;
    QVector<QVector<nnreal> > output;
    input.reserve(td.itemCount());
    output.reserve(td.itemCount());
    for (quint32 i = 0; i < td.itemCount(); i++) {
        const QVector<nnreal> inData = td.featuresForItem(i);
        input.append(inData);
        const QVector<nnreal> outData = classToVector(td.classIdForItem(i));
        output.append(outData);
    }
    nn.train(input, output, 100, 0.8, 0.2, 0.1);

    return 0;
}
