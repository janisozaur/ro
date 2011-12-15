#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include "../ClassifierInterface.h"
#include "../../common/TypeCommon.h"
#include "Neuron.h"

#include <QVector>

class NetworkLayer;

class NeuralNetwork : public ClassifierInterface
{
public:
    NeuralNetwork();
    void addLayer(const int &numNeurons,
                  const Neuron::ActivationType &activationFunctionType = Neuron::Tanh);
    quint8 classify(const QVector<nnreal> &input);
    quint8 outputVectorToLabel(const QVector<nnreal> &output) const;
    QVector<float> train(const QVector<QVector<nnreal> > &train,
               const QVector<QVector<nnreal> > &exOutput,
               const int &maxEpochs, const nnreal &learningRate,
               const nnreal &momentum, const nnreal &desiredError);
    void createOutputVector(const quint8 &label, QVector<nnreal> &outputVec) const;

    QVector<nnreal> classifyVec(const QVector<nnreal> &input);
    nnreal backprop(const QVector<nnreal> &input,
                    const QVector<nnreal> &targetOutput,
                    const nnreal &learningRate, const nnreal &momentum);

    friend QDataStream &operator<<(QDataStream &stream, const NeuralNetwork &nn);
    friend QDataStream &operator>>(QDataStream &stream, NeuralNetwork &nn);

private:
    QVector<NetworkLayer *> mLayers;
    int id;
};

#endif // NEURALNETWORK_H
