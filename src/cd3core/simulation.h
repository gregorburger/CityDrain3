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

#ifndef SIMULATION_H
#define SIMULATION_H

#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time.hpp>
using namespace boost;
using namespace boost::posix_time;

#include <cd3globals.h>

class IModel;
class Controller;
class Node;
struct NodeConnection;
class IController;

struct CD3_PUBLIC SimulationParameters {
	SimulationParameters(){}
	SimulationParameters(std::string start,
						 std::string stop,
						 std::string dt) {
		this->start = time_from_string(start);
		this->stop = time_from_string(stop);
		this->dt = lexical_cast<int>(dt);
	}
	SimulationParameters(ptime start,
						 ptime stop,
						 int dt) : start(start), stop(stop), dt(dt) {

	}

	ptime start;
	ptime stop;
	int dt;
};

#define CD3_DECLARE_SIMULATION_NAME(simulation) \
const char *simulation::name = #simulation; \
const char *simulation::getClassName() const { return simulation::name; }

#define CD3_DECLARE_SIMULATION(simulation) \
class CD3_PUBLIC simulation : public ISimulation { \
public: \
	static const char *name; \
	const char *getClassName() const; \
private:

class CD3_PUBLIC ISimulation {
public:
	ISimulation();
	virtual ~ISimulation();
	virtual const char *getClassName() const = 0;
	virtual void setSimulationParameters(const SimulationParameters &params);
	virtual SimulationParameters getSimulationParameters() const;
	virtual void setModel(IModel *model);
	virtual IModel *getModel() const;
	virtual void start(ptime time);
	virtual void stop();

	virtual void serialize(const std::string &dir) const;
	virtual void deserialize(const std::string &dir, ptime time) const;

	virtual int run(ptime time, int dt) = 0;

	virtual NodeConnection *createConnection(Node *source,
											 const std::string &soport,
											 Node *sink,
											 const std::string &siport) const;

	boost::signals2::signal<void (ISimulation *, ptime)> timestep_after;
	boost::signals2::signal<void (ISimulation *, ptime)> timestep_before;

	virtual void addController(IController *controller);

protected:
	SimulationParameters sim_param;
	IModel *model;
	ptime current_time;
	bool running;
	std::vector<IController *> controllers;
};

#endif // SIMULATION_H
