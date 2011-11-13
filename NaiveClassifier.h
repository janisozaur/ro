#ifndef NAIVECLASSIFIER_H
#define NAIVECLASSIFIER_H

#include "ClassifierInterface.h"

class NaiveClassifier : public ClassifierInterface
{
public:
    NaiveClassifier();
    QVector<QVector<int> > classify(const float *trainFeatures,
                          const float *testFeatures,
                          const qint8 *trainClasses,
                          const qint8 *testClasses,
                          const quint32 featuresPerItem,
                          const quint32 trainItemCount,
                          const quint32 testItemCount,
                          const QList<int> k);
};

#endif // NAIVECLASSIFIER_H
