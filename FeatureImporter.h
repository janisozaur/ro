#ifndef FEATUREIMPORTER_H
#define FEATUREIMPORTER_H

#include "NeuralNetwork/NeuralNetworkCommon.h"

#include <QByteArray>
#include <QStringList>
#include <QVector>

class QIODevice;

class FeatureImporter
{
public:
    FeatureImporter();
	FeatureImporter(QIODevice *data);
    ~FeatureImporter();
    QVector<nnreal> featuresForItem(const quint32 itemNumber) const;
	const float *features() const;
	QString labelForItem(const quint32 itemNumber) const;
	QStringList labels() const;
    quint8 classIdForItem(const quint32 itemNumber) const;
    QVector<quint8> classesId() const;
    void synchronizeClassId(const FeatureImporter &other);
	quint32 featuresPerItem() const;
	quint32 itemCount() const;
	QString name() const;
    void open(QIODevice *data);

private:
	float *mData;
	QStringList mLabels;
	quint32 mItemCount;
	quint32 mFeatureCount;
	QString mName;
    QVector<QString> mClassesHash;
    QVector<quint8> mClassesId;

};

#endif // FEATUREIMPORTER_H
