#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "simulationscene.h"
#include <simulationregistry.h>
#include <simulation.h>
#include <noderegistry.h>

#include <qfiledialog.h>
#include <QDebug>
#include <QMessageBox>
#include <boost/foreach.hpp>

struct MainWindowPrivate {
	SimulationScene scene;
};

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow) {
	scene = new SimulationScene();
	connect(scene, SIGNAL(simulationCreated()), SLOT(simulationCreated()));
	ui->setupUi(this);
	ui->graphicsView->setScene(scene);
	ui->graphicsView->setRenderHints(QPainter::Antialiasing);
	QStringList default_node_paths;
	default_node_paths << "./libnodes.so" << "../../libnodes.so" << "./build/libnodes.so";
	Q_FOREACH(QString path, default_node_paths) {
		if (QFile::exists(path)) {
			scene->getNodeRegistry()->addNativePlugin(path.toStdString());
			scene->getSimulationRegistry()->addNativePlugin(path.toStdString());
			on_pluginsAdded();
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
	qDebug() << "hallo";
	QString plugin = QFileDialog::getOpenFileName(this, "select plugin", ".", "*.so");
	qDebug() << plugin;
	if (plugin == "")
		return;
	scene->getNodeRegistry()->addNativePlugin(plugin.toStdString());
	scene->getSimulationRegistry()->addNativePlugin(plugin.toStdString());
	on_pluginsAdded();
}

void MainWindow::on_pluginsAdded() {
	QTreeWidgetItem *nodes = new QTreeWidgetItem(QStringList("nodes"));

	BOOST_FOREACH(std::string node_name, scene->getNodeRegistry()->getRegisteredNames()) {
		QTreeWidgetItem *item = new QTreeWidgetItem(nodes);
		item->setText(0, QString::fromStdString(node_name));
	}
	ui->treeWidget->insertTopLevelItem(0, nodes);

	nodes->setExpanded(true);
}

void MainWindow::simulationCreated() {
	ui->runButton->setEnabled(true);
}

void MainWindow::on_runButton_clicked() {
	SimulationParameters sp = scene->getSimulation()->getSimulationParameters();
	scene->getSimulation()->start(sp.start);
}
