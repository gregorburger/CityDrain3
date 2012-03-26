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

#include "renamenodedialog.h"
#include "ui_renamenodedialog.h"
#include <QPushButton>
#include <nodeitem.h>

RenameNodeDialog::RenameNodeDialog(NodeItem *item, QStringList node_names, QWidget *parent) :
	QDialog(parent), item(item), node_names(node_names),
	ui(new Ui::RenameNodeDialog) {
	ui->setupUi(this);
	ui->node_class->setText(item->getClassName());
	ui->node_id->setText(item->getId());
}

RenameNodeDialog::~RenameNodeDialog() {
	delete ui;
}

void RenameNodeDialog::changeEvent(QEvent *e) {
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void RenameNodeDialog::on_node_id_textChanged(const QString &text) {
		QPalette p = ui->node_class->palette();
		p.setColor(p.Base, node_names.contains(text) ? Qt::red : Qt::white);
		ui->node_id->setPalette(p);
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!node_names.contains(text));
}

QString RenameNodeDialog::newId() const {
	return ui->node_id->text();
}
