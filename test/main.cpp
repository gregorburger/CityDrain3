#include <xmlloader.h>
#include <model.h>

#include <iostream>
#include <QFile>
#include <boost/shared_ptr.hpp>

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
	f.open(QIODevice::ReadOnly);
	XmlLoader loader(f);
	boost::shared_ptr<Model> m = loader.loadModel();
	m.get()->dump();
}
