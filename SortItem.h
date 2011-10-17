#ifndef SORTITEM_H
#define SORTITEM_H

class SortItem
{
public:
	bool operator <(const SortItem &other) const;
	float mDistance;
	int mClass;
};

#endif // SORTITEM_H
