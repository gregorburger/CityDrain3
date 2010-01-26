#include "newsimulationdialog.h"
#include <ui_newsimulationdialog.h>
#include <simulationregistry.h>
#include <noderegistry.h>
#include <simulation.h>
#include <simulationscene.h>
#include <flow.h>

#include <QFile>
#include <QString>
#include <QDateTime>
#include <boost/foreach.hpp>
#include <boost/date_time.hpp>

using namespace boost::posix_time;
using namespace boost::gregorian;

NewSimulationDialog::NewSimulationDialog(QWidget *parent,
										 Qt::WindowFlags f)
	: QDialog(parent, f), ui(new Ui::NewSimulationDialog()) {
	registry = new SimulationRegistry();
	ui->setupUi(this);
	QStringList list;


	default_node_paths << "./libnodes.so" << "../../libnodes.so" << "./build/libnodes.so";
	default_node_paths << "./nodes.dll"  << "../../nodes.dll" << "./build/nodes.dll";
	Q_FOREACH(QString path, default_node_paths) {
		if (QFile::exists(path)) {
			registry->addNativePlugin(path.toStdString());
			break;
		}
	}

	BOOST_FOREACH(std::string name, registry->getRegisteredNames()) {
		list << QString::fromStdString(name);
	}
	ui->simulationComboBox->addItems(list);
}

ptime qttopt(QDateTime dt) {
	date d(dt.date().year(), dt.date().month(), dt.date().day());
	ptime t(d, time_duration(dt.time().hour(), dt.time().minute(), dt.time().second()));
	return t;
}

SimulationScene *NewSimulationDialog::createSimulationScene() {
	SimulationParameters p(qttopt(ui->start->dateTime()),
						   qttopt(ui->stop->dateTime()),
						   ui->dt->value());
	ISimulation *sim = registry->createSimulation(ui->simulationComboBox->currentText().toStdString());
	sim->setSimulationParameters(p);
	SimulationScene *scene = new SimulationScene();
	scene->setSimulation(sim);
	if (ui->defaultNodesCheckBox->isChecked()) {
		Q_FOREACH(QString path, default_node_paths) {
			if (QFile::exists(path)) {
				scene->getNodeRegistry()->addNativePlugin(path.toStdString());
				break;
			}
		}
	}
	//workaround until a gui for flowdefinition is settled
	std::map<std::string, Flow::CalculationUnit> fd;
	fd["Q"] = Flow::flow;
	Flow::define(fd);
	return scene;
}
