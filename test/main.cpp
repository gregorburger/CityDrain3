#include <xmlloader.h>
#include <model.h>

#include <iostream>
#include <QFile>
#include <model.h>

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
	Model *m;
	if (!(m = loader.loadModel())) {
		std::cout << "could not load xml file" << std::endl;
	}
	m->dump();
	delete m;
}
