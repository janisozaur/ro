#ifndef NEURON_H
#define NEURON_H

#include "../../common/TypeCommon.h"

#include <QVector>

class Connection;

class Neuron
{
public:
    enum ActivationType {
        Tanh,
        Sigmoid
    };

    Neuron();
    ~Neuron();

    void addConnection(const int &neuronIndex, const nnreal &weight);
    void addConnection(const int &neuronIndex);
    int connectionsCount() const;
    Connection *connection(const int &connection) const;

    void randomize();
    ActivationType activationType() const;
    void setActivationType(ActivationType type);
    nnreal output() const;
    void updateOutput(const nnreal &input);
    nnreal delta() const;
    void updateDelta(const nnreal &input);
    nnreal bias() const;
    void setBias(const nnreal &newBias);
    nnreal biasDelta() const;
    void setBiasDelta(const nnreal &newDelta);

private:
    nnreal mOutput;
    nnreal mBias;
    nnreal mDelta;
    nnreal mBiasDelta;
    QVector<Connection *> mConnections;
    ActivationType mActivationType;
};

#endif // NEURON_H
