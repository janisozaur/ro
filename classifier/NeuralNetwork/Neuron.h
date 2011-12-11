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
    void updateDelta(nnreal error);
    nnreal bias() const;
    void setBias(const nnreal &newBias);

private:
    nnreal mOutput;
    nnreal mBias;
    nnreal mDelta;
    QVector<Connection *> mConnections;
    ActivationType mActivationType;
};

#endif // NEURON_H
