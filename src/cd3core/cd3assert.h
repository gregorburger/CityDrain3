#ifndef CD3ASSERT_H
#define CD3ASSERT_H

#include <cstdlib>
#include <logger.h>
#ifdef DEBUG
#define cd3assert(expr, message) if (!(expr)) { \
	Logger(Error) << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ \
			<< " assert:" << message; \
	std::abort(); \
	}
#else
#define cd3assert(expr, message) (void) (expr);
#endif

#endif // CD3ASSERT_H
