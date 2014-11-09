/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

#include <typeconverter.h>
#include "saxloader.h"

#include <QLibrary>
#include <QFileInfo>
#include <QDir>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <stack>

using namespace boost;
using namespace std;

#include <noderegistry.h>
#include <simulationregistry.h>
#include <imodel.h>
#include <cd3assert.h>
#include "node.h"
#include <simulation.h>
#include <flow.h>
#include <boost/foreach.hpp>
#ifndef PYTHON_DISABLED
#include <pythoncontroller.h>
#endif

struct SaxLoaderPriv {
	NodeRegistry *node_registry;
	SimulationRegistry *sim_registry;
	Flow *f;
	std::string param_name;
	std::map<std::string, Flow::CalculationUnit> flow_definition;
};

SaxLoader::SaxLoader(IModel *model) : delete_node_reg(true), delete_sim_reg(true) {
	cd3assert(model, "model must not be null");
	pd = new SaxLoaderPriv();
	pd->node_registry = new NodeRegistry();
	pd->sim_registry = new SimulationRegistry();
	this->model = model;
	this->simulation = 0;
}

SaxLoader::SaxLoader(IModel *model,
					 NodeRegistry *nr,
					 SimulationRegistry *sr) : delete_node_reg(false), delete_sim_reg(false) {
	cd3assert(model, "model must not be null");
	pd = new SaxLoaderPriv();
	pd->node_registry = nr;
	pd->sim_registry = sr;
	this->model = model;
	this->simulation = 0;
}

SaxLoader::~SaxLoader() {
	if (delete_sim_reg)
		delete pd->sim_registry;
	if (delete_node_reg)
		delete pd->node_registry;
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
		current = pd->node_registry->createNode(klass);
		model->addNode(id, current);
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
		cd3assert(simulation == 0, "Simulation already set");
		std::string klass = atts.value("class").toStdString();
		Logger(Debug) << "loading simulation" << klass;
		this->simulation = pd->sim_registry->createSimulation(klass);
		consumed = true;
	}
	if (lname == "time") {
		SimulationParameters p;
		Logger(Debug) << "setting simulation time parameters" <<
				"start:" << atts.value("start") <<
				"stop:" << atts.value("stop") <<
				"dt:" << atts.value("dt");
		p.dt = lexical_cast<int>(atts.value("dt").toStdString());
		Logger(Debug) << "parsing posix time " << atts.value("start").toStdString();
		p.start = time_from_string(atts.value("start").toStdString());
		Logger(Debug) << "parsing posix time " << atts.value("stat").toStdString();
		p.stop = time_from_string(atts.value("stop").toStdString());
		simulation->setSimulationParameters(p);
		consumed = true;
	}
	if (lname == "unit") {
		double value = lexical_cast<double>(atts.value("value").toStdString());
		pd->f->setValue(atts.value("name").toStdString(), value);
		consumed = true;
	}
	if (lname == "pluginpath") {
		std::string path = atts.value("path").toStdString();
		pd->node_registry->addNativePlugin(path);
		pd->sim_registry->addNativePlugin(path);
		consumed = true;
	}
	if (lname == "pythonmodule") {
#ifndef PYTHON_DISABLED
		std::string script = atts.value("module").toStdString();
		QFileInfo module_file(atts.value("module"));
		if (!module_file.exists()) {
			Logger(Error) << "can't find specified python module" << script;
			return false;
		}
		NodeRegistry::addToPythonPath(module_file.dir().path().toStdString());
		pd->node_registry->addPythonPlugin(script);
		consumed = true;
#else
		Logger(Error) << "python support is disabled";
		consumed = true;
#endif
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
	if (lname == "connection") {
		cycle_break = atts.value("cycle_break") == "true";
		if (cycle_break) {
			Logger(Standard) << "got cyclebreak";
		}
		consumed = true;
	}
	if (lname == "concentration") {
		cd3assert(atts.index("name") >= 0, "concentration must define a name");
		std::string name = atts.value("name").toStdString();
		cd3assert(name.length() > 0, "concentration name must not be empty");
		pd->flow_definition[name] = Flow::concentration;
		consumed = true;
	}
	if (lname == "climatic") {
		cd3assert(atts.index("name") >= 0, "climatic must define a name");
		std::string name = atts.value("name").toStdString();
		cd3assert(name.length() > 0, "climatic name must not be empty");
		pd->flow_definition[name] = Flow::climatic;
		consumed = true;
	}
	if (lname == "temperature") {
		cd3assert(atts.index("name") >= 0, "temperature must define a name");
		std::string name = atts.value("name").toStdString();
		cd3assert(name.length() > 0, "temperature name must not be empty");
		pd->flow_definition[name] = Flow::temperature;
		consumed = true;
	}
	if (lname == "flowdefinition") {
		pd->flow_definition["Q"] = Flow::flow;
		consumed = true;
	}
	if (lname == "flow") {
		consumed = true;
	}
	if (lname == "gui" || lname == "nodeposition") { //used for gui
		consumed = true;
	}
	if (lname == "controller") {
#ifndef PYTHON_DISABLED
		simulation->addController(new PythonController(atts.value("path").toStdString()));
		consumed = true;
#else
		Logger(Error) << "python support is disabled";
		consumed = true;
#endif
	}
	if (!consumed) {
		Logger(Warning) << "not used xml element" << lname.toStdString();
	}
	cd3assert(consumed, str(format("not used element: %1%") % lname.toStdString()));
	return consumed;
}

bool SaxLoader::error(const QXmlParseException &exception) {
	int line = exception.lineNumber();
	int col = exception.columnNumber();
	Logger(Error) << "XmlException: " << line << ":" << col << exception.message();
	return false;
}

bool SaxLoader::fatalError(const QXmlParseException &exception) {
	int line = exception.lineNumber();
	int col = exception.columnNumber();
	Logger(Error) << "XmlException: " << line << ":" << col << exception.message();
	return false;
}

bool SaxLoader::warning(const QXmlParseException &exception) {
	Logger(Warning) << exception.message();
	return true;
}

bool SaxLoader::load(QFile &file) {
	cd3assert(file.exists(), "no such file or directory");
	bool opened = file.open(QIODevice::ReadOnly);
	cd3assert(opened, "could not open model");
	QXmlSimpleReader r;
	r.setContentHandler(this);
	r.setErrorHandler(this);
	r.setLexicalHandler(this);
	QXmlInputSource source(&file);
	bool cwd = QDir::setCurrent(QFileInfo(file).absoluteDir().path());
#ifndef PYTHON_DISABLED
	NodeRegistry::addToPythonPath(QFileInfo(file).absoluteDir().path().toStdString());
#endif
	return r.parse(&source);
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
		} else {
			Node *sink = model->getNode(sink_id);
			Node *source = model->getNode(source_id);
			Logger(Debug) << "creating connection:"
					<< source << "[" << source_port << "] => "
					<< sink << "[" << sink_port << "]";
			model->addConnection(
					simulation->createConnection(source, source_port,
													 sink, sink_port));
		}
		return true;
	}
	if (lname == "nodelist") {
		node_set_type not_inited = model->initNodes(simulation->getSimulationParameters());
		if (!not_inited.empty()) {
			Logger(Error) << "some nodes failed to init";
			BOOST_FOREACH(Node *n, not_inited) {
				Logger(Error) << "node" << n->getId() << "of type" << n->getClassName() << "failed to init";
			}
			return false;
		}
		return true;
	}
	if (lname == "flowdefinition") {
		Flow::undefine();
		Flow::define(pd->flow_definition);
		return true;
	}
	return true;
}

void SaxLoader::loadParameter(const QXmlAttributes& atts) {
	cd3assert(atts.index("name") >= 0, "no parameter name set");
	std::string name = atts.value("name").toStdString();
	std::string type = "double";
	if (atts.index("type") >= 0)
		type = atts.value("type").toStdString();

	std::string value = atts.value("value").toStdString();
	TypeConverter *conv = TypeConverter::get(type);
	cd3assert(conv, "no type converter found for type");
	conv->setParameter(current, name, value);
}

void SaxLoader::breakCycle() {
	Node *sink = model->getNode(sink_id);
	Node *source = model->getNode(source_id);

	Node *start = pd->node_registry->createNode("CycleNodeStart");
	Node *end = pd->node_registry->createNode("CycleNodeEnd");
	model->addNode(sink_id+source_id+"-cycle_start", start);
	model->addNode(sink_id+source_id+"-cycle_end", end);
	end->setParameter("start", start);
	model->addConnection(simulation->createConnection(start, "out",
															  sink, sink_port));


	model->addConnection(simulation->createConnection(source, source_port,
															  end, "in"));
}
