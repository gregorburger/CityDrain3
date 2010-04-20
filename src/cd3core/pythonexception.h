#ifndef PYTHONEXCEPTION_H
#define PYTHONEXCEPTION_H

#include <string>
#include <cd3globals.h>

class CD3_PUBLIC PythonException {
public:
	PythonException() {}
	PythonException(std::string error);
	std::string type;
	std::string value;
	std::string traceback;
};

#endif // PYTHONEXCEPTION_H
