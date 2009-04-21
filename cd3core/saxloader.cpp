#include "saxloader.h"

#include <QLibrary>
#include <QDebug>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;

#include <noderegistry.h>
#include <simulationregistry.h>
#include <typeregistry.h>
#include <model.h>
#include <cd3assert.h>
#include <node.h>
#include <simulation.h>
#include <flow.h>
#include <calculationunit.h>

struct SaxLoaderPriv {
	NodeRegistry node_registry;
	SimulationRegistry sim_registry;
	TypeRegistry type_registry;
	IModel *model;
	ISimulation *simulation;
	Flow f;
	std::string param_name;
};

SaxLoader::SaxLoader(IModel *model) {
	pd = new SaxLoaderPriv();
	pd->model = model;
	pd->simulation = 0;
}

SaxLoader::~SaxLoader() {
	delete pd;
}

bool SaxLoader::startElement(const QString &/*ns*/,
							 const QString &lname,
							 const QString &/*qname*/,
							 const QXmlAttributes &atts) {
	if (lname == "node") {
		std::string id = atts.value("id").toStdString();
		std::string klass = atts.value("class").toStdString();

		current = pd->node_registry.createNode(klass);
		pd->model->addNode(id, current);
		return true;
	}
	if (lname == "sink") {
		sink_id = atts.value("node").toStdString();
		sink_port = atts.value("port").toStdString();
		return true;
	}
	if (lname == "source") {
		source_id = atts.value("node").toStdString();
		source_port = atts.value("port").toStdString();
		return true;
	}
	if (lname == "parameter") {
		loadParameter(atts);
		return true;
	}
	if (lname == "simulation") {
		cd3assert(pd->simulation == 0, "Simulation already set");
		std::string klass = atts.value("class").toStdString();
		pd->simulation = pd->sim_registry.createSimulation(klass);
		return true;
	}
	if (lname == "time") {
		SimulationParameters p;
		p.dt = lexical_cast<int>(atts.value("dt").toStdString());
		p.start = lexical_cast<int>(atts.value("start").toStdString());
		p.stop = lexical_cast<int>(atts.value("stop").toStdString());
		pd->simulation->setSimulationParameters(p);
		return true;
	}
	typedef CalculationUnit CU;
	if (lname == "unit") {
		std::string kind = atts.value("kind").toStdString();
		CU *cunit = CU::fromString(kind);
		cd3assert(cunit != CU::null,
				  str(format("can't determine flow unit type: [%1%]") % kind));
		double value = lexical_cast<double>(atts.value("value").toStdString());
		std::cout << format("unit %1% has value %2%") % atts.value("name").toStdString() % value << std::endl;
		pd->f.addUnit(atts.value("name").toStdString(),
					  cunit,
					  value);
		return true;
	}
	if (lname == "pluginpath") {
		loadPlugin(atts.value("path"));
		return true;
	}
	if (lname == "citydrain") {
		return true;
	}
	if (lname == "model") {
		return true;
	}
	if (lname == "nodelist") {
		return true;
	}
	if (lname == "connectionlist") {
		return true;
	}
	if (lname == "flow") {
		//pd->f = Flow::nullFlow();
		pd->f = Flow();
		return true;
	}
	if (lname == "connection") {
		return true;
	}
	qWarning() << "not used element " << lname;
	return true;
}

ISimulation *SaxLoader::load(QFile &file) {
	bool opened = file.open(QIODevice::ReadOnly);
	cd3assert(opened, "could not open model");
	QXmlSimpleReader r;
	r.setContentHandler(this);
	QXmlInputSource source(&file);
	r.parse(&source);
	return pd->simulation;
}

bool SaxLoader::endElement(const QString &/*ns*/,
						   const QString &lname,
						   const QString &/*qname*/) {

	if (lname == "connection") {
		cd3assert(!source_id.empty(), "source node not set");
		cd3assert(!source_port.empty(), "source port not set");
		cd3assert(!sink_id.empty(), "sink node not set");
		cd3assert(!sink_port.empty(), "sink port not set");
		pd->model->addConnection(source_id,
								 source_port,
								 sink_id,
								 sink_port);
	}
	if (lname == "nodelist") {
		qDebug() << "initing nodes";
		pd->model->initNodes(pd->simulation->getSimulationParameters());
	}
	if (lname == "flow") {
		current->setParameter<Flow>(pd->param_name, pd->f);
	}
	return true;
}

void SaxLoader::loadPlugin(QString path) {
	QLibrary l(path);
	bool loaded = l.load();
	cd3assert(loaded, str(format("could not load plugin %1%") % path.toStdString()));
	regTypeFunProto regTypeFun = (regTypeFunProto) l.resolve("registerTypes");
	if (regTypeFun) {
		regTypeFun(&pd->type_registry);
	} else {
		qWarning() << path << " has no type register hook";
	}
	regSimFunProto regSimFun = (regSimFunProto) l.resolve("registerSimulations");
	if (regSimFun) {
		regSimFun(&pd->sim_registry);
	} else {
		qWarning() << path << " has no node register hook";
	}
	regNodeFunProto regNodeFun = (regNodeFunProto) l.resolve("registerNodes");
	if (regNodeFun) {
		regNodeFun(&pd->node_registry);
	} else {
		qWarning() << path << " has no node register hook";
	}
}

void SaxLoader::loadParameter(const QXmlAttributes& atts) {
	bool simple = atts.value("kind") != "complex";

	std::string name = atts.value("name").toStdString();
	std::cout << "loading paramter " <<  name << std::endl;
	std::string type = "double";
	if (atts.index("type") > 0)
		type = atts.value("type").toStdString();

	cd3assert(current->const_parameters->count(name),
			  str(format("no such parameter in node %1%") % name.c_str()));

	if (simple) {
		QString value = atts.value("value");

		if (type == "double") {
			double d = value.toDouble();
			current->setParameter<double>(name, d);
			return;
		}

		if (type == "float") {
			float d = value.toFloat();
			current->setParameter<float>(name, d);
			return;
		}

		if (type == "int") {
			int ivalue = value.toInt();
			current->setParameter<int>(name, ivalue);
			return;
		}

		if (type == "bool") {
			bool bvalue = value == "true";
			current->setParameter<bool>(name, bvalue);
			return;
		}

		if (type == "string") {
			std::string std = value.toStdString();
			current->setParameter<std::string>(name, std);
			return;
		}

		//xmlError(element, QString().sprintf("%s simple type unknown", type.c_str()));
	} else {
		if (type == "Flow") {
			pd->param_name = atts.value("name").toStdString();
			//pd->f.clear();
		}
		return;
	}
	qWarning() << "unknown type " << atts.value("type");
}
