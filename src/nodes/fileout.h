#ifndef FILEOUT_H
#define FILEOUT_H

#include <flow.h>
#include <node.h>
#include <QFile>
#include <QTextStream>

CD3_DECLARE_NODE(FileOut)
public:
	FileOut();
	~FileOut();
	virtual int f(int time, int dt);
	virtual void init(int start, int stop, int dt);
	virtual void deinit();
private:
	Flow in;
	std::string *out_file_name;
	QFile file;
	QTextStream stream;
	bool first_run;
};

#endif // FILEOUT_H
