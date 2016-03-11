#pragma once

#include <cstdint>
#include <climits>
#include <stdexcept>

namespace fsmath {
	#define MAX_UINT_POWER2 (1 << (sizeof(unsigned int) * 8 - 1))

	inline unsigned int NextPowerOf2(unsigned int x)
	{
		if (x > MAX_UINT_POWER2) {
			std::runtime_error("NextPowerOf2: given value is too large.");
		}

		unsigned int result = 1;
		while (x > result) {
			result <<= 1;
		}

		return result;
	}

	inline bool AnyLess(const glm::vec3 &v1, const glm::vec3 &v2) {
		return ((v1.x < v2.x)
			|| (v1.y < v2.y)
			|| (v1.z < v2.z));
	}
}