/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

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
