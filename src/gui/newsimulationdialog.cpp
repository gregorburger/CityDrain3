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

	registry->addNativePlugin("nodes");

	BOOST_FOREACH(std::string name, registry->getRegisteredNames()) {
		list << QString::fromStdString(name);
	}
	ui->simulationComboBox->addItems(list);
}

ptime qttopt(const QDateTime &dt) {
	date d(dt.date().year(), dt.date().month(), dt.date().day());
	ptime t(d, time_duration(dt.time().hour(), dt.time().minute(), dt.time().second()));
	return t;
}

void NewSimulationDialog::defineFlow() {
	using namespace std;
	map<string, Flow::CalculationUnit> definition;
	definition[ui->flowName->text().toStdString()] = Flow::flow;
	Q_FOREACH(QString c, ui->concentrationNames->text().split(' ', QString::SkipEmptyParts)) {
		definition[c.trimmed().toStdString()] = Flow::concentration;
	}
	Flow::undefine();
	Flow::define(definition);
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
		scene->addPlugin("nodes");
	}
	defineFlow();
	return scene;
}

void NewSimulationDialog::on_start_dateTimeChanged(const QDateTime &date) {
	ui->stop->setMinimumDateTime(date.addSecs(ui->dt->value()));
}
