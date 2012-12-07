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

#include "saveasdialog.h"
#include "ui_saveasdialog.h"
#include <QFileInfo>
#include <QPushButton>
#include <QDir>

SaveAsDialog::SaveAsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SaveAsDialog) {
	ui->setupUi(this);
	ui->projectRoot->setText(QDir::currentPath());
	ok = ui->buttonBox->button(QDialogButtonBox::Ok);
}

SaveAsDialog::~SaveAsDialog() {
	delete ui;
}

void SaveAsDialog::on_projectName_textChanged(QString s) {
	QFileInfo info(s);
	ok->setEnabled(!(info.exists() && info.isFile()));
}

QString SaveAsDialog::getSaveFileName() {
	SaveAsDialog d;
	if (d.exec() == QDialog::Accepted) {
		return QDir::cleanPath(QDir::currentPath()
							   + QDir::separator()
							   + d.ui->projectName->text());
	}
	return "";
}
