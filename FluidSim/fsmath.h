#pragma once

#include <cstdint>
#include <climits>
#include <stdexcept>
#include <algorithm>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

#include "UniformGrid.h"

namespace fsmath {
#define MAX_UINT_POWER2 (1 << (sizeof(unsigned int) * 8 - 1))

	inline float NextPowerOf2(float x)
	{
		float exponent = std::ceil(std::log2f(x));
		return static_cast<float>(pow(2, exponent));
	}

	inline glm::uvec3 allNextPowerOf2(const glm::vec3 &vec) {
		return glm::vec3(
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

	void ComputeJacobian(UniformGrid<glm::mat3x3> &jacobianGrid, UniformGrid<glm::vec3> &velocityGrid);

	inline void ComputeFiniteDifference(UniformGrid<glm::mat3x3> &jacobianGrid, UniformGrid<glm::vec3> &velocityGrid,
		const size_t xIndex, const size_t yIndex, const size_t zIndex, const size_t offset, const glm::vec3 &doubleCellExtent,
		const glm::uvec3 &pointsAmountMinus1, const size_t yOffsetDistance, const size_t zOffsetDistance)

	{

		glm::mat3x3 &currentMatrix = jacobianGrid.AtOffset(offset);


		if (xIndex == 0) {
			currentMatrix[0] = (velocityGrid.AtOffset(offset + 1) - velocityGrid.AtOffset(offset)) / velocityGrid.GetCellExtent();
		}
		else if (xIndex == pointsAmountMinus1.x) {
			currentMatrix[0] = (velocityGrid.AtOffset(offset) - velocityGrid.AtOffset(offset - 1)) / velocityGrid.GetCellExtent();
		}
		else {
			currentMatrix[0] = (velocityGrid.AtOffset(offset + 1) - velocityGrid.AtOffset(offset - 1)) / doubleCellExtent;
		}


		if (yIndex == 0) {
			currentMatrix[1] = (velocityGrid.AtOffset(offset + yOffsetDistance) - velocityGrid.AtOffset(offset)) / velocityGrid.GetCellExtent();
		}
		else if (yIndex == pointsAmountMinus1.y) {
			currentMatrix[1] = (velocityGrid.AtOffset(offset) - velocityGrid.AtOffset(offset - yOffsetDistance)) / velocityGrid.GetCellExtent();
		}
		else {
			currentMatrix[1] = (velocityGrid.AtOffset(offset + yOffsetDistance) - velocityGrid.AtOffset(offset - yOffsetDistance)) / doubleCellExtent;
		}


		if (zIndex == 0) {
			currentMatrix[2] = (velocityGrid.AtOffset(offset + zOffsetDistance) - velocityGrid.AtOffset(offset)) / velocityGrid.GetCellExtent();
		}
		else if (zIndex == pointsAmountMinus1.z) {
			currentMatrix[2] = (velocityGrid.AtOffset(offset) - velocityGrid.AtOffset(offset - zOffsetDistance)) / velocityGrid.GetCellExtent();
		}
		else {
			currentMatrix[2] = (velocityGrid.AtOffset(offset + zOffsetDistance) - velocityGrid.AtOffset(offset - zOffsetDistance)) / doubleCellExtent;
		}

	}

}