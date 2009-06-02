#ifndef CD3ASSERT_H
#define CD3ASSERT_H

#include <cstdlib>
#include <string.h>
#include <iostream>

#define cd3assert(expr, message) if (!(expr)) { \
	std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ \
			<< " assert: " << message << std::endl; \
	std::abort(); \
	}

#endif // CD3ASSERT_H
