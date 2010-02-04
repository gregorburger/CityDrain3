#include "mainwindow.h"
#include <simulationregistry.h>
#include <simulation.h>
#include <noderegistry.h>
#include <mapbasedmodel.h>

#include "ui_mainwindow.h"
#include "ui_newsimulationdialog.h"
#include "simulationscene.h"
#include "newsimulationdialog.h"
#include "simulationthread.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDateTimeEdit>
#include <QLabel>
#include <QSpinBox>
#include <boost/foreach.hpp>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow), scene(0), model_unsaved(false) {
	ui->setupUi(this);
	ui->graphicsView->setRenderHints(QPainter::Antialiasing);

	QLabel *startLabel = new QLabel("start:", ui->mainToolBar);
	ui->mainToolBar->addWidget(startLabel);

	start = new QDateTimeEdit(ui->mainToolBar);
	start->setDisplayFormat("d.M.yy h:mm:ss");
	start->setCalendarPopup(true);
	ui->mainToolBar->addWidget(start);
	this->connect(start, SIGNAL(dateTimeChanged(QDateTime)), SLOT(on_start_stop_dateTimeChanged(QDateTime)));

	QLabel *stopLabel = new QLabel("stop:", ui->mainToolBar);
	ui->mainToolBar->addWidget(stopLabel);


	stop = new QDateTimeEdit(ui->mainToolBar);
	stop->setDisplayFormat("d.M.yy h:mm:ss");
	stop->setCalendarPopup(true);
	ui->mainToolBar->addWidget(stop);
	this->connect(stop, SIGNAL(dateTimeChanged(QDateTime)), SLOT(on_start_stop_dateTimeChanged(QDateTime)));

	QLabel *dtLabel = new QLabel("dt:", ui->mainToolBar);
	ui->mainToolBar->addWidget(dtLabel);

	dt = new QSpinBox(ui->mainToolBar);
	dt->setRange(0, INT_MAX-1);
	dt->setSingleStep(60);
	ui->mainToolBar->addWidget(dt);
	this->connect(dt, SIGNAL(valueChanged(int)), SLOT(on_dt_valueChanged(int)));
	//QMetaObject::connectSlotsByName(this);
	sceneChanged();
}

MainWindow::~MainWindow() {
	if (scene)
		delete scene;
	delete ui;
}

void MainWindow::changeEvent(QEvent *e) {
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void MainWindow::on_actionAdd_Plugin_activated() {
	QString plugin = QFileDialog::getOpenFileName(this, "select plugin", ".", "*.so, *.dll");
	if (plugin == "")
		return;
	scene->addPlugin(plugin);
	pluginsAdded();
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_Plus) {
		zoomIn();
		return;
	}
	if (e->key() == Qt::Key_Minus) {
		zoomOut();
		return;
	}
	QMainWindow::keyPressEvent(e);
}

void MainWindow::wheelEvent(QWheelEvent *event) {
	if (event->modifiers() & Qt::ControlModifier) {
		int times = event->delta() / 120;
		if (times < 0)
			zoomOut(qAbs(times));
		else
			zoomIn(times);
		return;
	}
	QMainWindow::wheelEvent(event);
}

void MainWindow::zoomIn(int times) {
	ui->graphicsView->scale(times * 1.2, times * 1.2);
}

void MainWindow::zoomOut(int times) {
	ui->graphicsView->scale(1/(times * 1.2), 1/(times * 1.2));
}

void MainWindow::pluginsAdded() {
	ui->treeWidget->clear();
	QTreeWidgetItem *nodes = new QTreeWidgetItem(QStringList("nodes"));

	BOOST_FOREACH(std::string node_name, scene->getNodeRegistry()->getRegisteredNames()) {
		QString qnode_name = QString::fromStdString(node_name);
		if (qnode_name.startsWith("CycleNode"))
			continue;
		QTreeWidgetItem *item = new QTreeWidgetItem(nodes);
		item->setText(0, qnode_name);
	}
	ui->treeWidget->insertTopLevelItem(0, nodes);

	nodes->setExpanded(true);
}

void MainWindow::on_runButton_clicked() {
	current_thread = new SimulationThread(scene->getSimulation());
	QObject::connect(current_thread->handler, SIGNAL(progress(int)),
					 ui->simProgressBar, SLOT(setValue(int)), Qt::QueuedConnection);
	QObject::connect(current_thread, SIGNAL(finished()),
					 this, SLOT(simulationFinished()), Qt::QueuedConnection);
	this->setEnabled(false);
	current_thread->start();
}

void MainWindow::simulationFinished() {
	delete current_thread;
	this->setEnabled(true);
}

void MainWindow::on_actionNewSimulation_activated() {
	NewSimulationDialog ns(this);
	if (ns.exec()) {
		scene = ns.createSimulationScene();
		sceneChanged();
		start->setDateTime(ns.ui->start->dateTime());
		stop->setDateTime(ns.ui->stop->dateTime());
		dt->setValue(ns.ui->dt->value());
		ui->graphicsView->setScene(scene);
		ui->actionSave_Simulation->setEnabled(true);
		ui->runButton->setEnabled(true);
		pluginsAdded();
		ui->actionAdd_Plugin->setEnabled(true);
		ui->actionAdd_Python_Module->setEnabled(true);
	}
}

void MainWindow::sceneChanged() {
	if (!scene) {
		start->setEnabled(false);
		stop->setEnabled(false);
		dt->setEnabled(false);
		return;
	}

	start->setEnabled(true);
	stop->setEnabled(true);
	dt->setEnabled(true);
}

void MainWindow::on_actionSave_Simulation_activated() {
	if (scene->getModelFileName() == "") {
		QString fileName = QFileDialog::getSaveFileName(this, "Save Model File", ".", "XML Files (*.xml)");
		if (fileName == "")
			return;
		scene->setModelFileName(fileName);
	}
	scene->save();
	model_unsaved = false;
}

void MainWindow::on_actionAdd_Python_Module_activated() {
	QString plugin = QFileDialog::getOpenFileName(this,
												  "select python module",
												  "./data/scripts", "*.py");
	if (plugin == "")
		return;
	scene->addPythonModule(plugin);
	pluginsAdded();
}

void MainWindow::on_action_exit_activated() {
	//TODO ask nicely
	QApplication::quit();
}

void MainWindow::on_action_open_activated() {
	if (scene) {
		//TODO show dialog
		return;
	}

	QString path = QFileDialog::getOpenFileName(this, "Open Model File", ".", "XML Files (*.xml");
	if (path == "")
		return;
	if (!path.endsWith(".xml", Qt::CaseInsensitive)) {
		qDebug() << "adding xml";
		path = path + ".xml";
	}
	scene = new SimulationScene(path);
	ui->graphicsView->setScene(scene);
	ui->actionSave_Simulation->setEnabled(true);
	ui->runButton->setEnabled(true);
	ui->actionAdd_Plugin->setEnabled(true);
	ui->actionAdd_Python_Module->setEnabled(true);
	pluginsAdded();
}

void MainWindow::on_start_stop_dateTimeChanged(const QDateTime &date) {
	start->setMaximumDateTime(stop->dateTime().addSecs(dt->value()));
	stop->setMinimumDateTime(start->dateTime().addSecs(-dt->value()));
	setNewSimulationParameters();
}

void MainWindow::on_dt_valueChanged(int value) {
	setNewSimulationParameters();
}

void MainWindow::setNewSimulationParameters() {
	SimulationParameters p = scene->getSimulation()->getSimulationParameters();
	p.start = qttopt(start->dateTime());
	p.stop = qttopt(stop->dateTime());
	p.dt = dt->value();
	scene->getSimulation()->setSimulationParameters(p);
}
