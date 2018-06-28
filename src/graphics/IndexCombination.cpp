#include "IndexCombination.h"

bool operator==(const IndexCombination& a, const IndexCombination& b)
{
	return std::equal(std::begin(a.indices), std::end(a.indices), std::begin(b.indices));
}