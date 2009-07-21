#include "saxloader.h"

#include <QLibrary>
#include <QDebug>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <memory>

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
#include <controller.h>
#include <nodeconnection.h>
#include <../nodes/cyclenodeend.h>
#include <../nodes/cyclenodestart.h>

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
		std::string script = atts.value("script").toStdString();
		if (script.empty()) {
			Logger(Debug) << "creating a" << klass << "node with id:" << id;
			current = pd->node_registry.createNode(klass);
		} else {
			Logger(Debug) << "creating a scripted" << klass
					<< "node with id:" << id
					<< "and script:" << script;
			current = pd->node_registry.createNode(klass, script);
		}
		current->setId(id);
		pd->model->addNode(current);
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
		Logger(Debug) << "loading simulation" << klass;
		pd->simulation = pd->sim_registry.createSimulation(klass);
		return true;
	}
	if (lname == "controller") {
		cd3assert(pd->simulation != 0, "no simulation set");
		QString script = atts.value("script");
		cd3assert(QFile::exists(script), "no such controller script file");

		Logger(Debug) << "creating JavaScript Controller with script" << script;
		shared_ptr<Controller> c(new Controller(script.toStdString()));
		pd->simulation->timestep_before
				.connect(bind<void>(&Controller::controllBefore, c, _1, _2));
		pd->simulation->timestep_after
				.connect(bind<void>(&Controller::controllAfter, c, _1, _2));
		return true;
	}
	if (lname == "time") {
		SimulationParameters p;
		p.dt = lexical_cast<int>(atts.value("dt").toStdString());
		p.start = lexical_cast<int>(atts.value("start").toStdString());
		p.stop = lexical_cast<int>(atts.value("stop").toStdString());
		Logger(Debug) << "setting simulation time parameters" <<
				"start:" << atts.value("start") <<
				"stop:" << atts.value("stop") <<
				"dt:" << atts.value("dt");
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
		pd->f.addUnit(atts.value("name").toStdString(),
					  cunit,
					  value);
		return true;
	}
	if (lname == "pluginpath") {
		std::string path = atts.value("path").toStdString();
		pd->node_registry.addPlugin(path);
		pd->sim_registry.addPlugin(path);
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
		cycle_break = atts.value("cycle_break") == "true";
		if (cycle_break) {
			Logger(Standard) << "got cyclebreak";
		}
		return true;
	}
	Logger(Debug) << "not used element:" << lname.toStdString();
	return true;
}

bool SaxLoader::error(const QXmlParseException &exception) {
	Logger(Error) << exception.message();
	return true;
}

bool SaxLoader::fatalError(const QXmlParseException &exception) {
	Logger(Error) << exception.message();
	return true;
}

bool SaxLoader::warning(const QXmlParseException &exception) {
	Logger(Warning) << exception.message();
	return true;
}

ISimulation *SaxLoader::load(QFile &file) {
	bool opened = file.open(QIODevice::ReadOnly);
	cd3assert(opened, "could not open model");
	QXmlSimpleReader r;
	r.setContentHandler(this);
	r.setErrorHandler(this);
	r.setLexicalHandler(this);
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
		if (cycle_break) {
			breakCycle();
			cycle_break = false;
			return true;
		}
		Node *sink = pd->model->getNode(sink_id);
		Node *source = pd->model->getNode(source_id);
		Logger(Debug) << "creating connection:"
				<< source << "[" << source_port << "] => "
				<< sink << "[" << sink_port << "]";
		pd->model->addConnection(
				pd->simulation->createConnection(source, source_port,
												 sink, sink_port));
	}
	if (lname == "nodelist") {
		pd->model->initNodes(pd->simulation->getSimulationParameters());
	}
	if (lname == "flow") {
		current->setParameter<Flow>(pd->param_name, pd->f);
	}
	return true;
}

void SaxLoader::loadParameter(const QXmlAttributes& atts) {
	bool simple = atts.value("kind") != "complex";

	std::string name = atts.value("name").toStdString();
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

	} else {
		if (type == "Flow") {
			pd->param_name = atts.value("name").toStdString();
			//pd->f.clear();
		}
		return;
	}
	qWarning() << "unknown type " << atts.value("type");
}

void SaxLoader::breakCycle() {
	Node *sink = pd->model->getNode(sink_id);
	Node *source = pd->model->getNode(source_id);

	CycleNodeStart *start = (CycleNodeStart*) pd->node_registry.createNode("CycleNodeStart");
	start->setId(sink_id+source_id+"-cycle_start");
	CycleNodeEnd *end = (CycleNodeEnd*) pd->node_registry.createNode("CycleNodeEnd");
	end->setId(sink_id+source_id+"-cycle_end");
	pd->model->addNode(start);
	pd->model->addNode(end);
	end->start = start;
	pd->model->addConnection(pd->simulation->createConnection(start, "out",
															  sink, sink_port));


	pd->model->addConnection(pd->simulation->createConnection(source, source_port,
															  end, "in"));
}
