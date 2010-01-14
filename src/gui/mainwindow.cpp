#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_connectiondialog.h"
#include "nodeitem.h"
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
	MainWindowPrivate() {
		simulation = 0;
		source_selection = 0;
	}

	MapBasedModel model;
	NodeRegistry node_reg;
	SimulationRegistry sim_reg;
	ISimulation *simulation;
	QGraphicsScene scene;
	QMap<string, int> ids;
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
	nodes->setExpanded(true);
	QTreeWidgetItem *simulations = new QTreeWidgetItem(QStringList("simulations"));

	BOOST_FOREACH(string node_name, priv->node_reg.getRegisteredNames()) {
		QTreeWidgetItem *item = new QTreeWidgetItem(nodes);
		item->setText(0, QString::fromStdString(node_name));
		if (!priv->ids.contains(node_name))	//set default ids to zero
			priv->ids[node_name] = 0;
	}
	ui->treeWidget->insertTopLevelItem(0, nodes);

	BOOST_FOREACH(string sim_name, priv->sim_reg.getRegisteredNames()) {
		QTreeWidgetItem *item = new QTreeWidgetItem(simulations);
		item->setText(0, QString::fromStdString(sim_name));
	}
	ui->treeWidget->insertTopLevelItem(0, simulations);
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column) {
	(void) column;
	if (item->parent()->text(0) == "nodes") {
		Node *node = priv->node_reg.createNode(item->text(0).toStdString());
		//default id is klassname_+counter
		string id_count = lexical_cast<string>(priv->ids[node->getClassName()]++);
		node->setId(node->getClassName() + string("_") + id_count);
		priv->model.addNode(node);
		qDebug() << "node with id " << QString::fromStdString(node->getId());
		NodeItem *nitem = new NodeItem(node);
		nitem->setFlag(QGraphicsItem::ItemIsMovable, true);
		ui->graphicsView->scene()->addItem(nitem);
		QObject::connect(nitem, SIGNAL(clicked(NodeItem*)), this, SLOT(nodeItemClicked(NodeItem*)));
	}
	if (item->parent()->text(0) == "simulations") {
		if (priv->simulation != 0) {
			QMessageBox::warning(this, "Simulation already set", "The simulation has already been choosen");
			return;
		}
		priv->simulation = priv->sim_reg.createSimulation(item->text(0).toStdString());
		ui->runButton->setEnabled(true);
		qDebug() << "simulation set";
	}
}

void MainWindow::nodeItemClicked(NodeItem *source) {
	qDebug() << source->getId() << "clicked" << endl;
	if (priv->source_selection == 0) {
		priv->source_selection = source;
		return;
	}

	QDialog d;
	Ui::ConnectionDialog cd;
	cd.setupUi(&d);

	cd.srcPortCombo->addItems(priv->source_selection->getOutPorts());
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
		priv->model.addConnection(con);
	}

	priv->source_selection = 0;
}
