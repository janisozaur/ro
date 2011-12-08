#ifndef LABELLEDDATA_H
#define LABELLEDDATA_H

#include <QtGlobal>
#include <QVector>

class LabelledData
{
public:
    LabelledData(const QVector<float> &data = QVector<float>(), const quint8 &dataClass = 0);
    void setData(const QVector<float> &data);
    void setClass(const quint8 &dataClass);
    QVector<float> data() const;
    quint8 dataClass() const;

private:
    QVector<float> mData;
    quint8 mClass;

    friend QDataStream &operator<< (QDataStream &stream, const LabelledData &li);
    friend QDataStream &operator>> (QDataStream &stream, LabelledData &li);
};

QDataStream &operator<< (QDataStream &stream, const LabelledData &li);
QDataStream &operator>> (QDataStream &stream, LabelledData &li);

#endif // LABELLEDDATA_H
