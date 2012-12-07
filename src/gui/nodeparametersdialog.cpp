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

#include "nodeparametersdialog.h"
#include <ui_nodeparametersdialog.h>
#include <stringparameteredit.h>
#include <node.h>
#include <flow.h>

#include <boost/foreach.hpp>
#include <limits>

#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDirModel>
#include <QCompleter>
#include <QDebug>

typedef std::pair<std::string, NodeParameter*> ptype;

NodeParametersDialog::~NodeParametersDialog() {
}

NodeParametersDialog::NodeParametersDialog(Node *node, QWidget *parent)
	: QDialog(parent), node(node) {
	ui = new Ui::NodeParametersDialog();
	ui->setupUi(this);

	int row = 0;

	BOOST_FOREACH(ptype p, node->getParameters()) {
		NodeParameter *param = p.second;
		QString name = QString::fromStdString(param->name);
		QString unit = QString::fromStdString(param->unit);
		QLabel *label = new QLabel(this);
		label->setText(QString("%1 (%2) :").arg(name).arg(unit));
		label->setToolTip(QString::fromStdString(param->description));
		QWidget *param_widget = widgetForParameter(param);
		ui->gridLayout->addWidget(label, row, 0, Qt::AlignRight);
		ui->gridLayout->addWidget(param_widget, row, 1);
		row ++;
		widgets[param->name] = param_widget;
	}

	this->adjustSize();
}

QWidget *NodeParametersDialog::widgetForParameter(NodeParameter *p) {
	if (p->type == cd3::TypeInfo(typeid(std::vector<double>))) {
		QLineEdit *param_widget = new QLineEdit(this);

		std::vector<double> values = *((std::vector<double> *) p->value);

		QString s;
		for (size_t i = 0; i < values.size(); i++) {
			s += i == 0 ? QString("%1").arg(values[i]) : QString(", %1").arg(values[i]);
		}
		param_widget->setText(s);
		return param_widget;
	}

	if (p->type == cd3::TypeInfo(typeid(int))) {
		QSpinBox *widget = new QSpinBox();
		widget->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
		int *value = (int *) p->value;
		widget->setValue(*value);
		return widget;
	}

	if (p->type == cd3::TypeInfo(typeid(double))) {
		QDoubleSpinBox *widget = new QDoubleSpinBox(this);
		widget->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
		widget->setDecimals(10);
		double *value = (double *) p->value;
		widget->setValue(*value);
		return widget;
	}

	if (p->type == cd3::TypeInfo(typeid(std::string))) {
		StringParameterEdit *widget = new StringParameterEdit(this);
		std::string *value = (std::string *) p->value;
		widget->setValue(QString::fromStdString(*value));
		return widget;
	}
	if (p->type == cd3::TypeInfo(typeid(Flow))) {
		QLineEdit *widget = new QLineEdit(this);
		std::vector<std::string> names = Flow::getNames();
		QString text = QString::fromStdString(names[0]);
		for (size_t i = 1; i < names.size(); i++)  {
			text += ", " + QString::fromStdString(names[i]);
		}
		widget->setStatusTip(text);

		Flow *f = (Flow*) p->value;
		text = QString("%1").arg((*f)[0]);
		for (size_t i = 1; i < Flow::size(); i++) {
			text += QString(", %1").arg((*f)[i]);
		}
		widget->setText(text);
		return widget;
	}
	Logger() << "UNKNOWN  type of parameter " << p->name;
	return new QLineEdit(this);
}

bool NodeParametersDialog::updateNodeParameters() {
	std::map<std::string, NodeParameter *> params = node->getParameters();
	Q_FOREACH(std::string p, widgets.keys()) {
		NodeParameter *param = params[p];
		if (param->type == cd3::TypeInfo(typeid(int))) {
			QSpinBox *widget = (QSpinBox *) widgets[p];
			node->setParameter(p, widget->value());
			continue;
		}
		if (param->type == cd3::TypeInfo(typeid(double))) {
			QDoubleSpinBox *widget = (QDoubleSpinBox *) widgets[p];
			node->setParameter(p, widget->value());
			continue;
		}
		if (param->type == cd3::TypeInfo(typeid(std::string))) {
			StringParameterEdit *widget = (StringParameterEdit *) widgets[p];
			node->setParameter(p, widget->value().toStdString());
			continue;
		}
		if (param->type == cd3::TypeInfo(typeid(Flow))) {
			std::vector<std::string> names = Flow::getNames();
			QLineEdit *widget = static_cast<QLineEdit *>(widgets[p]);
			QStringList values = widget->text().split(",");
			if (values.size() != names.size()) {
				qDebug() << "input format wrong";
				return false;
			}
			int valuei = 0;
			Flow f;
			Q_FOREACH(QString qsvalue, values) {
				bool ok;
				double value = qsvalue.trimmed().toDouble(&ok);
				if (!ok) {
					qDebug() << "value for " << QString::fromStdString(names[valuei]) << " not a double";
					return false;
				}
				f.setValue(names[valuei], value);
				++valuei;
			}
			node->setParameter(p, f);
			continue;
		}
		if (param->type == cd3::TypeInfo(typeid(std::vector<double>))) {
			std::vector<double> v;
			QLineEdit *line = static_cast<QLineEdit *>(widgets[p]);
			QStringList values = line->text().split(",", QString::SkipEmptyParts);
			Q_FOREACH(QString value, values) {
				bool ok;
				v.push_back(value.toDouble(&ok));
			}
			node->setParameter(p, v);
			continue;
		}
		Logger() << "cannot update node parameter " << p;
	}
	return true;
}
