#ifndef KNNCLASSIFIER_H
#define KNNCLASSIFIER_H

#include "ClassifierInterface.h"
#include "FeatureImporter.h"
#include "../common/TypeCommon.h"

class KnnClassifier : public ClassifierInterface
{
public:
    KnnClassifier(const qint8 &k, const FeatureImporter &trainData);
    quint8 classify(const QVector<nnreal> &tf);
    QVector<quint8> fullClassification() const;

private:
    const FeatureImporter mTrainData;
    const nnreal *mTrainFeatures;
    QVector<quint8> mClassificationTemp;
    QList<qint8> mKValues;
    const quint32 mTrainItemCount;
    const quint32 mFeaturesPerItem;
};

#endif // KNNCLASSIFIER_H
