#ifndef CLASSIFIERINTERFACE_H
#define CLASSIFIERINTERFACE_H

#include <QStringList>

class ClassifierInterface
{
public:
    ClassifierInterface();
    virtual qint8 classify(const float *testFeatures) = 0;
};

#endif // CLASSIFIERINTERFACE_H
