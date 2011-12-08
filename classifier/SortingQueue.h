#ifndef SORTINGQUEUE_H
#define SORTINGQUEUE_H

#include <QLinkedList>
#include <QPair>
#include <QVector>

class SortingQueue
{
public:
    SortingQueue(const qint8 &size);
    void tryAdd(const QPair<float, quint8> &element);
    QVector<QPair<float, quint8> > toVector() const;

private:
    QLinkedList<QPair<float, quint8> > mList;
    const qint8 mSize;
    float mFarthest;
};

#endif // SORTINGQUEUE_H
