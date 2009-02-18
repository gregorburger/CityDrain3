#include <xmlloader.h>
#include <model.h>

#include <iostream>
#include <QFile>
#include <mapbasedmodel.h>
#include <simulation.h>
#include <set>
#include <boost/foreach.hpp>
#include <node.h>

void usage(int argc, char **argv) {
	(void) argc;
	std::cout << "usage: " << argv[0] << " model.xml" << std::endl;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		usage(argc, argv);
		return -1;
	}
	QFile f(argv[1]);

	MapBasedModel m;
	Simulation s;

	XmlLoader loader(&s, &m);

	if (!loader.load(f)) {
		std::cout << "could not load xml file" << std::endl;
	}

	BOOST_FOREACH(Node *n, m.sinkNodes()) {
		std::cout << "sink: " << n->getNodeName() << std::endl;
	}

	BOOST_FOREACH(Node *n, m.sourceNodes()) {
		std::cout << "source: " << n->getNodeName() << std::endl;
	}

	s.start(&m);
}
