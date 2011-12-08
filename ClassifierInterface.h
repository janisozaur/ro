#ifndef CLASSIFIERINTERFACE_H
#define CLASSIFIERINTERFACE_H

#include "NeuralNetwork/NeuralNetworkCommon.h"

#include <QStringList>

class ClassifierInterface
{
public:
    ClassifierInterface();
    virtual quint8 classify(const QVector<nnreal> &tf) = 0;
};

#endif // CLASSIFIERINTERFACE_H
