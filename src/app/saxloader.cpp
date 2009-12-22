#include "saxloader.h"

#include <QLibrary>
#include <QDebug>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <stack>

using namespace boost;
using namespace std;

#include <noderegistry.h>
#include <simulationregistry.h>
#include <typeregistry.h>
#include <imodel.h>
#include <cd3assert.h>
#include "node.h"
#include <simulation.h>
#include <flow.h>
#include <module.h>

struct SaxLoaderPriv {
	NodeRegistry node_registry;
	SimulationRegistry sim_registry;
	TypeRegistry type_registry;
	IModel *model;
	ISimulation *simulation;
	Flow *f;
	std::string param_name;
	std::map<std::string, Flow::CalculationUnit> flow_definition;
	stack<string> parent_nodes;
};

SaxLoader::SaxLoader(IModel *model) {
	cd3assert(model, "model must not be null");
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
	bool consumed = false;

	if (lname == "node") {
		std::string id = atts.value("id").toStdString();
		std::string klass = atts.value("class").toStdString();

		Logger(Debug) << "creating a" << klass << "node with id:" << id;
		current = pd->node_registry.createNode(klass);

		current->setId(id);
		pd->model->addNode(current);
		consumed = true;
	}
	if (lname == "sink") {
		sink_id = atts.value("node").toStdString();
		sink_port = atts.value("port").toStdString();
		consumed = true;
	}
	if (lname == "source") {
		source_id = atts.value("node").toStdString();
		source_port = atts.value("port").toStdString();
		consumed = true;
	}
	if (lname == "parameter") {
		loadParameter(atts);
		consumed = true;
	}
	if (lname == "simulation") {
		cd3assert(pd->simulation == 0, "Simulation already set");
		std::string klass = atts.value("class").toStdString();
		Logger(Debug) << "loading simulation" << klass;
		pd->simulation = pd->sim_registry.createSimulation(klass);
		consumed = true;
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
		consumed = true;
	}
	if (lname == "unit") {
		double value = lexical_cast<double>(atts.value("value").toStdString());
		pd->f->setValue(atts.value("name").toStdString(), value);
		consumed = true;
	}
	if (lname == "pluginpath") {
		std::string path = atts.value("path").toStdString();
		pd->node_registry.addNativePlugin(path);
		pd->sim_registry.addNativePlugin(path);
		consumed = true;
	}
	if (lname == "pythonmodule") {
		std::string module = atts.value("module").toStdString();
		cout << "Loading Python Module " << module << endl;
		PythonEnv::getInstance()->registerNodes(&pd->node_registry, module);
		consumed = true;
	}
	if (lname == "citydrain") {
		consumed = true;
	}
	if (lname == "model") {
		consumed = true;
	}
	if (lname == "nodelist") {
		consumed = true;
	}
	if (lname == "connectionlist") {
		consumed = true;
	}
	if (lname == "flow") {
		if (pd->parent_nodes.top() == "parameter")
			pd->f = new Flow();
		consumed = true;
	}
	if (lname == "connection") {
		cycle_break = atts.value("cycle_break") == "true";
		if (cycle_break) {
			Logger(Standard) << "got cyclebreak";
		}
		consumed = true;
	}
	if (lname == "arrayentry") {
		cd3assert(atts.index("value") >= 0, "arrayentry must provide a value");
		bool ok;
		double value = atts.value("value").toDouble(&ok);
		cd3assert(ok, "value is not a double value");
		current->appendArrayParameter(pd->param_name, value);
		consumed = true;
	}
	if (lname == "concentration") {
		cd3assert(atts.index("name") >= 0, "concentration must define a name");
		std::string name = atts.value("name").toStdString();
		cd3assert(name.length() > 0, "concentration name must not be empty");
		pd->flow_definition[name] = Flow::concentration;
		consumed = true;
	}
	if (lname == "flowdefinition") {
		pd->flow_definition["Q"] = Flow::flow;
		consumed = true;
	}
	pd->parent_nodes.push(lname.toStdString());
	cd3assert(consumed, str(format("not used element: %1%") % lname.toStdString()));
	return consumed;
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
	cd3assert(file.exists(), "no such file or directory");
	bool opened = file.open(QIODevice::ReadOnly);
	cd3assert(opened, "could not open model");
	QXmlSimpleReader r;
	r.setContentHandler(this);
	r.setErrorHandler(this);
	r.setLexicalHandler(this);
	QXmlInputSource source(&file);
	r.parse(&source);
	cd3assert(pd->simulation, "could not load simulation");
	return pd->simulation;
}

bool SaxLoader::endElement(const QString &/*ns*/,
						   const QString &lname,
						   const QString &/*qname*/) {
	pd->parent_nodes.pop();
	bool consumed;
	if (lname == "connection") {
		cd3assert(!source_id.empty(), "source node not set");
		cd3assert(!source_port.empty(), "source port not set");
		cd3assert(!sink_id.empty(), "sink node not set");
		cd3assert(!sink_port.empty(), "sink port not set");
		if (cycle_break) {
			breakCycle();
			cycle_break = false;
		} else {
			shared_ptr<Node> sink = pd->model->getNode(sink_id);
			shared_ptr<Node> source = pd->model->getNode(source_id);
			Logger(Debug) << "creating connection:"
					<< source << "[" << source_port << "] => "
					<< sink << "[" << sink_port << "]";
			pd->model->addConnection(
					pd->simulation->createConnection(source, source_port,
													 sink, sink_port));
		}
		consumed = true;
	}
	if (lname == "nodelist") {
		pd->model->initNodes(pd->simulation->getSimulationParameters());
		consumed = true;
	}
	if (lname == "flow") {
		if (pd->parent_nodes.top() == "parameter") {
			current->setParameter<Flow>(pd->param_name, *pd->f);
			delete pd->f;
		}
		consumed = true;
	}
	if (lname == "flowdefinition") {
		Flow::define(pd->flow_definition);
		consumed = true;
	}
	return true;
}

void SaxLoader::loadParameter(const QXmlAttributes& atts) {
	cd3assert(atts.index("name") >= 0, "no parameter name set");
	std::string name = atts.value("name").toStdString();
	std::string kind = "simple";
	if (atts.index("kind") >= 0)
		kind = atts.value("kind").toStdString();
	std::string type = "double";
	if (atts.index("type") >= 0)
		type = atts.value("type").toStdString();

	cd3assert(current->const_parameters->count(name) ||
			  current->const_array_parameters->count(name),
			  str(format("no such parameter in node %1%") % name.c_str()));

	if (kind == "simple") {
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
		cd3assert(false, str(format("unnknown simple type %1%") % type.c_str()));
		return;
	}
	
	if (kind == "complex" || kind == "array") {
		pd->param_name = atts.value("name").toStdString();
		return;
	}

	cd3assert(false, str(format("unknown kind %1%") % kind.c_str()));
}

void SaxLoader::breakCycle() {
	shared_ptr<Node> sink = pd->model->getNode(sink_id);
	shared_ptr<Node> source = pd->model->getNode(source_id);

	shared_ptr<Node> start = pd->node_registry.createNode("CycleNodeStart");
	start->setId(sink_id+source_id+"-cycle_start");
	shared_ptr<Node> end = pd->node_registry.createNode("CycleNodeEnd");
	end->setId(sink_id+source_id+"-cycle_end");
	pd->model->addNode(start);
	pd->model->addNode(end);
	end->setParameter("start", start);
	pd->model->addConnection(pd->simulation->createConnection(start, "out",
															  sink, sink_port));


	pd->model->addConnection(pd->simulation->createConnection(source, source_port,
															  end, "in"));
}
