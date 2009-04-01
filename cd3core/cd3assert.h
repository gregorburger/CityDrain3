#ifndef CD3ASSERT_H
#define CD3ASSERT_H

#include <cstdlib>
#include <string.h>
#include <iostream>

/*#define BACK_TRACE_SIZE 100
static void *buffer[BACK_TRACE_SIZE];
static int num_back;*/

//void print_stacktrace();

#ifdef assert
#undef assert
#endif
#define assert(expr, message) if (!(expr)) { \
	std::cerr << __FILE__ << ":" << __LINE__ \
			<< " assert: " << message << std::endl; \
	std::abort(); \
	}

#endif // CD3ASSERT_H
