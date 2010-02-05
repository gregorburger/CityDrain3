#include "nodeparametersdialog.h"
#include <ui_nodeparametersdialog.h>
#include <node.h>

#include <boost/foreach.hpp>

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
		label->setText(QString("%1 [%2] :").arg(name).arg(unit));
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
		widget->setRange(INT_MIN, INT_MAX);
		int *value = (int *) p->value;
		widget->setValue(*value);
		return widget;
	}

	if (p->type == cd3::TypeInfo(typeid(double))) {
		QDoubleSpinBox *widget = new QDoubleSpinBox(this);
		widget->setRange(-INFINITY, INFINITY);
		double *value = (double *) p->value;
		widget->setValue(*value);
		return widget;
	}

	if (p->type == cd3::TypeInfo(typeid(string))) {
		QLineEdit *widget = new QLineEdit(this);
		string *value = (string *) p->value;
		QDirModel *model = new QDirModel(this);
		QCompleter *completer = new QCompleter(model, this);
		widget->setText(QString::fromStdString(*value));
		widget->setCompleter(completer);
		return widget;
	}
	qDebug() << "UNKNOWN  type of parameter";
	return new QLineEdit(this);
}

void NodeParametersDialog::updateNodeParameters() {
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
			QLineEdit *widget = (QLineEdit *) widgets[p];
			node->setParameter(p, widget->text().toStdString());
			continue;
		}
		qWarning() << "cannot update node parameter " << QString::fromStdString(p);
	}
}
