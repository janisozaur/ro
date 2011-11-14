#include "SortingQueue.h"

SortingQueue::SortingQueue(const qint8 &size) :
    mSize(size), mFarthest(0.0f)
{
}

void SortingQueue::tryAdd(const QPair<float, qint8> &element)
{
    if (mList.size() < mSize) {
        if (mList.size() == 0) {
            mList.append(element);
            return;
        } else {
            auto it = mList.begin();
            for(; it != mList.end(); ++it) {
                if (element < (*it)) {
                    mList.insert(it, element);
                    return;
                }
            }
            mList.insert(it, element);
        }
    } else if (element.first < mList.last().first) {
        mList.removeLast();

        auto it = mList.begin();
        for (; it != mList.end(); ++it) {
            if (element < (*it)) {
                mList.insert(it, element);
                return;
            }
        }
        mList.insert(it, element);
    }
}

QVector<QPair<float, qint8> > SortingQueue::toVector() const
{
    QVector<QPair<float, qint8> > result;
    result.reserve(mList.size());
    for (auto it = mList.constBegin(); it != mList.constEnd(); it++) {
        result.append(*it);
    }
    return result;
}
