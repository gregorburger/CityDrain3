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

typedef pair<string, NodeParameter*> ptype;

NodeParametersDialog::~NodeParametersDialog() {
	delete layout;
}

NodeParametersDialog::NodeParametersDialog(Node *node, QWidget *parent)
	: QDialog(parent), node(node) {
	ui = new Ui::NodeParametersDialog();
	ui->setupUi(this);

	layout = new QGridLayout();

	int row = 0;

	BOOST_FOREACH(ptype p, node->getParameters()) {
		NodeParameter *param = p.second;
		QString name = QString::fromStdString(param->name);
		QString unit = QString::fromStdString(param->unit);
		QLabel *label = new QLabel(this);
		label->setText(QString("%1 (%2) :").arg(name).arg(unit));
		label->setToolTip(QString::fromStdString(param->description));
		QWidget *param_widget = widgetForParameter(param);
		layout->addWidget(label, row, 0, Qt::AlignRight);
		layout->addWidget(param_widget, row, 1);
		row ++;
		widgets[param->name] = param_widget;
		//parameters << param->name;
	}

	ui->verticalLayout->insertLayout(0, layout);
	this->adjustSize();
}

QWidget *NodeParametersDialog::widgetForParameter(NodeParameter *p) {
	if (p->type == cd3::TypeInfo(typeid(int))) {
		QSpinBox *widget = new QSpinBox();
		widget->setRange(numeric_limits<int>::min(), numeric_limits<int>::max());
		int *value = (int *) p->value;
		widget->setValue(*value);
		return widget;
	}

	if (p->type == cd3::TypeInfo(typeid(double))) {
		QDoubleSpinBox *widget = new QDoubleSpinBox(this);
		widget->setRange(-numeric_limits<double>::max(), numeric_limits<double>::max());
		double *value = (double *) p->value;
		widget->setValue(*value);
		return widget;
	}

	if (p->type == cd3::TypeInfo(typeid(string))) {
		StringParameterEdit *widget = new StringParameterEdit(this);
		string *value = (string *) p->value;
		widget->setValue(QString::fromStdString(*value));
		return widget;
	}
	if (p->type == cd3::TypeInfo(typeid(Flow))) {
		QLineEdit *widget = new QLineEdit(this);
		vector<string> names = Flow::getNames();
		QString text = QString::fromStdString(names[0]);
		for (size_t i = 1; i < names.size(); i++)  {
			text += ", " + QString::fromStdString(names[i]);
		}
		widget->setText(text);
		return widget;
	}
	qDebug() << "UNKNOWN  type of parameter " << QString::fromStdString(p->name);
	return new QLineEdit(this);
}

bool NodeParametersDialog::updateNodeParameters() {
	map<string, NodeParameter *> params = node->getParameters();
	Q_FOREACH(string p, widgets.keys()) {
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
		if (param->type == cd3::TypeInfo(typeid(string))) {
			StringParameterEdit *widget = (StringParameterEdit *) widgets[p];
			node->setParameter(p, widget->value().toStdString());
			continue;
		}
		if (param->type == cd3::TypeInfo(typeid(Flow))) {
			vector<string> names = Flow::getNames();
			QLineEdit *widget = (QLineEdit *) widgets[p];
			QStringList values = widget->text().split(",");
			if (values.length() != names.size()) {
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
		qWarning() << "cannot update node parameter " << QString::fromStdString(p);
	}
	return true;
}
