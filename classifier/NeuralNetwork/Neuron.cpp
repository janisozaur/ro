#include "Neuron.h"
#include "Connection.h"

#include <cmath>

Neuron::Neuron() :
	mOutput(nnreal(0.0)), mDelta(nnreal(0.0)), mBiasDelta(nnreal(0.0))
{
	randomize();
}

Neuron::~Neuron()
{
	for (int i = 0; i < mConnections.size(); i++) {
		delete mConnections.at(i);
	}
}

Neuron::ActivationType Neuron::activationType() const
{
	return mActivationType;
}

void Neuron::setActivationType(ActivationType type) {
	mActivationType = type;
}

void Neuron::addConnection(const int &neuronIndex, const nnreal &weight)
{
	Connection *newConnection = new Connection(neuronIndex, weight);
	mConnections.push_back(newConnection);
}

void Neuron::addConnection(const int &neuronIndex)
{
	Connection *newConnection = new Connection(neuronIndex);
	mConnections.push_back(newConnection);
}

int Neuron::connectionsCount() const
{
	return mConnections.size();
}

Connection *Neuron::connection(const int &_connection)
{
	return mConnections[_connection];
}

void Neuron::randomize()
{
	mBias = ((nnreal)qrand() / ((nnreal)(RAND_MAX))) * 2 - 1;
}

nnreal Neuron::output() const
{
	return mOutput;
}

void Neuron::updateOutput(const nnreal &input)
{
	switch (mActivationType) {
		default:
		case Neuron::Tanh:
			mOutput = tanh(input);
			break;
		case Neuron::Sigmoid:
			mOutput = nnreal(1) / nnreal(1 + exp(-input));
			break;
	}
}

nnreal Neuron::delta() const
{
	return mDelta;
}

void Neuron::updateDelta(const nnreal &error)
{
	switch (mActivationType) {
		default:
		case Neuron::Tanh:
			mDelta = 1 - mOutput * mOutput;
			break;
		case Neuron::Sigmoid:
			mDelta = mOutput * (1 - mOutput);
			break;
	}
	mDelta *= error;
}

nnreal Neuron::bias() const
{
	return mBias;
}

void Neuron::setBias(const nnreal &newBias)
{
	mBias = newBias;
}

nnreal Neuron::biasDelta() const
{
	return mBiasDelta;
}

void Neuron::setBiasDelta(const nnreal &newDelta)
{
	mBiasDelta = newDelta;
}
