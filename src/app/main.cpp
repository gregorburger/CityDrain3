#include <saxloader.h>
#include <simulation.h>
#include <cd3assert.h>
#include <mapbasedmodel.h>
#include <log.h>
#include <logger.h>

#include <iostream>
#include <fstream>
#include <QFile>
#include <QTime>
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
		lastp = 0;
		length = sp.stop - sp.start;
		count = 0;
		t = QTime::currentTime();
	}
	void operator()(ISimulation *s, int time) {
		(void) s;
		int newp = (time / length) * 100;
		count ++;
		if (newp <= lastp)
			return;
		QTime tmp_t(QTime::currentTime());
		Logger(Standard) << "Progress:" << newp << "%" << count << "dt:" << t.msecsTo(tmp_t);
		lastp = newp;
		count = 0;
		t = tmp_t;
	}
	double pfactor;
	int lastp;
	int count;
	float length;
	QTime t;
	SimulationParameters sp;
};

int main(int argc, char **argv) {
	po::options_description desc("CityDrain3 command line options");

	desc.add_options()
		("help,h", "produce help message")
		("state-dir,d", po::value<std::string>(), "used to store and locate restart data")
		("restart,r", po::value<int>(), "specifiy the time where to restart")
		("log,l", po::value<std::string>(), "write log to specified file")
		("maxlog,v", po::value<int>(), "secifiy the max loglevel\n0 all debug\n1 all standard\n2 all warnings\n3 only errors")
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

	Log::init(out, max);
	Logger(Debug) << "starting";

	Logger(Debug) << "loading file model:" << vm["model"].as<std::string>();

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
		Logger(Standard) << "restarting from" << starttime
				<< "using state dir:" << vm["state-dir"].as<std::string>();
		s->deserialize(vm["state-dir"].as<std::string>(), starttime);
	}

	s->timestep_before.connect(ProgressHandler(s));
	s->start(starttime);
	Logger(Debug) << "shutting down";
	Log::shutDown();
	delete s;
	return 0;
}
