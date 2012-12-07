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

#ifndef RENAMENODEDIALOG_H
#define RENAMENODEDIALOG_H

#include <QDialog>

class NodeItem;

namespace Ui {
	class RenameNodeDialog;
}

class RenameNodeDialog : public QDialog {
	Q_OBJECT

public:
	explicit RenameNodeDialog(NodeItem *item, QStringList node_names, QWidget *parent = 0);
	~RenameNodeDialog();

	QString newId() const;
private Q_SLOTS:
	void on_node_id_textChanged(const QString &text);

protected:
	void changeEvent(QEvent *e);

private:
	NodeItem *item;
	QStringList node_names;
	Ui::RenameNodeDialog *ui;
};

#endif // RENAMENODEDIALOG_H
