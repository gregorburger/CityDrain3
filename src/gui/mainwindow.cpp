#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "nodeitem.h"
#include <mapbasedmodel.h>
#include <noderegistry.h>
#include <simulationregistry.h>

#include <qfiledialog.h>
#include <QDebug>
#include <boost/foreach.hpp>

struct MainWindowPrivate {
	MapBasedModel model;
	NodeRegistry node_reg;
	SimulationRegistry sim_reg;
	QGraphicsScene scene;
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
			return;
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
	QTreeWidgetItem *simulations = new QTreeWidgetItem(QStringList("simuluations"));

	BOOST_FOREACH(string node_name, priv->node_reg.getRegisteredNames()) {
		QTreeWidgetItem *item = new QTreeWidgetItem(nodes);
		item->setText(0, QString::fromStdString(node_name));
	}
	ui->treeWidget->insertTopLevelItem(0, nodes);

	BOOST_FOREACH(string sim_name, priv->sim_reg.getRegisteredNames()) {
		QTreeWidgetItem *item = new QTreeWidgetItem(simulations);
		item->setText(0, QString::fromStdString(sim_name));
	}
	ui->treeWidget->insertTopLevelItem(0, simulations);
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column) {
	Node *node = priv->node_reg.createNode(item->text(0).toStdString());
	NodeItem *nitem = new NodeItem(node);
	nitem->setFlag(QGraphicsItem::ItemIsMovable, true);
	ui->graphicsView->scene()->addItem(nitem);
}
