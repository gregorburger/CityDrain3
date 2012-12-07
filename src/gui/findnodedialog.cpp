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

#include "findnodedialog.h"
#include "ui_findnodedialog.h"
#include "simulationscene.h"
#include <QList>
#include <QLineEdit>

FindNodeDialog::FindNodeDialog(SimulationScene *scene, QWidget *parent) :
	QDialog(parent), found(0), ui(new Ui::FindNodeDialog), scene(scene) {
	ui->setupUi(this);
	QStringList ids;

	Q_FOREACH(NodeItem *item, scene->getNodeItems()) {
		ids << item->getId();
		this->items[item->getId()] = item;
	}
	ui->id->addItems(ids);
	QLineEdit *e = ui->id->lineEdit();
	e->setSelection(0, e->text().length());
}

FindNodeDialog::~FindNodeDialog() {
	delete ui;
}

void FindNodeDialog::changeEvent(QEvent *e) {
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void FindNodeDialog::accept() {
	if (items.contains(ui->id->currentText())) {
		found = items[ui->id->currentText()];
	}
	QDialog::accept();
}
