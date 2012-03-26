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

#ifndef NEWSIMULATION_H
#define NEWSIMULATION_H

#include <QDialog>
#include <boost/date_time/posix_time/ptime.hpp>

namespace Ui {
	class NewSimulationDialog;
}
class SimulationRegistry;
class ISimulation;
class QDateTime;
class QPushButton;

class NewSimulationDialog : public QDialog
{
	Q_OBJECT
public:
	NewSimulationDialog(SimulationRegistry *sim_reg,
						QWidget *parent = 0,
						Qt::WindowFlags f = 0);

	ISimulation *createSimulation();
	SimulationRegistry *registry;
	Ui::NewSimulationDialog *ui;

private Q_SLOTS:
	void on_projectRootLineEdit_textEdited(QString );
	void on_chooser_clicked();
	void on_start_dateTimeChanged(const QDateTime &date);
private:
	void defineFlow();
	QPushButton *ok;
};

boost::posix_time::ptime qttopt(const QDateTime &dt);

#endif // NEWSIMULATION_H
