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

#ifndef STRINGPARAMETEREDIT_H
#define STRINGPARAMETEREDIT_H

#include <QWidget>

namespace Ui {
	class StringParameterEdit;
}

class StringParameterEdit : public QWidget {
Q_OBJECT
public:
	StringParameterEdit(QWidget *parent = 0);

	QString value() const;
	void setValue(const QString &value);
private Q_SLOTS:
	void on_fileButton_clicked();
private:
	Ui::StringParameterEdit *ui;
};

#endif // STRINGPARAMETEREDIT_H
