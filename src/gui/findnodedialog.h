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

#ifndef FINDNODEDIALOG_H
#define FINDNODEDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
	class FindNodeDialog;
}

class SimulationScene;
class NodeItem;

class FindNodeDialog : public QDialog {
	Q_OBJECT
public:
	explicit FindNodeDialog(SimulationScene *scene, QWidget *parent = 0);
	virtual ~FindNodeDialog();
	NodeItem *found;

protected:
	void changeEvent(QEvent *e);

private:
	void accept();
	Ui::FindNodeDialog *ui;
	SimulationScene *scene;
	QMap<QString, NodeItem*> items;
};

#endif // FINDNODEDIALOG_H
