#include "FeatureImporter.h"

#include <QIODevice>
#include <QTextStream>

#include <QDebug>

FeatureImporter::FeatureImporter(QIODevice *data)
{
	QTextStream stream(data);
	mName = stream.readLine();
	stream >> mItemCount;
	stream >> mFeatureCount;
	mData = new float[mItemCount * mFeatureCount];
	for (unsigned int i = 0; i < mItemCount; i++) {
		QString label;
		stream >> label;
		for (unsigned int j = 0; j < mFeatureCount; j++) {
			stream >> mData[i * mFeatureCount + j];
		}
	}
}

FeatureImporter::~FeatureImporter()
{
	delete [] mData;
}

const float *FeatureImporter::featuresForItem(const quint32 itemNumber) const
{
	return mData + itemNumber * mFeatureCount;
}

const float *FeatureImporter::features() const
{
	return mData;
}

QString FeatureImporter::labelForItem(const quint32 itemNumber) const
{
	return mLabels.at(itemNumber);
}

QStringList FeatureImporter::labels() const
{
	return mLabels;
}

quint32 FeatureImporter::featuresPerItem() const
{
	return mFeatureCount;
}

quint32 FeatureImporter::itemCount() const
{
	return mItemCount;
}

QString FeatureImporter::name() const
{
	return mName;
}
