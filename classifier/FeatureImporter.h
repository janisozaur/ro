#ifndef FEATUREIMPORTER_H
#define FEATUREIMPORTER_H

#include "../common/TypeCommon.h"
#include "../common/LabelledData.h"

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
    quint8 classIdForItem(const quint32 itemNumber) const;
    QVector<quint8> classesId() const;
    quint32 featuresPerItem() const;
    quint32 itemCount() const;
    QString name() const;
    QStringList args() const;
    void open(QIODevice *data);
    const nnreal *features()const;

private:
    QString mExtractorName;
    QStringList mExtractorArgs;
    quint32 mFeatureCount;
    QVector<LabelledData> mItems;
    QVector<nnreal> mFeatures;
};

#endif // FEATUREIMPORTER_H
