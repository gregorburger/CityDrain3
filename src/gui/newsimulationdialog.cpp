#include "newsimulationdialog.h"
#include <ui_newsimulationdialog.h>
#include <simulationregistry.h>
#include <simulation.h>
#include <QString>
#include <QDateTime>
#include <boost/foreach.hpp>
#include <boost/date_time.hpp>

using namespace boost::posix_time;
using namespace boost::gregorian;

NewSimulationDialog::NewSimulationDialog(SimulationRegistry *registry,
										 QWidget *parent,
										 Qt::WindowFlags f)
	: QDialog(parent, f), ui(new Ui::NewSimulationDialog()), registry(registry) {
	ui->setupUi(this);
	QStringList list;

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

ISimulation *NewSimulationDialog::createSimulation() {
	SimulationParameters p(qttopt(ui->start->dateTime()),
						   qttopt(ui->stop->dateTime()),
						   ui->dt->value());
	ISimulation *sim = registry->createSimulation(ui->simulationComboBox->currentText().toStdString());
	sim->setSimulationParameters(p);
	return sim;
}
