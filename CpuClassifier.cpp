#include "CpuClassifier.h"
#include "SortItem.h"

#include <cmath>
#include <QVector>
#include <vector>
#include <QPair>
#include <QHash>

#include <QDebug>

#define T 3

CpuClassifier::CpuClassifier()
{
}

QVector<int> CpuClassifier::classify(const float *trainFeatures,
									 const float *testFeatures,
									 const int *trainClasses,
									 const int *testClasses,
									 const quint32 featuresPerItem,
									 const quint32 trainItemCount,
									 const quint32 testItemCount,
									 const QList<int> k)
{
	float *dist = new float[testItemCount * trainItemCount];
	for (quint32 i = 0; i < testItemCount * trainItemCount; i++) {
		dist[i] = 0;
	}
	QVector<int> result;
	QVector<SortItem> sortVec;
	sortVec.reserve(trainItemCount);
	for (quint32 i = 0; i < testItemCount; i++) {
		sortVec.resize(0);
		for (quint32 j = 0; j < trainItemCount; j++) {
			SortItem si;
			for (quint32 k = 0; k < featuresPerItem; k++) {
				dist[i * trainItemCount + j] += pow(fabs(testFeatures[i * featuresPerItem + k] - trainFeatures[j * featuresPerItem + k]), 3.0f);
			}
			dist[i * trainItemCount + j] = pow(dist[i * trainItemCount + j], 1.0f / 3.0f);
			si.mDistance = dist[i * trainItemCount + j];
			si.mClass = trainClasses[j];
			sortVec.append(si);
		}
		qSort(sortVec);
		int count = sortVec.size() - k.at(0);
		sortVec.remove(k.at(0), count);
		QHash<int, int> h;
		for (int j = 0; j < sortVec.size(); j++) {
			h[sortVec.at(j).mClass] += 1;
		}
		QVector<QPair<int, int> > pairVec;
		pairVec.reserve(sortVec.size());
		for (QHash<int, int>::const_iterator it = h.begin(); it != h.end(); it++) {
			pairVec.append(qMakePair(it.value(), it.key()));
		}
		qSort(pairVec);
		result << pairVec.at(0).second;
	}
	delete [] dist;
	return result;
}
