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

#include "mainwindow.h"
#include <simulation.h>
#include <noderegistry.h>
#include <mapbasedmodel.h>
#include <logger.h>
#include <pythonexception.h>

#include "ui_mainwindow.h"
#include "simulationscene.h"
#include "findnodedialog.h"
#include "simulationthread.h"
#include "guilogsink.h"
#include "ui_timecontrols.h"
#include "renamenodedialog.h"
#include "ui_aboutdialog.h"
#include "saveasdialog.h"
#include "autolayout.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDateTimeEdit>
#include <QLabel>
#include <QSpinBox>
#include <QDate>
#include <QDateTime>
#include <QPrinter>
#include <QSettings>
#include <QStateMachine>
#include <QPlainTextEdit>
#include <boost/foreach.hpp>
#include <boost/date_time.hpp>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow), scene(new SimulationScene()), unsaved_changes(false) {
	ui->setupUi(this);

	ui->graphicsView->setScene(scene);

	current_thread = new SimulationThread();
	ui->simProgressBar->connect(current_thread->handler, SIGNAL(progress(int)),
								SLOT(setValue(int)), Qt::QueuedConnection);
	this->connect(current_thread, SIGNAL(finished()), SLOT(checkThreadOk()));
	setupTimeControls();
	setupStateMachine();

	QSettings s;
	this->restoreState(s.value("gui/mainwindow/state").toByteArray());
	this->restoreGeometry(s.value("gui/mainwindow/geometry").toByteArray());

	log_updater = new GuiLogSink(ui->logWidget);
	Log::init(log_updater);
	this->connect(scene, SIGNAL(nodesRegistered()), SLOT(pluginsAdded()));
	this->connect(scene, SIGNAL(changed(QUndoCommand*)), SLOT(simulationChanged(QUndoCommand*)));
	this->connect(scene, SIGNAL(simulationParametersChanged()), SLOT(updateTimeControls()));

	ui->actionUndo->connect(&undo_stack, SIGNAL(canUndoChanged(bool)), SLOT(setEnabled(bool)));
	ui->actionRedo->connect(&undo_stack, SIGNAL(canRedoChanged(bool)), SLOT(setEnabled(bool)));
}

MainWindow::~MainWindow() {
	//delete log_updater;
	delete state_machine;
	delete tc_widget;
	delete current_thread;
	delete scene;
	delete ui;
	Log::shutDown();
}

void MainWindow::setupTimeControls() {
	time_controls = new Ui::TimeControls();
	tc_widget = new QWidget(ui->mainToolBar);
	time_controls->setupUi(tc_widget);
	ui->mainToolBar->addWidget(tc_widget);
	this->connect(time_controls->start, SIGNAL(dateTimeChanged(QDateTime)), SLOT(start_stop_dateTimeChanged(QDateTime)));
	this->connect(time_controls->stop, SIGNAL(dateTimeChanged(QDateTime)), SLOT(start_stop_dateTimeChanged(QDateTime)));
	this->connect(time_controls->dt, SIGNAL(valueChanged(int)), SLOT(dt_valueChanged(int)));
	this->connect(time_controls->apply_time_button, SIGNAL(clicked()), SLOT(applyTime()));
}

void MainWindow::setupStateMachine() {
	state_machine = new QStateMachine();
	QState *unloaded = new QState();
	//actions
	unloaded->assignProperty(ui->actionAdd_Plugin, "enabled", false);
	unloaded->assignProperty(ui->actionAdd_Python_Module, "enabled", false);
	unloaded->assignProperty(ui->actionExport_to_pdf, "enabled", false);
	unloaded->assignProperty(ui->actionClose, "enabled", false);
	unloaded->assignProperty(ui->actionSave_Simulation, "enabled", false);
	unloaded->assignProperty(ui->actionSave_as, "enabled", false);
	unloaded->assignProperty(ui->action_delete, "enabled", false);
	unloaded->assignProperty(ui->actionCopy, "enabled", false);
	unloaded->assignProperty(ui->actionPaste, "enabled", false);
	unloaded->assignProperty(ui->actionUndo, "enabled", false);
	unloaded->assignProperty(ui->actionRedo, "enabled", false);
	unloaded->assignProperty(ui->actionRename, "enabled", false);
	unloaded->assignProperty(ui->graphicsView, "enabled", false);
	unloaded->assignProperty(ui->actionAuto_layout, "enabled", false);

	unloaded->assignProperty(ui->actionFind_node, "enabled", false);
	unloaded->assignProperty(ui->actionZoom_in, "enabled", false);
	unloaded->assignProperty(ui->actionZoom_out, "enabled", false);
	unloaded->assignProperty(ui->actionZoom_reset, "enabled", false);
	unloaded->assignProperty(ui->actionSelect_all, "enabled", false);
	unloaded->assignProperty(ui->actionAlign_vertically_center, "enabled", false);
	unloaded->assignProperty(ui->actionHorizontal_center_align, "enabled", false);
	unloaded->assignProperty(ui->actionIncrease_distance, "enabled", false);
	unloaded->assignProperty(ui->actionDecrease_distance, "enabled", false);

	//run buttons
	unloaded->assignProperty(ui->runButton, "enabled", false);
	unloaded->assignProperty(ui->stopButton, "enabled", false);
	unloaded->assignProperty(ui->runButton, "enabled", false);
	//time controls
	unloaded->assignProperty(tc_widget, "enabled", false);
	//when nothing is loaded nothing can be unsaved!!!
	unloaded->assignProperty(this, "unsaved_changes", false);
	ui->logWidget->connect(unloaded, SIGNAL(entered()), SLOT(clear()));


	QState *loaded = new QState(QState::ParallelStates);
	//actions
	loaded->assignProperty(ui->actionAdd_Plugin, "enabled", true);
	loaded->assignProperty(ui->actionAdd_Python_Module, "enabled", true);
	loaded->assignProperty(ui->actionExport_to_pdf, "enabled", true);
	loaded->assignProperty(ui->actionClose, "enabled", true);
	loaded->assignProperty(ui->action_delete, "enabled", true);
	loaded->assignProperty(ui->graphicsView, "enabled", true);
	loaded->assignProperty(ui->actionCopy, "enabled", true);
	loaded->assignProperty(ui->actionPaste, "enabled", true);
	loaded->assignProperty(ui->actionRename, "enabled", true);
	loaded->assignProperty(ui->actionSave_as, "enabled", true);
	/*loaded->assignProperty(ui->actionUndo, "enabled", true);
	loaded->assignProperty(ui->actionRedo, "enabled", true);*/
	loaded->assignProperty(ui->actionFind_node, "enabled", true);
	loaded->assignProperty(ui->actionZoom_in, "enabled", true);
	loaded->assignProperty(ui->actionZoom_out, "enabled", true);
	loaded->assignProperty(ui->actionZoom_reset, "enabled", true);
	loaded->assignProperty(ui->actionSelect_all, "enabled", true);
	loaded->assignProperty(ui->actionAlign_vertically_center, "enabled", true);
	loaded->assignProperty(ui->actionHorizontal_center_align, "enabled", true);
	loaded->assignProperty(ui->actionIncrease_distance, "enabled", true);
	loaded->assignProperty(ui->actionDecrease_distance, "enabled", true);

	loaded->assignProperty(ui->actionAuto_layout, "enabled", true);

	//run buttons
	loaded->assignProperty(ui->runButton, "enabled", true);
	loaded->assignProperty(ui->stopButton, "enabled", true);
	loaded->assignProperty(ui->runButton, "enabled", true);
	//time controls
	loaded->assignProperty(tc_widget, "enabled", true);
	this->connect(loaded, SIGNAL(entered()), SLOT(updateTimeControls()));

	unloaded->addTransition(scene, SIGNAL(loaded()), loaded);
	loaded->addTransition(scene, SIGNAL(unloaded()), unloaded);

	/*save stuff*/
	QState *save_group = new QState(loaded);
	QState *saved = new QState(save_group);
	saved->assignProperty(this, "unsaved_changes", false);
	saved->assignProperty(ui->actionSave_Simulation, "enabled", false);

	QState *unsaved = new QState(save_group);
	unsaved->assignProperty(this, "unsaved_changes", true);
	unsaved->assignProperty(ui->actionSave_Simulation, "enabled", true);

	saved->addTransition(scene, SIGNAL(changed(QUndoCommand*)), unsaved);
	unsaved->addTransition(scene, SIGNAL(saved()), saved);
	save_group->setInitialState(saved);

	this->connect(saved, SIGNAL(entered()), SLOT(simulationSaved()));

	/*running stuff*/
	QState *running_group = new QState(loaded);
	QState *not_running = new QState(running_group);
	not_running->assignProperty(ui->dockWidget, "enabled", true);
	not_running->assignProperty(ui->menuBar, "enabled", true);
	not_running->assignProperty(ui->mainToolBar, "enabled", true);
	not_running->assignProperty(ui->runButton, "enabled", true);
	not_running->assignProperty(ui->stopButton, "enabled", false);

	QState *running = new QState(running_group);
	running->assignProperty(ui->dockWidget, "enabled", false);
	running->assignProperty(ui->menuBar, "enabled", false);
	running->assignProperty(ui->mainToolBar, "enabled", false);
	running->assignProperty(ui->runButton, "enabled", false);
	running->assignProperty(ui->stopButton, "enabled", true);

	not_running->addTransition(current_thread, SIGNAL(started()), running);
	running->addTransition(current_thread, SIGNAL(finished()), not_running);
	running_group->setInitialState(not_running);

	QState *time_group = new QState(loaded);

	QState *time_unchanged = new QState(time_group);
	time_unchanged->assignProperty(time_controls->apply_time_button, "enabled", false);

	QState *time_changed = new QState(time_group);
	time_changed->assignProperty(time_controls->apply_time_button, "enabled", true);

	time_unchanged->addTransition(time_controls->start, SIGNAL(dateTimeChanged(QDateTime)), time_changed);
	time_unchanged->addTransition(time_controls->stop, SIGNAL(dateTimeChanged(QDateTime)), time_changed);
	time_unchanged->addTransition(time_controls->dt, SIGNAL(valueChanged(int)), time_changed);
	time_changed->addTransition(scene, SIGNAL(simulationParametersChanged()), time_unchanged);
	time_group->setInitialState(time_unchanged);

	state_machine->addState(unloaded);
	state_machine->addState(loaded);
	state_machine->setInitialState(unloaded);
	state_machine->start();
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

void MainWindow::on_actionAdd_Plugin_triggered() {
	QString plugin = QFileDialog::getOpenFileName(this, "select plugin", ".", "*.so *.dll");
	if (plugin == "")
		return;
	scene->addPlugin(plugin);
	pluginsAdded();
}

void MainWindow::zoomIn(int times) {
	ui->graphicsView->scale(times * 1.2, times * 1.2);
}

void MainWindow::zoomOut(int times) {
	ui->graphicsView->scale(1/(times * 1.2), 1/(times * 1.2));
}

void MainWindow::on_actionZoom_in_triggered() {
	zoomIn(1);
}

void MainWindow::on_actionZoom_out_triggered() {
	zoomOut(1);
}

void MainWindow::on_actionZoom_reset_triggered() {
	//scene->update();
	QRectF bounds;
	if (scene->selectedItems().size() == 0) {
		bounds = scene->itemsBoundingRect();
	} else {
		QList<NodeItem *> items = scene->filterNodes(scene->selectedItems());
		Q_FOREACH(NodeItem *item, items) {
			bounds = bounds.united(item->mapRectToScene(item->boundingRect()));
		}
	}

	ui->graphicsView->setSceneRect(bounds);
	ui->graphicsView->fitInView(bounds, Qt::KeepAspectRatio);
}

typedef std::pair<std::string, std::vector<std::string> > names_and_sources_pair;

void MainWindow::pluginsAdded() {
	ui->treeWidget->clear();

	BOOST_FOREACH(names_and_sources_pair p, scene->getNodeRegistry()->getRegisteredNamesAndSources()) {
		QString sourceName = QString::fromStdString(p.first);
		QTreeWidgetItem *parent = new QTreeWidgetItem(QStringList(sourceName));


		BOOST_FOREACH(std::string node_name, p.second) {
			QString qnode_name = QString::fromStdString(node_name);
			if (qnode_name.startsWith("CycleNode"))
				continue;
			QTreeWidgetItem *item = new QTreeWidgetItem(parent);
			item->setText(0, qnode_name);
		}
		ui->treeWidget->insertTopLevelItem(0, parent);
		parent->setExpanded(true);
	}
}

void MainWindow::on_runButton_clicked() {
	current_thread->setSimulation(scene->getSimulation());
	current_thread->start();
}

void MainWindow::on_stopButton_clicked() {
	current_thread->getSimulation()->stop();
}

void MainWindow::on_actionNewSimulation_triggered() {
	if (!unload()) {
		return;
	}
	scene->_new();
}

void MainWindow::updateTimeControls() {
	time_controls->start->setDateTime(scene->getStart());
	time_controls->stop->setDateTime(scene->getStop());
	time_controls->dt->setValue(scene->getDt());
}

void MainWindow::on_actionSave_Simulation_triggered() {
	if (scene->getModelFileName() == "") {
		on_actionSave_as_triggered();
		return;
	}
	scene->save(scene->getModelFileName());
}

void MainWindow::on_actionSave_as_triggered() {
	QString fileName = SaveAsDialog::getSaveFileName();
	if (fileName == "")
		return;
	if (!fileName.endsWith(".xml", Qt::CaseInsensitive)) {
		fileName = fileName + ".xml";
	}
	scene->save(fileName);
}

void MainWindow::on_actionAdd_Python_Module_triggered() {
#ifndef PYTHON_DISABLED
	QString plugin;
	plugin = QFileDialog::getOpenFileName(this,
										  "select python module",
										  "./data/scripts",
										  "*.py");
	if (plugin == "")
		return;
	try {
		scene->addPythonModule(plugin);
	} catch (PythonException e) {
		QString msg = QString("Error loading python module %1\n"
							  "see log for more information")
							  .arg(plugin);
		
		QMessageBox::critical(this, "Python module loading failed", msg);
		return;
	}

	pluginsAdded();
#else
	QMessageBox::critical(0, "python disabled", "Python support is disabled for this build");
#endif
}

void MainWindow::on_action_exit_triggered() {
	this->close();
}

void MainWindow::on_action_open_triggered() {
	if (!unload()) {
		return;
	}

	QString path;
	path = QFileDialog::getOpenFileName(this,
										"Open Model File",
										".",
										"XML Files (*.xml)");
	if (path == "")
		return;
	try {
		scene->load(path);
	} catch (PythonException e) {
		QString msg = QString("Python error while loading model file.\n"
		                      "See log for more information.");
		QMessageBox::critical(this, "Model loading failed", msg);
		return;
	}
	setWindowTitle(QString("%2 (%1)").arg(path, QApplication::applicationName()));
}

void MainWindow::start_stop_dateTimeChanged(const QDateTime &date) {
	if (time_controls->start->dateTime() == date) {
		time_controls->stop->setMinimumDateTime(time_controls->start->dateTime().addSecs(+time_controls->dt->value()));
	}
	if (time_controls->stop->dateTime() == date) {
		time_controls->start->setMaximumDateTime(time_controls->stop->dateTime().addSecs(-time_controls->dt->value()));
	}
	//time_controls->apply_time_button->setEnabled(true);
}

void MainWindow::dt_valueChanged(int value) {
	//time_controls->apply_time_button->setEnabled(true);
}

void MainWindow::applyTime() {
	SimulationParameters p = scene->getSimulation()->getSimulationParameters();
	p.start = qttopt(time_controls->start->dateTime());
	p.stop = qttopt(time_controls->stop->dateTime());
	p.dt = time_controls->dt->value();

	if (!scene->setSimulationParameters(p)) {
		QMessageBox::critical(this, "", "Some nodes failed to initialize!\n"
										"See log window for more informations!");
		return;
	}
	//time_controls->apply_time_button->setEnabled(false);
}

void MainWindow::closeEvent(QCloseEvent *event) {
	if (unload()) {
		QSettings s;
		s.setValue("gui/mainwindow", this->saveState());
		s.setValue("gui/mainwindow/geometry", this->saveGeometry());
		event->accept();
	} else {
		event->ignore();
	}
}


void MainWindow::simulationSaved() {
	setWindowTitle(QString("%2 (%1)").arg(scene->getModelFileName(), QApplication::applicationName()));
}

void MainWindow::simulationChanged(QUndoCommand *cmd) {
	setWindowTitle(QString("%2 (%1) *").arg(scene->getModelFileName(), QApplication::applicationName()));
	if (cmd) {
		undo_stack.push(cmd);
	}
}

void MainWindow::on_actionExport_to_pdf_triggered() {
	QString fileName = QFileDialog::getSaveFileName(this, "Export To PDF", ".", "PDF Files (*.pdf)");
	if (fileName == "")
		return;

	QPrinter printer;

	printer.setOutputFileName(fileName);
	printer.setOutputFormat(QPrinter::PdfFormat);
	QRectF rect = scene->itemsBoundingRect();
	printer.setPaperSize(QSizeF(rect.width(), rect.height()), QPrinter::Millimeter);

	QPainter painter(&printer);
	painter.setRenderHint(QPainter::Antialiasing);
	scene->clearSelection();
	scene->render(&painter, QRectF(), scene->itemsBoundingRect());//, QRectF(0, 0, rect.width(), rect.height()), rect);
}

bool MainWindow::unload() {
	if (unsaved_changes) {
		QString message("%1 has been modified. Do you want to save?");
		QMessageBox::StandardButton ret = QMessageBox::question(this,
							  "unsaved changes",
							  message.arg(scene->getModelFileName()),
							  QMessageBox::No | QMessageBox::Cancel | QMessageBox::Yes,
							  QMessageBox::Yes);
		if (ret == QMessageBox::Yes) {
			on_actionSave_Simulation_triggered();
		}
		if (ret == QMessageBox::Cancel) {
			return false;
		}
	}
	ui->treeWidget->clear();
	undo_stack.clear();
	scene->unload();
	this->setWindowTitle(QApplication::applicationName());
	return true;
}

void MainWindow::on_actionFind_node_triggered() {
	FindNodeDialog fn(scene, this);
	if (fn.exec() && fn.found) {
		ui->graphicsView->centerOn(fn.found);
		scene->clearSelection();
		fn.found->setSelected(true);
	}
}

void MainWindow::on_actionHorizontal_center_align_triggered() {
	if (scene->selectedItems().size() < 2) {
		return;
	}

	int y = scene->selectedItems()[0]->pos().y();

	Q_FOREACH(QGraphicsItem *item, scene->filterNodes(scene->selectedItems())) {
		int x = item->pos().x();
		item->setPos(x, y);
	}
}

void MainWindow::on_actionAlign_vertically_center_triggered() {
	if (scene->selectedItems().size() < 2) {
		return;
	}

	int x = scene->selectedItems()[0]->pos().x();

	Q_FOREACH(QGraphicsItem *item, scene->filterNodes(scene->selectedItems())) {
		int y = item->pos().y();
		item->setPos(x, y);
	}
}

void MainWindow::on_actionCopy_triggered() {
	scene->copy();
}

void MainWindow::on_actionPaste_triggered() {
	scene->paste();
}

void MainWindow::on_actionClose_triggered() {
	unload();
}

void MainWindow::checkThreadOk() {
	if (current_thread->hasFailed()) {
		QString msg = QString("failed to run simulation.\n"
							  "python error in log");
		
		QMessageBox::critical(this, "Python Error", msg);
	}
}

void MainWindow::on_action_delete_triggered() {
	scene->deleteSelectedItems();
}

void MainWindow::on_actionUndo_triggered() {
	if (!undo_stack.canUndo()) {
		return;
	}
	undo_stack.undo();
}

void MainWindow::on_actionRedo_triggered() {
	if (!undo_stack.canRedo()) {
		return;
	}
	undo_stack.redo();
}

void MainWindow::on_actionRename_triggered() {
	Q_FOREACH(NodeItem *item, scene->filterNodes(scene->selectedItems())) {
		NodeItem *nitem = static_cast<NodeItem *>(item);
		RenameNodeDialog rn(nitem, scene->getNodeNames(), this);
		if (rn.exec()) {
			scene->renameNodeItem(nitem->getId(), rn.newId());
		}
	}
}

/*bool xLessThan(const NodeItem *g1, const NodeItem &g2) {
	return g1.pos().x() < g2.pos().x();
}*/

bool yLessThan(NodeItem *g1, NodeItem *g2) {
	return g1->pos().y() < g2->pos().y();
}

void MainWindow::incVerticalDistance(double inc) {
	double ydist = 0.0;
	QList<NodeItem *> items = scene->filterNodes(scene->selectedItems());
	qSort(items.begin(), items.end(), yLessThan);
	for (int i = 0; i < items.size() - 1; i++) {
		ydist += items[i+1]->pos().y() - items[i]->pos().y();
	}
	ydist /= items.size() - 1;
	ydist += inc;
	for (int i = 1; i < items.size(); i++) {
		QPointF pos = items[i-1]->pos();
		pos.setY(pos.y() + ydist);
		items[i]->setPos(pos);
	}
}

void MainWindow::on_actionIncrease_distance_triggered() {
	incVerticalDistance(1);
}

void MainWindow::on_actionDecrease_distance_triggered() {
	incVerticalDistance(-1);
}

void MainWindow::on_actionSelect_all_triggered() {
	Q_FOREACH(QGraphicsItem *item, scene->items()) {
		item->setSelected(true);
	}
}

void MainWindow::on_action_About_triggered() {
	QDialog dialog(this);
	Ui::AboutDialog d;
	d.setupUi(&dialog);
	dialog.exec();
}

void MainWindow::on_actionAuto_layout_triggered() {
#ifdef WITH_AUTOLAYOUT
	AutoLayout a(scene);
	a.layout();
	on_actionZoom_reset_triggered();
#else
	QMessageBox::critical(0, "autolayout disabled", "Autolayout support is disabled for this build");
#endif
}
