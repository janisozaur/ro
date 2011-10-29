#ifndef CLASSIFIERINTERFACE_H
#define CLASSIFIERINTERFACE_H

#include <QStringList>

class ClassifierInterface
{
public:
	ClassifierInterface();
    virtual QVector<QVector<int> > classify(const float *trainFeatures,
								  const float *testFeatures,
                                  const qint8 *trainClasses,
                                  const qint8 *testClasses,
								  const quint32 featuresPerItem,
								  const quint32 trainItemCount,
								  const quint32 testItemCount,
								  const QList<int> k) = 0;
};

#endif // CLASSIFIERINTERFACE_H
