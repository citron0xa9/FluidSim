
#include "fsmath.h"


void fsmath::ComputeJacobian(UniformGrid<glm::mat3x3>& jacobianGrid, UniformGrid<glm::vec3>& velocityGrid)
{
	const glm::vec3 doubleCellExtent = 2.0f * velocityGrid.GetCellExtent();

	const glm::uvec3 pointsAmountMinus1 = velocityGrid.GetPointsAmount() - glm::uvec3(1);

	const size_t yOffsetDistance = velocityGrid.GetPointsAmount().x;
	const size_t zOffsetDistance = velocityGrid.GetPointsAmount().y * yOffsetDistance;

	//Compute derivatives for interior
	size_t offset = zOffsetDistance + yOffsetDistance + 1;
	for (size_t zIndex = 1; zIndex < pointsAmountMinus1.z; zIndex++) {
		for (size_t yIndex = 1; yIndex < pointsAmountMinus1.y; yIndex++) {
			for (size_t xIndex = 1; xIndex < pointsAmountMinus1.x; xIndex++) {
				glm::mat3x3 &currentMatrix = jacobianGrid.AtOffset(offset);

				//calculate d/dx, d/dy, d/dz.
				//use centered derivation

				// d/dx
				currentMatrix[0] = (velocityGrid.AtOffset(offset + 1) - velocityGrid.AtOffset(offset - 1)) / doubleCellExtent;
				// d/dy
				currentMatrix[1] = (velocityGrid.AtOffset(offset + yOffsetDistance) - velocityGrid.AtOffset(offset - yOffsetDistance)) / doubleCellExtent;
				// d/dz
				currentMatrix[2] = (velocityGrid.AtOffset(offset + zOffsetDistance) - velocityGrid.AtOffset(offset - zOffsetDistance)) / doubleCellExtent;

				offset++;
			}
		}
	}

	//compute derivatives for boundaries
	//+/- X boundary
	offset = 0;
	for (size_t zIndex = 0; zIndex < velocityGrid.GetPointsAmount().z; zIndex++) {
		for (size_t yIndex = 0; yIndex < velocityGrid.GetPointsAmount().y; yIndex++) {
			ComputeFiniteDifference(jacobianGrid, velocityGrid, 0, yIndex, zIndex, offset, doubleCellExtent, pointsAmountMinus1, yOffsetDistance, zOffsetDistance);
			ComputeFiniteDifference(jacobianGrid, velocityGrid, pointsAmountMinus1.x, yIndex, zIndex, offset + yOffsetDistance - 1, doubleCellExtent, pointsAmountMinus1, yOffsetDistance, zOffsetDistance);
			offset += yOffsetDistance;
		}
	}

	//+/- Y boundary
	offset = 0;
	for (size_t zIndex = 0; zIndex < velocityGrid.GetPointsAmount().z; zIndex++) {
		for (size_t xIndex = 0; xIndex < velocityGrid.GetPointsAmount().x; xIndex++) {
			ComputeFiniteDifference(jacobianGrid, velocityGrid, xIndex, 0, zIndex, offset, doubleCellExtent, pointsAmountMinus1, yOffsetDistance, zOffsetDistance);
			ComputeFiniteDifference(jacobianGrid, velocityGrid, xIndex, pointsAmountMinus1.y, zIndex, offset + zOffsetDistance - yOffsetDistance, doubleCellExtent, pointsAmountMinus1, yOffsetDistance, zOffsetDistance);
			offset++;
		}
		offset = offset - yOffsetDistance + zOffsetDistance;
	}

	//+/- Z boundary
	offset = 0;
	const size_t gridCapacity = velocityGrid.GetGridPointCapacity();
	for (size_t yIndex = 0; yIndex < velocityGrid.GetPointsAmount().y; yIndex++) {
		for (size_t xIndex = 0; xIndex < velocityGrid.GetPointsAmount().x; xIndex++) {
			ComputeFiniteDifference(jacobianGrid, velocityGrid, xIndex, yIndex, 0, offset, doubleCellExtent, pointsAmountMinus1, yOffsetDistance, zOffsetDistance);
			ComputeFiniteDifference(jacobianGrid, velocityGrid, xIndex, yIndex, pointsAmountMinus1.z, offset + gridCapacity - zOffsetDistance, doubleCellExtent, pointsAmountMinus1, yOffsetDistance, zOffsetDistance);
			offset++;
		}
	}
}
