#ifndef FILEOUT_H
#define FILEOUT_H

#include <node.h>

CD3_DECLARE_NODE(FileOut)
public:
	FileOut();
	~FileOut();
	void f(int time, int dt);

private:
	Flow *in;
	std::string *out_file_name;
};

#endif // FILEOUT_H
