#ifndef CLASSIFIERINTERFACE_H
#define CLASSIFIERINTERFACE_H

#include <QStringList>

class ClassifierInterface
{
public:
	ClassifierInterface();
	virtual QVector<int> classify(const float *trainFeatures,
								  const float *testFeatures,
								  const int *trainClasses,
								  const int *testClasses,
								  const quint32 featuresPerItem,
								  const quint32 trainItemCount,
								  const quint32 testItemCount,
								  const QList<int> k) = 0;
};

#endif // CLASSIFIERINTERFACE_H
