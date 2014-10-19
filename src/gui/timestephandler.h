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

#ifndef TIMESTEPHANDLER_H
#define TIMESTEPHANDLER_H

#include <QObject>
#ifndef Q_MOC_RUN
#include <boost/date_time.hpp>
#endif

using namespace boost::posix_time;

class ISimulation;

class TimeStepHandler : public QObject
{
Q_OBJECT
public:
	TimeStepHandler();
	virtual ~TimeStepHandler();
	void after(ISimulation *sim, ptime t);

private:
	int old_progress;
Q_SIGNALS:
	void progress(int percent);
	void finished();
};

#endif // TIMESTEPHANDLER_H
