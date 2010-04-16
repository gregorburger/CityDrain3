#ifndef PYTHONEXCEPTION_H
#define PYTHONEXCEPTION_H

#include <string>

class PythonException {
public:
	PythonException(std::string error);
	std::string type;
	std::string value;
	std::string traceback;
};

#endif // PYTHONEXCEPTION_H
