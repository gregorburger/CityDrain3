#include <saxloader.h>
#include <simulation.h>
#include <cd3assert.h>
#include <mapbasedmodel.h>

#include <iostream>
#include <QFile>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

struct PerStateHandler {
	PerStateHandler(const std::string dir) {
		state_dir = dir;
	}
	void operator()(ISimulation *s, int time) {
		(void) time;
		s->serialize(state_dir);
	}
private:
	std::string state_dir;
};

struct ProgressHandler {
	ProgressHandler(ISimulation *sim) {
		sp = sim->getSimulationParameters();
		pfactor = 100.0 / (sp.stop - sp.start);
		lastp = 0;
	}
	void operator()(ISimulation *s, int time) {
		(void) s;
		int newp = (int) round(pfactor * (time - sp.start));
		if (newp == lastp)
			return;
		std::cout << "                  ";
		std::cout << "\r" << newp << "%";
		lastp = newp;
		if (time == sp.stop)
			std::cout << std::endl;
	}
	double pfactor;
	int lastp;
	SimulationParameters sp;
};

int main(int argc, char **argv) {
	po::options_description desc("CityDrain3 command line options");

	desc.add_options()
		("help,h", "produce help message")
		("state-dir,d", po::value<std::string>(), "used to store and locate restart data")
		("restart,r", po::value<int>(), "specifiy the time where to restart")
		("model,m", po::value<std::string>(), "the model to run the simulation");
	;

	po::positional_options_description p;
	p.add("model", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 1;
	}

	if (!vm.count("model")) {
		std::cerr << "provide a model file" << std::endl;
		std::cout << desc << std::endl;
		//std::cout << p << std::endl;
		return -1;
	}

	QFile f(QString::fromStdString(vm["model"].as<std::string>()));

	MapBasedModel m;

	SaxLoader loader(&m);

	ISimulation *s = loader.load(f);
	cd3assert(s, "simulation loading failed");

	if (vm.count("state-dir")) {
		s->timestep_after.connect(PerStateHandler(vm["state-dir"].as<std::string>()));
	}

	s->setModel(&m);

	int starttime = s->getSimulationParameters().start;

	if (vm.count("restart")) {
		if (!vm.count("state-dir")) {
			std::cerr << "provide state-dir with restart option" << std::endl;
			std::cerr << desc << std::endl;
			return -1;
		}
		starttime = vm["restart"].as<int>();
		std::cout << "restarting from " << starttime <<  std::endl;
		s->deserialize(vm["state-dir"].as<std::string>(), starttime);
	}

	s->timestep_after.connect(ProgressHandler(s));
	s->start(starttime);

	std::cout << std::endl; //newline the progress
	//s->serialize("states");
	delete s;
	return 0;
}
