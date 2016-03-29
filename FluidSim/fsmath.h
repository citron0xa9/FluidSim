#pragma once

#include <cstdint>
#include <climits>
#include <stdexcept>
#include <algorithm>

namespace fsmath {
	#define MAX_UINT_POWER2 (1 << (sizeof(unsigned int) * 8 - 1))

	inline float NextPowerOf2(float x)
	{
		float exponent = std::ceil(std::log2f(x));
		return pow(2, exponent);
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

	inline bool insideBox(const glm::vec3 &point, const glm::vec3 &minCorner, const glm::vec3 &extent) {
		return !(AnyLess(point, minCorner) || AnyLess(minCorner + extent, point));
	}
}