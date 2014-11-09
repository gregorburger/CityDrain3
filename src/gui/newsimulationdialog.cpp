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

#include <simulation.h>
#include "newsimulationdialog.h"
#include <ui_newsimulationdialog.h>
#include <simulationregistry.h>
#include <noderegistry.h>
#include <simulationscene.h>
#include <flow.h>

#include <QFile>
#include <QString>
#include <QFileDialog>
#include <QPushButton>
#include <boost/foreach.hpp>

NewSimulationDialog::NewSimulationDialog(SimulationRegistry *registry,
										 QWidget *parent,
										 Qt::WindowFlags f)
	: QDialog(parent, f), registry(registry), ui(new Ui::NewSimulationDialog()) {
	ui->setupUi(this);
	ok = ui->buttonBox->button(QDialogButtonBox::Ok);
	ok->setEnabled(false);
	QStringList list;

	BOOST_FOREACH(std::string name, registry->getRegisteredNames()) {
		list << QString::fromStdString(name);
	}
	ui->simulationComboBox->addItems(list);
	ui->projectRootLineEdit->setText(QDir::currentPath());
	on_projectRootLineEdit_textEdited(QDir::currentPath());
}



void NewSimulationDialog::defineFlow() {
	std::map<std::string, Flow::CalculationUnit> definition;
	definition[ui->flowName->text().toStdString()] = Flow::flow;
	Q_FOREACH(QString c, ui->concentrationNames->text().split(' ', QString::SkipEmptyParts)) {
		definition[c.trimmed().toStdString()] = Flow::concentration;
	}
	Q_FOREACH(QString c, ui->climaticNames->text().split(' ', QString::SkipEmptyParts)) {
		definition[c.trimmed().toStdString()] = Flow::climatic;
	}
	Q_FOREACH(QString c, ui->temperatureNames->text().split(' ', QString::SkipEmptyParts)) {
		definition[c.trimmed().toStdString()] = Flow::temperature;
	}
	Flow::undefine();
	Flow::define(definition);
}

ISimulation *NewSimulationDialog::createSimulation() {
	SimulationParameters p(qttopt(ui->start->dateTime()),
						   qttopt(ui->stop->dateTime()),
						   ui->dt->value());
	std::string sim_class = ui->simulationComboBox->currentText().toStdString();
	ISimulation *sim = registry->createSimulation(sim_class);
	sim->setSimulationParameters(p);

	defineFlow();
	return sim;
}

void NewSimulationDialog::on_start_dateTimeChanged(const QDateTime &date) {
	ui->stop->setMinimumDateTime(date.addSecs(ui->dt->value()));
}

void NewSimulationDialog::on_chooser_clicked() {
	QString dir = QFileDialog::getExistingDirectory(this, "choose working directory");
	if (dir == "") {
		return;
	}
	ui->projectRootLineEdit->setText(dir);;
	ok->setEnabled(true);
}

void NewSimulationDialog::on_projectRootLineEdit_textEdited(QString s) {
	QDir d;
	ok->setEnabled(d.exists(s));
}
