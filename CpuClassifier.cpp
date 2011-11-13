#include "CpuClassifier.h"
#include "SortItem.h"

#include <cmath>
#include <QVector>
#include <vector>
#include <QPair>
#include <QHash>
#include <omp.h>

#include <QDebug>

#define T 3
#define SSE_VECT

CpuClassifier::CpuClassifier()
{
}

QVector<QVector<int> > CpuClassifier::classify(const float *trainFeatures,
									 const float *testFeatures,
                                     const qint8 *trainClasses,
                                     const qint8 */*testClasses*/,
									 const quint32 featuresPerItem,
									 const quint32 trainItemCount,
									 const quint32 testItemCount,
									 const QList<int> k)
{
    int chunks = 1;
    float *dist = new float[(testItemCount / chunks) * trainItemCount];
    QVector<SortItem> *resultArr = new QVector<SortItem>[testItemCount];
    for (quint32 i = 0; i < testItemCount; i++) {
        resultArr[i].reserve(k.at(0));
    }

    for (int c = 0; c < chunks; c++) {
        qDebug() << "chunk" << c;
        for (quint32 i = 0; i < (testItemCount / chunks) * trainItemCount; i++) {
            dist[i] = 0;
        }
		quint32 start_i = (testItemCount / chunks) * c;
		quint32 stop_i = (testItemCount / chunks) * (c + 1);
#pragma omp parallel for
        for (quint32 i = start_i; i < stop_i; i++) {
            QVector<SortItem> sortVec;
            sortVec.reserve(trainItemCount);
            //qDebug() << "testItem start" << i;
            for (quint32 j = 0; j < trainItemCount; j++) {
                SortItem si;
#ifdef SSE_VECT
                for (quint32 k = 0; k < featuresPerItem; k += 4) {
#else
                for (quint32 k = 0; k < featuresPerItem; k++) {
#endif
                    float temp1 = fabs(testFeatures[i * featuresPerItem + k] - trainFeatures[j * featuresPerItem + k]);
#ifdef SSE_VECT
                    float temp2 = fabs(testFeatures[i * featuresPerItem + k + 1] - trainFeatures[j * featuresPerItem + k + 1]);
                    float temp3 = fabs(testFeatures[i * featuresPerItem + k + 2] - trainFeatures[j * featuresPerItem + k + 2]);
                    float temp4 = fabs(testFeatures[i * featuresPerItem + k + 3] - trainFeatures[j * featuresPerItem + k + 3]);
#endif
                    temp1 = temp1 * temp1 * temp1;
#ifdef SSE_VECT
                    temp2 = temp2 * temp2 * temp2;
                    temp3 = temp3 * temp3 * temp3;
                    temp4 = temp4 * temp4 * temp4;
#endif
                    //dist[i * trainItemCount + j] += pow(fabs(testFeatures[i * featuresPerItem + k] - trainFeatures[j * featuresPerItem + k]), 3.0f);
#ifdef SSE_VECT
                    dist[(i - (testItemCount / chunks) * c) * trainItemCount + j] += temp1 + temp2 + temp3 + temp4;
#else
                    dist[(i - (testItemCount / chunks) * c) * trainItemCount + j] += temp1;
#endif
                }
                dist[(i - (testItemCount / chunks) * c) * trainItemCount + j] = pow(dist[(i - (testItemCount / chunks) * c) * trainItemCount + j], 1.0f / 3.0f);
                si.mDistance = dist[(i - (testItemCount / chunks) * c) * trainItemCount + j];
                si.mClass = trainClasses[j];
                sortVec.append(si);
            }
            qSort(sortVec);
            int count = sortVec.size() - k.at(0);
            sortVec.remove(k.at(0), count);

            resultArr[i] = sortVec;
            resultArr[i].squeeze();
            //qDebug() << "testItem stop" << i;
        }
    }
    delete [] dist;

    QVector<QVector<int> > result;
    result.resize(k.size());
    for (int i = 0; i < k.size(); i++) {
        result[i].reserve(testItemCount);
    }

    for (int i = 0; i < k.size(); i++) {
        QVector<int> resTemp;
        resTemp.reserve(testItemCount);
        for (quint32 w = 0; w < testItemCount; w++) {
            QHash<int, int> h;
            for (int j = 0; j < k.at(i); j++) {
                h[resultArr[w].at(j).mClass] += 1;
            }
            QVector<QPair<int, int> > pairVec;
            pairVec.reserve(k.at(i));
            for (QHash<int, int>::const_iterator it = h.begin(); it != h.end(); it++) {
                pairVec.append(qMakePair(it.value(), it.key()));
            }
            qSort(pairVec);
            resTemp << pairVec.at(0).second;
        }
        result[i] = resTemp;
    }

    delete [] resultArr;
    return result;
}
