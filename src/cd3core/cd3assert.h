#ifndef CD3ASSERT_H
#define CD3ASSERT_H

#include <cstdlib>
#include <logger.h>

#define cd3assert(expr, message) if (!(expr)) { \
	Logger(Debug) << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ \
			<< " assert:" << message; \
	std::abort(); \
	}

#endif // CD3ASSERT_H
