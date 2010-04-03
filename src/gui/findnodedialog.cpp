#include "findnodedialog.h"
#include "ui_findnodedialog.h"
#include "simulationscene.h"
#include <QList>
#include <QDebug>

FindNodeDialog::FindNodeDialog(SimulationScene *scene, QWidget *parent) :
	QDialog(parent), found(0), ui(new Ui::FindNodeDialog), scene(scene) {
	ui->setupUi(this);
	QStringList ids;

	Q_FOREACH(NodeItem *item, scene->getNodeItems()) {
		ids << item->getId();
		this->items[item->getId()] = item;
	}
	ui->id->addItems(ids);
}

FindNodeDialog::~FindNodeDialog() {
	delete ui;
}

void FindNodeDialog::changeEvent(QEvent *e) {
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void FindNodeDialog::accept() {
	if (items.contains(ui->id->currentText())) {
		found = items[ui->id->currentText()];
	}
	QDialog::accept();
}
