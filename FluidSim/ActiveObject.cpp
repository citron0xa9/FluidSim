
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

void ActiveObject::deregisterContainerObjectHooks()
{
	m_ContainerObjectPtr->removeActiveObject(*this);
}
