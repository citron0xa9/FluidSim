
#include "DrawableGridGeometry.h"




DrawableGridGeometry::DrawableGridGeometry(const UniformGridGeometry & geometry, ContainerObject & container)
	: UniformGridGeometry(geometry), DrawableObject(container), Object(container)
{

}

DrawableGridGeometry::~DrawableGridGeometry()
{
}
