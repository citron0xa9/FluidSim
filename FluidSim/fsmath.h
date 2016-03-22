#pragma once

#include <cstdint>
#include <climits>
#include <stdexcept>
#include <algorithm>

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

	inline glm::uvec3 allNextPowerOf2(const glm::uvec3 &vec) {
		return glm::uvec3(
			NextPowerOf2(vec.x),
			NextPowerOf2(vec.y),
			NextPowerOf2(vec.z)
			);
	}

	inline bool AnyLess(const glm::vec3 &v1, const glm::vec3 &v2) {
		return ((v1.x < v2.x)
			|| (v1.y < v2.y)
			|| (v1.z < v2.z));
	}

	inline glm::vec3 allMin(const glm::vec3 &v1, const glm::vec3 &v2) {
		return glm::vec3(
			std::min(v1.x, v2.x),
			std::min(v1.y, v2.y),
			std::min(v1.z, v2.z)
			);
	}

	inline glm::vec3 allMax(const glm::vec3 &v1, const glm::vec3 &v2) {
		return glm::vec3(
			std::max(v1.x, v2.x),
			std::max(v1.y, v2.y),
			std::max(v1.z, v2.z)
			);
	}

	inline glm::vec3 allCeil(const glm::vec3 &vec) {
		return glm::vec3(
			std::ceil(vec.x),
			std::ceil(vec.y),
			std::ceil(vec.z)
			);
	}
}