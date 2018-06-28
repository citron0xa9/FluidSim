
#include "fsmath.h"
#include "../simulation/UniformGrid.h"

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
				
				currentMatrix = glm::transpose(currentMatrix);

				currentOffset++;
			}
			currentOffset += 2;
		}
		currentOffset += 2 * yOffsetDistance;
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

void fsmath::computeFiniteDifference(UniformGrid<glm::dmat3x3>& jacobianGrid, UniformGrid<glm::dvec3>& velocityGrid,
	const size_t xIndex, const size_t yIndex, const size_t zIndex, const size_t offset,
	const glm::dvec3& doubleCellExtent, const glm::uvec3& pointsAmountMinus1, const size_t yOffsetDistance,
	const size_t zOffsetDistance)
{

    glm::dmat3x3 &currentMatrix = jacobianGrid.atOffset(offset);


    if (xIndex == 0) {
        currentMatrix[0] = (velocityGrid.atOffset(offset + 1) - velocityGrid.atOffset(offset)) / velocityGrid.cellExtent();
    }
    else if (xIndex == pointsAmountMinus1.x) {
        currentMatrix[0] = (velocityGrid.atOffset(offset) - velocityGrid.atOffset(offset - 1)) / velocityGrid.cellExtent();
    }
    else {
        currentMatrix[0] = (velocityGrid.atOffset(offset + 1) - velocityGrid.atOffset(offset - 1)) / doubleCellExtent;
    }


    if (yIndex == 0) {
        currentMatrix[1] = (velocityGrid.atOffset(offset + yOffsetDistance) - velocityGrid.atOffset(offset)) / velocityGrid.cellExtent();
    }
    else if (yIndex == pointsAmountMinus1.y) {
        currentMatrix[1] = (velocityGrid.atOffset(offset) - velocityGrid.atOffset(offset - yOffsetDistance)) / velocityGrid.cellExtent();
    }
    else {
        currentMatrix[1] = (velocityGrid.atOffset(offset + yOffsetDistance) - velocityGrid.atOffset(offset - yOffsetDistance)) / doubleCellExtent;
    }


    if (zIndex == 0) {
        currentMatrix[2] = (velocityGrid.atOffset(offset + zOffsetDistance) - velocityGrid.atOffset(offset)) / velocityGrid.cellExtent();
    }
    else if (zIndex == pointsAmountMinus1.z) {
        currentMatrix[2] = (velocityGrid.atOffset(offset) - velocityGrid.atOffset(offset - zOffsetDistance)) / velocityGrid.cellExtent();
    }
    else {
        currentMatrix[2] = (velocityGrid.atOffset(offset + zOffsetDistance) - velocityGrid.atOffset(offset - zOffsetDistance)) / doubleCellExtent;
    }

    currentMatrix = glm::transpose(currentMatrix);

}

size_t fsmath::nextLargerMultipleOf(const size_t multipleBaseValue, const size_t threshold)
{
    return ( multipleBaseValue * ((threshold / multipleBaseValue) + 1) );
}
