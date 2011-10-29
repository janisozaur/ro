#ifndef FEATUREIMPORTER_H
#define FEATUREIMPORTER_H

#include <QByteArray>
#include <QStringList>

class QIODevice;

class FeatureImporter
{
public:
    FeatureImporter();
	FeatureImporter(QIODevice *data);
	~FeatureImporter();
	const float *featuresForItem(const quint32 itemNumber) const;
	const float *features() const;
	QString labelForItem(const quint32 itemNumber) const;
	QStringList labels() const;
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

};

#endif // FEATUREIMPORTER_H
