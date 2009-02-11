#include "loader/xmlloader.h"

#include <iostream>
#include <QFile>
#include <boost/shared_ptr.hpp>

int main() {
	QFile f("models/test.xml");
	f.open(QIODevice::ReadOnly);
	XmlLoader loader(f);
	boost::shared_ptr<Model> m = loader.loadModel();
}
