#include "NaiveClassifier.h"
#include "SortingQueue.h"

#include <cmath>
#include <QVector>
#include <QPair>
#include <QHash>

#define SSE_VECT

NaiveClassifier::NaiveClassifier(const qint8 &k, const FeatureImporter &trainData) :
    mTrainData(trainData), mTrainFeatures(trainData.features()), mTrainItemCount(trainData.itemCount()),
    mFeaturesPerItem(trainData.featuresPerItem()), mTrainClasses(trainData.classesId().data())
{
    for (qint8 i = k; i > 0; i--) {
        mKValues.append(i);
    }
}

qint8 NaiveClassifier::classify(const float *testFeatures)
{
    const float power = 1.0f / 3.0f;
    QVector<qint8> result(mKValues.size());
    qint8 *resultPtr = result.data();
    QVector<QPair<float, qint8> > distances;
    SortingQueue q(mKValues.at(0));
    for (quint32 j = 0; j < mTrainItemCount; j++) {
        float distanceSum = 0;
#ifdef SSE_VECT
        for (quint32 k = 0; k < mFeaturesPerItem; k += 4) {
#else
        for (quint32 k = 0; k < featuresPerItem; k++) {
#endif
            float featureDistance1 = fabs(testFeatures[k] - mTrainFeatures[j * mFeaturesPerItem + k]);
#ifdef SSE_VECT
            float featureDistance2 = fabs(testFeatures[k + 1] - mTrainFeatures[j * mFeaturesPerItem + k + 1]);
            float featureDistance3 = fabs(testFeatures[k + 2] - mTrainFeatures[j * mFeaturesPerItem + k + 2]);
            float featureDistance4 = fabs(testFeatures[k + 3] - mTrainFeatures[j * mFeaturesPerItem + k + 3]);
#endif
            featureDistance1 = featureDistance1 * featureDistance1 * featureDistance1;
#ifdef SSE_VECT
            featureDistance2 = featureDistance2 * featureDistance2 * featureDistance2;
            featureDistance3 = featureDistance3 * featureDistance3 * featureDistance3;
            featureDistance4 = featureDistance4 * featureDistance4 * featureDistance4;
#endif
#ifdef SSE_VECT
            const float dist12 = featureDistance1 + featureDistance2;
            const float dist34 = featureDistance3 + featureDistance4;
            distanceSum += dist12 + dist34;
#else
            distanceSum += featureDistance1;
#endif
        }
        distanceSum = pow(distanceSum, power);
        q.tryAdd(qMakePair(distanceSum, mTrainClasses[j]));
    }
    distances = q.toVector();
    for (int w = 0; w < mKValues.size(); w++) {
        QHash<qint8, int> occurences;
        for (int j = 0; j < mKValues.at(w); j++) {
            occurences[distances.at(j).second] += 1;
        }
        int max = 0;
        qint8 objectClass = occurences.keys().at(0);
        for (int j = 0; j < occurences.size(); j++) {
            if (occurences.values().at(j) > max) {
                max = occurences.values().at(j);
                objectClass = occurences.keys().at(j);
            }
        }
        resultPtr[w] = objectClass;
    }
    mClassificationTemp = result;
    return result.at(0);
}

QVector<qint8> NaiveClassifier::fullClassification() const
{
    return mClassificationTemp;
}
