#ifndef NAIVECLASSIFIER_H
#define NAIVECLASSIFIER_H

#include "ClassifierInterface.h"
#include "FeatureImporter.h"

class NaiveClassifier : public ClassifierInterface
{
public:
    NaiveClassifier(const qint8 &k, const FeatureImporter &trainData);
    qint8 classify(const float *testFeatures);
    QVector<qint8> fullClassification() const;

private:
    const FeatureImporter mTrainData;
    const float *mTrainFeatures;
    QVector<qint8> mClassificationTemp;
    QList<qint8> mKValues;
    const quint32 mTrainItemCount;
    const quint32 mFeaturesPerItem;
    const qint8 *mTrainClasses;
};

#endif // NAIVECLASSIFIER_H
