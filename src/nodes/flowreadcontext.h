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
