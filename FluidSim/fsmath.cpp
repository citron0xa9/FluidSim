
#include "fsmath.h"


void fsmath::computeJacobian(UniformGrid<glm::dmat3x3>& jacobianGrid, UniformGrid<glm::dvec3>& velocityGrid)
{
	const glm::dvec3 doubleCellExtent = 2.0 * velocityGrid.cellExtent();

	const glm::uvec3 pointsAmountMinus1 = velocityGrid.pointsAmount() - glm::uvec3(1);

	const size_t yOffsetDistance = velocityGrid.pointsAmount().x;
	const size_t zOffsetDistance = velocityGrid.pointsAmount().y * yOffsetDistance;

	//Compute derivatives for interior
	size_t currentOffset = zOffsetDistance + yOffsetDistance + 1;
	for (size_t zIndex = 1; zIndex < pointsAmountMinus1.z; zIndex++) {
		for (size_t yIndex = 1; yIndex < pointsAmountMinus1.y; yIndex++) {
			for (size_t xIndex = 1; xIndex < pointsAmountMinus1.x; xIndex++) {
				glm::dmat3x3 &currentMatrix = jacobianGrid.atOffset(currentOffset);

				//calculate d/dx, d/dy, d/dz.
				//use centered derivation

				// d/dx
				currentMatrix[0] = (velocityGrid.atOffset(currentOffset + 1) - velocityGrid.atOffset(currentOffset - 1)) / doubleCellExtent;
				// d/dy
				currentMatrix[1] = (velocityGrid.atOffset(currentOffset + yOffsetDistance) - velocityGrid.atOffset(currentOffset - yOffsetDistance)) / doubleCellExtent;
				// d/dz
				currentMatrix[2] = (velocityGrid.atOffset(currentOffset + zOffsetDistance) - velocityGrid.atOffset(currentOffset - zOffsetDistance)) / doubleCellExtent;

				currentOffset++;
			}
		}
	}

	//compute derivatives for boundaries
	//+/- X boundary
	currentOffset = 0;
	for (size_t zIndex = 0; zIndex < velocityGrid.pointsAmount().z; zIndex++) {
		for (size_t yIndex = 0; yIndex < velocityGrid.pointsAmount().y; yIndex++) {
			computeFiniteDifference(jacobianGrid, velocityGrid, 0, yIndex, zIndex, currentOffset, doubleCellExtent, pointsAmountMinus1, yOffsetDistance, zOffsetDistance);
			computeFiniteDifference(jacobianGrid, velocityGrid, pointsAmountMinus1.x, yIndex, zIndex, currentOffset + yOffsetDistance - 1, doubleCellExtent, pointsAmountMinus1, yOffsetDistance, zOffsetDistance);
			currentOffset += yOffsetDistance;
		}
	}

	//+/- Y boundary
	currentOffset = 0;
	for (size_t zIndex = 0; zIndex < velocityGrid.pointsAmount().z; zIndex++) {
		for (size_t xIndex = 0; xIndex < velocityGrid.pointsAmount().x; xIndex++) {
			computeFiniteDifference(jacobianGrid, velocityGrid, xIndex, 0, zIndex, currentOffset, doubleCellExtent, pointsAmountMinus1, yOffsetDistance, zOffsetDistance);
			computeFiniteDifference(jacobianGrid, velocityGrid, xIndex, pointsAmountMinus1.y, zIndex, currentOffset + zOffsetDistance - yOffsetDistance, doubleCellExtent, pointsAmountMinus1, yOffsetDistance, zOffsetDistance);
			currentOffset++;
		}
		currentOffset = currentOffset - yOffsetDistance + zOffsetDistance;
	}

	//+/- Z boundary
	currentOffset = 0;
	const size_t gridCapacity = velocityGrid.gridPointCapacity();
	for (size_t yIndex = 0; yIndex < velocityGrid.pointsAmount().y; yIndex++) {
		for (size_t xIndex = 0; xIndex < velocityGrid.pointsAmount().x; xIndex++) {
			computeFiniteDifference(jacobianGrid, velocityGrid, xIndex, yIndex, 0, currentOffset, doubleCellExtent, pointsAmountMinus1, yOffsetDistance, zOffsetDistance);
			computeFiniteDifference(jacobianGrid, velocityGrid, xIndex, yIndex, pointsAmountMinus1.z, currentOffset + gridCapacity - zOffsetDistance, doubleCellExtent, pointsAmountMinus1, yOffsetDistance, zOffsetDistance);
			currentOffset++;
		}
	}
}
