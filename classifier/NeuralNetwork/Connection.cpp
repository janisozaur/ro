#include "Connection.h"

#include <QtGlobal>
#include <cstdlib>

Connection::Connection(const int &neuronIndex, const nnreal &weight) :
    mNeuronIndex(neuronIndex),
    mWeight(weight),
    mWeightDelta(nnreal(0.0))
{
}

Connection::Connection(const int &neuronIndex) :
    mNeuronIndex(neuronIndex),
    mWeightDelta(nnreal(0.0))
{
    randomize();
}

void Connection::randomize()
{
    mWeight = ((nnreal)qrand() / ((nnreal)(RAND_MAX))) * 2 - 1;
}

nnreal Connection::weight() const
{
    return mWeight;
}

void Connection::setWeight(const nnreal &weight)
{
    mWeight = weight;
}

nnreal Connection::weightDelta() const
{
    return mWeightDelta;
}

void Connection::setWeightDelta(const nnreal &delta)
{
    mWeightDelta = delta;
}

int Connection::neuronIndex() const
{
    return mNeuronIndex;
}

void Connection::setNeuronIndex(const int &neuronIdx)
{
    mNeuronIndex = neuronIdx;
}
