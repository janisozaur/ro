#include "SortItem.h"

bool SortItem::operator <(const SortItem &other) const
{
	return mDistance < other.mDistance;
}
