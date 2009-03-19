#include <xmlloader.h>
#include <simulation.h>

#include <iostream>
#include <QFile>
#include <mapbasedmodel.h>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

void progress(float percent) {
	std::cout << "progress: " << percent << std::endl;
}


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

	XmlLoader loader(&m);

	ISimulation *s = loader.load(f);
	assert(s);

	if (vm.count("state-dir")) {
		s->timestep.connect(PerStateHandler(vm["state-dir"].as<std::string>()));
	}

	s->progress.connect(progress);
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

	s->start(starttime);

	std::cout << std::endl; //newline the progress
	//s->serialize("states");
	delete s;
}
