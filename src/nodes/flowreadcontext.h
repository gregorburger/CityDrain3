#ifndef FLOWREADCONTEXT_H
#define FLOWREADCONTEXT_H


#include <node.h>
#include <string>
#include <queue>
#include <QtCore/QDir>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QFile>
#include <QtCore/QDateTime>

#include <flow.h>


class FlowLine{
public:
	FlowLine(QDateTime timestamp, int dt, double *data);
	~FlowLine();
	int addtoOut(Flow *out, int *dt);
	QDateTime timestamp;
	double *data;
	int dt;
};

class FlowReadContext{
public:
	FlowReadContext();
	~FlowReadContext();
	std::queue<FlowLine*> buffer;
	QFile file;

	bool addLine();
	void setOutPort(Flow *out, QDateTime time, int dt);
	std::string dateformat;
	int availabe;
};

#endif // FLOWREADCONTEXT_H
