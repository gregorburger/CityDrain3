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

#include "stringparameteredit.h"
#include <ui_stringparameteredit.h>

#include <QFileDialog>

StringParameterEdit::StringParameterEdit(QWidget *parent)
	: QWidget(parent) {
	ui = new Ui::StringParameterEdit();
	ui->setupUi(this);
}

void StringParameterEdit::on_fileButton_clicked() {
	QFileInfo info(ui->value->text());
	QString path = QDir::current().path();
	if (info.dir().exists()) {
		path = info.dir().path();
	}
	QString name = QFileDialog::getSaveFileName(this,
												"Choose File or FileName",
												path);
	if (name != "") {
		ui->value->setText(QDir::current().relativeFilePath(name));
	}
}

QString StringParameterEdit::value() const {
	return ui->value->text();
}

void StringParameterEdit::setValue(const QString &value) {
	ui->value->setText(value);
}

