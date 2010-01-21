#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "simulationscene.h"
#include "simulationsaver.h"
#include <simulationregistry.h>
#include <simulation.h>
#include <noderegistry.h>
#include <module.h>

#include "newsimulationdialog.h"
#include "simulationthread.h"

#include <qfiledialog.h>
#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>
#include <boost/foreach.hpp>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow) {
	scene = new SimulationScene();
	ui->setupUi(this);
	ui->graphicsView->setScene(scene);
	ui->graphicsView->setRenderHints(QPainter::Antialiasing);
	QStringList default_node_paths;
	default_node_paths << "./libnodes.so" << "../../libnodes.so" << "./build/libnodes.so";
	default_node_paths << "./nodes.dll"  << "../../nodes.dll" << "./build/nodes.dll";
	Q_FOREACH(QString path, default_node_paths) {
		if (QFile::exists(path)) {
			scene->getNodeRegistry()->addNativePlugin(path.toStdString());
			scene->getSimulationRegistry()->addNativePlugin(path.toStdString());
			pluginsAdded();
			plugins << path;
			break;
		}
	}
}

MainWindow::~MainWindow() {
	delete scene;
	delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
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
	QString plugin = QFileDialog::getOpenFileName(this, "select plugin", ".", "*.so");
	if (plugin == "")
		return;
	plugins << plugin;
	scene->getNodeRegistry()->addNativePlugin(plugin.toStdString());
	scene->getSimulationRegistry()->addNativePlugin(plugin.toStdString());
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
		QTreeWidgetItem *item = new QTreeWidgetItem(nodes);
		item->setText(0, QString::fromStdString(node_name));
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
	NewSimulationDialog ns(scene->getSimulationRegistry(), this);
	if (ns.exec()) {
		ISimulation *sim = ns.createSimulation();
		scene->setSimulation(sim);
		ui->actionSave_Simulation->setEnabled(true);
		ui->runButton->setEnabled(true);
	}
}

void MainWindow::on_actionSave_Simulation_activated() {
	QString fileName = QFileDialog::getSaveFileName(this, "Enter new Simulation file Name");
	if (fileName == "")
		return;
	SimulationSaver ss(scene->getSimulation(), fileName, plugins, python_modules);
	ss.save();
}

void MainWindow::on_actionAdd_Python_Module_activated() {
	QString plugin = QFileDialog::getOpenFileName(this,
												  "select python module",
												  "./data/scripts", "*.py");
	if (plugin == "")
		return;
	QFileInfo module_file(plugin);
	PythonEnv::getInstance()->registerNodes(scene->getNodeRegistry(),
											module_file.baseName().toStdString());
	python_modules << module_file.baseName();
	pluginsAdded();
}
