#include "SortingQueue.h"
#include "../common/CommonDefines.h"

SortingQueue::SortingQueue(const qint8 &size) :
    mSize(size), mFarthest(0.0f)
{
}

void SortingQueue::tryAdd(const QPair<float, quint8> &element)
{
    if (mList.size() < mSize) {
        if (mList.size() == 0) {
            mList.append(element);
            return;
        } else {
            QLinkedList<QPair<float, quint8> >::iterator it = mList.begin();
            for (; it != mList.end(); ++it) {
                if (element < (*it)) {
                    mList.insert(it, element);
                    return;
                }
            }
            mList.insert(it, element);
        }
    } else if (element.first < mList.last().first) {
        mList.removeLast();

        QLinkedList<QPair<float, quint8> >::iterator it = mList.begin();
        for (; it != mList.end(); ++it) {
            if (element < (*it)) {
                mList.insert(it, element);
                return;
            }
        }
        mList.insert(it, element);
    }
}

QVector<QPair<float, quint8> > SortingQueue::toVector() const
{
    QVector<QPair<float, quint8> > result;
#ifdef HAS_VECTOR_RESERVE
    result.reserve(mList.size());
#endif
    for (QLinkedList<QPair<float, quint8> >::const_iterator it = mList.constBegin(); it != mList.constEnd(); it++) {
        result.append(*it);
    }
    return result;
}
