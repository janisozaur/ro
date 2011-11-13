#include "NaiveClassifier.h"

#include <cmath>
#include <QVector>
#include <QPair>
#include <QHash>

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
        QVector<QPair<float, qint8> > distances(trainItemCount);
        QPair<float, qint8> *distancesPtr = distances.data();
        for (quint32 j = 0; j < trainItemCount; j++) {
            float distanceSum = 0;
            for (quint32 k = 0; k < featuresPerItem; k++) {
                float featureDistance = fabs(testFeatures[i * featuresPerItem + k] - trainFeatures[j * featuresPerItem + k]);
                featureDistance = featureDistance * featureDistance * featureDistance;
                distanceSum += featureDistance;
            }
            distanceSum = pow(distanceSum, power);
            distancesPtr[j] = qMakePair(distanceSum, trainClasses[j]);
        }
        qSort(distances);
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
