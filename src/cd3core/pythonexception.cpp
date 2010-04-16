#include "pythonexception.h"
#include <logger.h>
#include <boost/algorithm/string.hpp>
#include <vector>

using namespace boost;

PythonException::PythonException(std::string error) {
	std::vector<string> values;
	algorithm::split(values, error, algorithm::is_any_of("|"));
	assert(values.size() == 3);
	type = values[0];
	value = values[1];
	traceback = values[2];
}
