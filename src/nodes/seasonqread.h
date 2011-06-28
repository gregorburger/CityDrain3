#ifndef SEASONQREAD_H
#define SEASONQREAD_H

#include <node.h>
#include <string>
#include <QtCore/QDir>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QFile>

#include "flow.h"
#include "ixxqbuffer.h"

struct SeasonQReadPrivate;

typedef QPair<int, int> MonthDay;

CD3_DECLARE_NODE(SeasonQRead)
public:
        SeasonQRead();
        virtual ~SeasonQRead();
	bool init(ptime start, ptime end, int dt);
	void deinit();
	int f(ptime time, int dt);
private:
    QPair<int, std::vector<double> > parseLine(int month, int day);
    QDir folder;
    QMap<MonthDay, QString> foldermap;
	Flow out;
    std::string rain_folder;
    QFile *file;
    IxxQBuffer buffer;
    MonthDay lastDay;
};

#endif // SEASONQREAD_H
