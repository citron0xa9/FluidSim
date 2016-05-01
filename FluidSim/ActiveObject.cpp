
#include "ActiveObject.h"


ActiveObject::ActiveObject(ContainerObject &container) : Object{container}
{
}


ActiveObject::~ActiveObject()
{
}

void ActiveObject::registerContainerObjectHooks()
{
	m_ContainerObjectPtr->addActiveObject(*this);
}
