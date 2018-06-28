#include <iterator>
#include "IndexCombination.h"

bool operator==(const IndexCombination& a, const IndexCombination& b)
{
	return std::equal(std::cbegin(a.indices), std::cend(a.indices), std::cbegin(b.indices));
}