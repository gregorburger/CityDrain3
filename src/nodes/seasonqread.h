#ifndef SEASONQREAD_H
#define SEASONQREAD_H

#include <node.h>
#include <string>
#include <queue>
#include <QtCore/QDir>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QFile>

#include "flow.h"


class SeasonBuffer{
public:
	SeasonBuffer(int dt, double *data);
	~SeasonBuffer();
	void addtoOut(int *dt, Flow *out);

private:
	double *data;
	int dt;

};

class SeasonContext{
public:
	SeasonContext();
	~SeasonContext();
	void take(int dt, Flow *out);
	void put(int dt, double *data);
	int available();
	void clear();
private:
	std::queue<SeasonBuffer*> buffer;
	int availableDt;
};


typedef QPair<int, int> MonthDay;

CD3_DECLARE_NODE(SeasonQRead)
public:
	SeasonQRead();
	virtual ~SeasonQRead();
	bool init(ptime start, ptime end, int dt);
	void deinit();
	int f(ptime time, int dt);
private:
	void parseLine(int month, int day, double *data, int *dt);

    QDir folder;
    QMap<MonthDay, QString> foldermap;
	Flow out;
    std::string rain_folder;
	QFile *file;
	MonthDay lastDay;
	SeasonContext ctxt;
};

#endif // SEASONQREAD_H
