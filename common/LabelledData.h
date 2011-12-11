#ifndef LABELLEDDATA_H
#define LABELLEDDATA_H

#include "TypeCommon.h"

#include <QtGlobal>
#include <QVector>

class LabelledData
{
public:
    LabelledData(const QVector<nnreal> &data = QVector<nnreal>(), const quint8 &dataClass = 0);
    void setData(const QVector<nnreal> &data);
    void setClass(const quint8 &dataClass);
    QVector<nnreal> data() const;
    quint8 dataClass() const;

private:
    QVector<nnreal> mData;
    quint8 mClass;

    friend QDataStream &operator<< (QDataStream &stream, const LabelledData &li);
    friend QDataStream &operator>> (QDataStream &stream, LabelledData &li);
};

QDataStream &operator<< (QDataStream &stream, const LabelledData &li);
QDataStream &operator>> (QDataStream &stream, LabelledData &li);

#endif // LABELLEDDATA_H
