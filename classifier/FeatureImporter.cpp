#include "FeatureImporter.h"

#include <QIODevice>
#include <QTextStream>

#include <QDebug>

FeatureImporter::FeatureImporter()
{
}

FeatureImporter::FeatureImporter(QIODevice *data)
{
    open(data);
}

void FeatureImporter::open(QIODevice *data)
{
    QDataStream stream(data);
    stream.setVersion(QDataStream::Qt_4_6);
    stream.setByteOrder(QDataStream::BigEndian);
    int size;
    stream >> size;
    Q_ASSERT(size == sizeof(nnreal));
    stream >> mExtractorName;
    stream >> mExtractorArgs;
    stream >> mFeatureCount;
    stream >> mItems;
    mFeatures.reserve(mItems.size() * mFeatureCount);
    for (int i = 0; i < mItems.size(); i++) {
        mFeatures << mItems.at(i).data();
    }
}

FeatureImporter::~FeatureImporter()
{
}

QVector<nnreal> FeatureImporter::featuresForItem(const quint32 itemNumber) const
{
    return mItems.at(itemNumber).data();
}

quint8 FeatureImporter::classIdForItem(const quint32 itemNumber) const
{
    return mItems.at(itemNumber).dataClass();
}

quint32 FeatureImporter::featuresPerItem() const
{
    return mFeatureCount;
}

quint32 FeatureImporter::itemCount() const
{
    return mItems.size();
}

QString FeatureImporter::name() const
{
    return mExtractorName;
}

QStringList FeatureImporter::args() const
{
    return mExtractorArgs;
}

const nnreal *FeatureImporter::features() const
{
    return mFeatures.constData();
}
