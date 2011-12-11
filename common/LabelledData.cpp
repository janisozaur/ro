#include "LabelledData.h"

#include <QDataStream>

LabelledData::LabelledData(const QVector<nnreal> &data, const quint8 &imageClass) :
    mData(data),
    mClass(imageClass)
{
}

void LabelledData::setData(const QVector<nnreal> &data)
{
    mData = data;
}

void LabelledData::setClass(const quint8 &dataClass)
{
    mClass = dataClass;
}

QVector<nnreal> LabelledData::data() const
{
    return mData;
}

quint8 LabelledData::dataClass() const
{
    return mClass;
}

QDataStream &operator<< (QDataStream &stream, const LabelledData &li)
{
    stream << li.mData << li.mClass;
    return stream;
}

QDataStream &operator>> (QDataStream &stream, LabelledData &li)
{
    stream >> li.mData >> li.mClass;
    return stream;
}
