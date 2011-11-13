#include "NaiveClassifier.h"
#include "SortingQueue.h"

#include <cmath>
#include <QVector>
#include <QPair>
#include <QHash>

#define SSE_VECT

NaiveClassifier::NaiveClassifier()
{
}

QVector<QVector<int> > NaiveClassifier::classify(const float *trainFeatures,
                                     const float *testFeatures,
                                     const qint8 *trainClasses,
                                     const qint8 */*testClasses*/,
                                     const quint32 featuresPerItem,
                                     const quint32 trainItemCount,
                                     const quint32 testItemCount,
                                     const QList<int> k)
{
    const float power = 1.0f / 3.0f;
    QVector<QVector<int> > result(k.size());
    for (int i = 0; i < result.size(); i++) {
        result[i].resize(testItemCount);
    }
    QVector<int> *resultPtr = result.data();
#pragma omp parallel for
    for (quint32 i = 0; i < testItemCount; i++) {
        QVector<QPair<float, qint8> > distances;
        SortingQueue q(k.at(0));
        for (quint32 j = 0; j < trainItemCount; j++) {
            float distanceSum = 0;
#ifdef SSE_VECT
            for (quint32 k = 0; k < featuresPerItem; k += 4) {
#else
            for (quint32 k = 0; k < featuresPerItem; k++) {
#endif
                float featureDistance1 = fabs(testFeatures[i * featuresPerItem + k] - trainFeatures[j * featuresPerItem + k]);
#ifdef SSE_VECT
                float featureDistance2 = fabs(testFeatures[i * featuresPerItem + k + 1] - trainFeatures[j * featuresPerItem + k + 1]);
                float featureDistance3 = fabs(testFeatures[i * featuresPerItem + k + 2] - trainFeatures[j * featuresPerItem + k + 2]);
                float featureDistance4 = fabs(testFeatures[i * featuresPerItem + k + 3] - trainFeatures[j * featuresPerItem + k + 3]);
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
            q.tryAdd(qMakePair(distanceSum, trainClasses[j]));
        }
        distances = q.toVector();
        for (int w = 0; w < k.size(); w++) {
            QHash<qint8, int> occurences;
            for (int j = 0; j < k.at(w); j++) {
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
            resultPtr[w][i] = objectClass;
        }
    }
    return result;
}
