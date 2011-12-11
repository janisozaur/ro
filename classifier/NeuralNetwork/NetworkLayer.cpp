#include "NetworkLayer.h"
#include "Neuron.h"
#include "Connection.h"

NetworkLayer::NetworkLayer(const NetworkLayer *previous,
                           Neuron::ActivationType activationType) :
    mPreviousLayer(previous),
    mActivationType(activationType)
{
}

NetworkLayer::~NetworkLayer()
{
    for (int i = 0; i < mNeurons.size(); i++) {
        delete mNeurons.at(i);
    }
}

Neuron *NetworkLayer::addNeuron(Neuron *neuron)
{
    if (neuron == NULL) {
        neuron = new Neuron();
    }
    neuron->setActivationType(mActivationType);
    mNeurons.push_back(neuron);
    return neuron;
}

void NetworkLayer::addNeurons(const int &neuronsCount)
{
    mNeurons.resize(0);
    for (int i = 0; i < neuronsCount; i++) {
        Neuron *n = addNeuron();
        if (mPreviousLayer != NULL) {
            for (int j = 0; j < mPreviousLayer->neuronsCount(); j++) {
                n->addConnection(j);
            }
        }
    }
}

Neuron *NetworkLayer::neuron(const int &neuronIndex) const
{
    return mNeurons.at(neuronIndex);
}

int NetworkLayer::neuronsCount() const
{
    return mNeurons.size();
}

const NetworkLayer *NetworkLayer::previousLayer() const
{
    return mPreviousLayer;
}

void NetworkLayer::calculate()
{
    for (int i = 0; i < mNeurons.size(); i++) {
        Neuron *neuron = mNeurons.at(i);
        nnreal sum = neuron->bias();
        for (int j = 0; j < neuron->connectionsCount(); j++) {
            Connection *conn = neuron->connection(j);
            const nnreal weight = conn->weight();
            const int idx = conn->neuronIndex();
            const nnreal output = mPreviousLayer->neuron(idx)->output();
            sum += weight * output;
        }
        neuron->updateOutput(sum);
    }
}

QVector<nnreal> NetworkLayer::backprop(const QVector<nnreal> &thisLayerError,
                                       const nnreal &learningRate,
                                       const nnreal &momentum)
{
    QVector<nnreal> result(mPreviousLayer->neuronsCount());

    for (int i = 0; i < mNeurons.size(); i++) {
        Neuron *neuron = mNeurons.at(i);

        neuron->updateDelta(thisLayerError.at(i));

        for (int j = 0; j < neuron->connectionsCount(); j++) {
            Connection *conn = neuron->connection(j);
            result[conn->neuronIndex()] += neuron->delta() * conn->weight();

            const nnreal momentumCoef = momentum*conn->weightDelta();
            const int idx = conn->neuronIndex();
            const nnreal prevNeuronOut = mPreviousLayer->neuron(idx)->output();
            const nnreal lrCoef = prevNeuronOut * neuron->delta() * learningRate;
            const nnreal weightDelta = momentumCoef + lrCoef;
            conn->setWeightDelta(weightDelta);
            conn->setWeight(conn->weight() + weightDelta);
        }
        neuron->setBias(neuron->bias() + (learningRate * neuron->delta()));
    }
    return result;
}
