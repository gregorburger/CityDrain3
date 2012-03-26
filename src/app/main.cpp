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

#include <simulation.h>
#include <cd3assert.h>
#include <mapbasedmodel.h>
#include <log.h>
#include <logger.h>
#include <logsink.h>
#include "saxloader.h"
#ifndef PYTHON_DISABLED
#include <pythonexception.h>
#include <noderegistry.h>
#include <QDir>
#endif

#include <iostream>
#include <fstream>
#include <QFile>
#include <QTime>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

using namespace boost::gregorian;

struct PerStateHandler {
	PerStateHandler(const std::string &dir) {
		state_dir = dir;
	}
	void operator()(ISimulation *s, ptime time) {
		(void) time;
		s->serialize(state_dir);
	}
private:
	std::string state_dir;
};

struct ProgressHandler {
	ProgressHandler(ISimulation *sim)
	 : sp(sim->getSimulationParameters()) {
		time_period tp(sp.start, sp.stop);
		long seconds = tp.length().total_seconds();
		pfrac = 100.0/seconds;
		last_perc = -1;
	}
	void operator()(ISimulation *s, ptime time) {
		time_period tp(sp.start, time);
		long time_seconds = tp.length().total_seconds();
		int time_perc = time_seconds*pfrac;
		if (time_perc != last_perc) {
			Logger(Standard) << "Progress:" << time_perc << "%";
			last_perc = time_perc;
		}
	}
	SimulationParameters sp;
	long seconds;
	double pfrac;
	int last_perc;
};

int main(int argc, char **argv) {
	po::options_description desc("CityDrain3 command line options");

	desc.add_options()
		("help,h", "produce help message")
		("state-dir,d", po::value<std::string>(), "used to store and locate restart data")
		("restart,r", po::value<std::string>(), "specifiy the time where to restart (e.g. 2004-Jan-01 00:00:00.00)")
		("log,l", po::value<std::string>(), "write log to specified file")
		("maxlog,v", po::value<int>(), "secifiy the max loglevel\n0 all debug\n1 all standard\n2 all warnings\n3 only errors")
		("model,m", po::value<std::string>(), "the model to run the simulation");
	;

	po::positional_options_description p;
	p.add("model", 1);

	po::variables_map vm;
	try {
		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	} catch (po::unknown_option o) {
		std::cout << "unknown option " << endl << endl;
		std::cout << desc << std::endl;
		return 1;
	}

	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 1;
	}

	if (!vm.count("model")) {
		std::cout << desc << std::endl;
		std::cerr << "provide a model file" << std::endl;
		return -1;
	}

	ostream *out = &cout;
	LogLevel max = Debug;
	if (vm.count("log")) {
		std::string log_file = vm["log"].as<std::string>();
		if (ifstream(log_file.c_str())) {
			cerr << "file already exists " << log_file << endl;
			return -1;
		}
		out = new ofstream(log_file.c_str());
	}

	if (vm.count("maxlog")) {
		int l = vm["maxlog"].as<int>();
		if (l < 0 || l > 3) {
			cerr << desc << endl;
			cerr << "log level must be between 0 an 3" << endl;
			return -1;
		}
		max = LogLevel(l);
	}

	Log::init(new OStreamLogSink(*out), max);
	Logger(Debug) << "starting";

	Logger(Debug) << "loading file model:" << vm["model"].as<std::string>();

	QFile f(QString::fromStdString(vm["model"].as<std::string>()));

	MapBasedModel m;
#ifndef PYTHON_DISABLED
	QFileInfo me(argv[0]);
	NodeRegistry::addToPythonPath(me.dir().path().toStdString());
	try {
#endif
	SaxLoader loader(&m);

	if (!loader.load(f)) {
		Logger(Error) << "error loading model file";
		return -1;
	}

	ISimulation *s = loader.getSimulation();
	cd3assert(s, "simulation loading failed");

	if (vm.count("state-dir")) {
		s->timestep_after.connect(PerStateHandler(vm["state-dir"].as<std::string>()));
	}

	s->setModel(&m);

	ptime starttime = s->getSimulationParameters().start;

	if (vm.count("restart")) {
		if (!vm.count("state-dir")) {
			std::cerr << "provide state-dir with restart option" << std::endl;
			std::cerr << desc << std::endl;
			return -1;
		}
		starttime = time_from_string(vm["restart"].as<std::string>());
		Logger(Standard) << "restarting from" << to_simple_string(starttime)
				<< "using state dir:" << vm["state-dir"].as<std::string>();
		s->deserialize(vm["state-dir"].as<std::string>(), starttime);
	}

	if (max <= Standard) //don't even call the progress handler
		s->timestep_before.connect(ProgressHandler(s));


	s->start(starttime);

	Logger(Debug) << "shutting down";
	//Log::shutDown();
	delete s;
#ifndef PYTHON_DISABLED
	}
	catch (PythonException e) {
		cout << "python exception cought" << endl;
	}
#endif
	return 0;
}
