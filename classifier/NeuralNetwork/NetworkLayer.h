#ifndef NETWORKLAYER_H
#define NETWORKLAYER_H

#include "Neuron.h"

#include <stdlib.h>
#include <QVector>

class NetworkLayer
{
public:
    NetworkLayer(const NetworkLayer *previous, Neuron::ActivationType activationType);
    ~NetworkLayer();

    void calculate();
    QVector<nnreal> backprop(const QVector<nnreal> &thisLayerError, const nnreal &learningRate, const nnreal &momentum);

    Neuron *addNeuron(Neuron *neuron = NULL);
    void addNeurons(const int &neuronsCount);
    Neuron *neuron(const int &neuronIndex) const;
    int neuronsCount() const;

    const NetworkLayer *previousLayer() const;

private:
    const NetworkLayer *mPreviousLayer;
    QVector<Neuron *> mNeurons;
    Neuron::ActivationType mActivationType;
};

#endif // NETWORKLAYER_H
