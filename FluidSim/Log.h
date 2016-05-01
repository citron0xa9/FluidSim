#pragma once
#include <iostream>

#define DEBUG(s) \
{ \
	std::cout << "DEBUG INFO: " << s << std::endl; \
}

#define WARNING(s) \
{ \
	std::cout << "WARNING: " << s << std::endl; \
}


#define ERROR_MSG(s) \
{ \
	std::cout << "ERROR: " << s << std::endl; \
}