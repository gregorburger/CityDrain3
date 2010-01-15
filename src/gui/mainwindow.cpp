#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_connectiondialog.h"
#include "newsimulationdialog.h"
#include "nodeitem.h"
#include "simulationscene.h"
#include <mapbasedmodel.h>
#include <noderegistry.h>
#include <simulationregistry.h>
#include <node.h>
#include <simulation.h>

#include <qfiledialog.h>
#include <QDebug>
#include <QMessageBox>
#include <boost/foreach.hpp>

struct MainWindowPrivate {
	MainWindowPrivate() : scene(&node_reg) {
		simulation = 0;
		source_selection = 0;
	}

	NodeRegistry node_reg;
	SimulationRegistry sim_reg;
	ISimulation *simulation;
	SimulationScene scene;
	NodeItem *source_selection;
};

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow) {
	ui->setupUi(this);
	priv = new MainWindowPrivate();
	ui->graphicsView->setScene(&priv->scene);
	ui->graphicsView->setRenderHints(QPainter::Antialiasing);
	QStringList default_node_paths;
	default_node_paths << "./libnodes.so" << "../../libnodes.so" << "./build/libnodes.so";
	Q_FOREACH(QString path, default_node_paths) {
		if (QFile::exists(path)) {
			priv->node_reg.addNativePlugin(path.toStdString());
			priv->sim_reg.addNativePlugin(path.toStdString());
			on_pluginsAdded();
			break;
		}
	}
}

MainWindow::~MainWindow() {
	delete priv;
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
	priv->node_reg.addNativePlugin(plugin.toStdString());
	priv->sim_reg.addNativePlugin(plugin.toStdString());
	on_pluginsAdded();
}

void MainWindow::on_pluginsAdded() {
	QTreeWidgetItem *nodes = new QTreeWidgetItem(QStringList("nodes"));

	BOOST_FOREACH(string node_name, priv->node_reg.getRegisteredNames()) {
		QTreeWidgetItem *item = new QTreeWidgetItem(nodes);
		item->setText(0, QString::fromStdString(node_name));
	}
	ui->treeWidget->insertTopLevelItem(0, nodes);

	nodes->setExpanded(true);
}

void MainWindow::nodeItemClicked(NodeItem *source) {
	if (!priv->simulation) {
		if (!newSimulation()) {
			return;
		}
	}
	qDebug() << source->getId() << "clicked" << endl;
	if (priv->source_selection == 0) {
		priv->source_selection = source;
		return;
	}

	QDialog d;
	Ui::ConnectionDialog cd;
	cd.setupUi(&d);

	/*cd.srcPortCombo->addItems(priv->source_selection->getOutPorts());
	cd.snkPortCombo->addItems(source->getInPorts());
	if (d.exec()) {
		Node *src = priv->source_selection->getNode();
		Node *snk = source->getNode();
		string src_port = cd.srcPortCombo->currentText().toStdString();
		string snk_port = cd.snkPortCombo->currentText().toStdString();
		NodeConnection *con;
		con = priv->simulation->createConnection(src,
												 src_port,
												 snk,
												 snk_port);
		//priv->scene.model.addConnection(con);
	}*/

	priv->source_selection = 0;
}

bool MainWindow::newSimulation() {
	NewSimulationDialog nsd(&priv->sim_reg, this);

	if (nsd.exec()) {
		priv->simulation = nsd.createSimulation();
		ui->runButton->setEnabled(true);
		return true;
	}
	return false;
}
