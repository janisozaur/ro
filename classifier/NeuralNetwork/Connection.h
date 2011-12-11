#ifndef CONNECTION_H
#define CONNECTION_H

#include "NeuralNetworkCommon.h"

class Connection
{
public:
    Connection(const int &neuronIndex, const nnreal &weight);
    Connection(const int &neuronIndex);

    void randomize();
    int neuronIndex() const;
    void setNeuronIndex(const int &neuronIdx);
    nnreal weight() const;
    void setWeight(const nnreal &weight);
    nnreal weightDelta() const;
    void setWeightDelta(const nnreal &delta);

private:
    int mNeuronIndex;
    nnreal mWeight;
    nnreal mWeightDelta;
};

#endif // CONNECTION_H
