#ifndef KNNCLASSIFIER_H
#define KNNCLASSIFIER_H

#include "ClassifierInterface.h"
#include "FeatureImporter.h"

class KnnClassifier : public ClassifierInterface
{
public:
    KnnClassifier(const qint8 &k, const FeatureImporter &trainData);
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

#endif // KNNCLASSIFIER_H
