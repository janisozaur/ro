#include "FeatureImporter.h"

#include <QIODevice>
#include <QTextStream>

#include <QDebug>

FeatureImporter::FeatureImporter() :
    mData(NULL)
{
}

FeatureImporter::FeatureImporter(QIODevice *data)
{
    open(data);
}

void FeatureImporter::open(QIODevice *data)
{
    QTextStream stream(data);
    mName = stream.readLine();
    stream >> mItemCount;
    stream >> mFeatureCount;
    mData = new float[mItemCount * mFeatureCount];
    mLabels.reserve(mItemCount);

    for (unsigned int i = 0; i < mItemCount; i++) {
        QString label;
        stream >> label;
        for (unsigned int j = 0; j < mFeatureCount; j++) {
            stream >> mData[i * mFeatureCount + j];
        }
        mLabels << label;
        qint16 index = mClassesHash.indexOf(label);
        if (index == -1) {
            mClassesHash.append(label);
            index = mClassesHash.size() - 1;
        }
        quint8 idx8 = index;
        mClassesId.append(idx8);
    }
}

FeatureImporter::~FeatureImporter()
{
	delete [] mData;
}

QVector<nnreal> FeatureImporter::featuresForItem(const quint32 itemNumber) const
{
    QVector<nnreal> result;
    result.reserve(mFeatureCount);
    for (quint32 i = 0; i < mFeatureCount; i++) {
        result.append(mData[itemNumber * mFeatureCount + i]);
    }
    return result;
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

quint8 FeatureImporter::classIdForItem(const quint32 itemNumber) const
{
    return mClassesId.at(itemNumber);
}

QVector<quint8> FeatureImporter::classesId() const
{
    return mClassesId;
}

void FeatureImporter::synchronizeClassId(const FeatureImporter &other)
{
    for (int i = 0; i < mClassesId.size(); i++) {
        qint8 newId = other.mClassesHash.indexOf(mLabels.at(i));
        if (newId == -1) {
            qCritical() << "there is no id for class" << mLabels.at(i);
        }
        mClassesId[i] = newId;
    }
    mClassesHash = other.mClassesHash;
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
