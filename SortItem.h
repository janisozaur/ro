#ifndef SORTITEM_H
#define SORTITEM_H

#include <QtGlobal>

class SortItem
{
public:
	bool operator <(const SortItem &other) const;
	float mDistance;
    qint8 mClass;
};

#endif // SORTITEM_H
