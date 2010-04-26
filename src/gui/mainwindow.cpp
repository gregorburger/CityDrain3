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

#include <QFileDialog>
#include <QDebug>
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

	log_updater = new GuiLogSink();
	Log::init(log_updater);
	ui->logWidget->connect(log_updater, SIGNAL(newLogLine(QString)), SLOT(appendPlainText(QString)), Qt::QueuedConnection);
	this->connect(scene, SIGNAL(nodesRegistered()), SLOT(pluginsAdded()));
}

MainWindow::~MainWindow() {
	if (scene)
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
	unloaded->assignProperty(ui->graphicsView, "enabled", false);
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
	saved->assignProperty(ui->actionSave_as, "enabled", false);
	saved->assignProperty(ui->actionSave_Simulation, "enabled", false);

	QState *unsaved = new QState(save_group);
	unsaved->assignProperty(this, "unsaved_changes", true);
	unsaved->assignProperty(ui->actionSave_as, "enabled", true);
	unsaved->assignProperty(ui->actionSave_Simulation, "enabled", true);

	saved->addTransition(scene, SIGNAL(changed()), unsaved);
	unsaved->addTransition(scene, SIGNAL(saved()), saved);
	save_group->setInitialState(saved);

	this->connect(unsaved, SIGNAL(entered()), SLOT(simulationChanged()));
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

void MainWindow::zoomIn(int times) {
	ui->graphicsView->scale(times * 1.2, times * 1.2);
}

void MainWindow::zoomOut(int times) {
	ui->graphicsView->scale(1/(times * 1.2), 1/(times * 1.2));
}

void MainWindow::on_actionZoom_in_activated() {
	zoomIn(1);
}

void MainWindow::on_actionZoom_out_activated() {
	zoomOut(1);
}

void MainWindow::on_actionZoom_reset_activated() {
	ui->graphicsView->fitInView(ui->graphicsView->sceneRect(), Qt::KeepAspectRatio);
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
	current_thread->setSimulation(scene->getSimulation());
	current_thread->start();
}

void MainWindow::on_stopButton_clicked() {
	current_thread->getSimulation()->stop();
}

void MainWindow::on_actionNewSimulation_activated() {
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

void MainWindow::on_actionSave_Simulation_activated() {
	if (scene->getModelFileName() == "") {
		on_actionSave_as_activated();
		return;
	}
	scene->save(scene->getModelFileName());
}

void MainWindow::on_actionSave_as_activated() {
	QString fileName = QFileDialog::getSaveFileName(this, "Save Model File", ".", "XML Files (*.xml)");
	if (fileName == "")
		return;
	if (!fileName.endsWith(".xml", Qt::CaseInsensitive)) {
		fileName = fileName + ".xml";
	}
	scene->save(fileName);
}

void MainWindow::on_actionAdd_Python_Module_activated() {
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
		QString type = QString::fromStdString(e.type);
		QString value = QString::fromStdString(e.value);
		QString msg = QString("failed to load python module.\n"
							  "python error: \n\t%1\t\n%2")
							  .arg(type, value);
		Logger(Error) << e.type;
		Logger(Error) << e.value;
		QMessageBox::critical(this, "Python module failure", msg);
		return;
	}

	pluginsAdded();
#else
	QMessageBox::critical(0, "python disabled", "Python support is disabled for this build");
#endif
}

void MainWindow::on_action_exit_activated() {
	this->close();
}

void MainWindow::on_action_open_activated() {
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
		QString type = QString::fromStdString(e.type);
		QString value = QString::fromStdString(e.value);
		QString msg = QString("failed to load model.\n"
							  "python error: \n\t%1\t\n%2")
							  .arg(type, value);
		Logger(Error) << e.type;
		Logger(Error) << e.value;
		QMessageBox::critical(this, "Python module failure", msg);
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
	time_controls->apply_time_button->setEnabled(true);
}

void MainWindow::dt_valueChanged(int value) {
	time_controls->apply_time_button->setEnabled(true);
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
	time_controls->apply_time_button->setEnabled(false);
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

void MainWindow::simulationChanged() {
	setWindowTitle(QString("%2 (%1) *").arg(scene->getModelFileName(), QApplication::applicationName()));
}

void MainWindow::on_actionExport_to_pdf_activated() {
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
			on_actionSave_Simulation_activated();
		}
		if (ret == QMessageBox::Cancel) {
			return false;
		}
	}
	ui->treeWidget->clear();
	scene->unload();
	this->setWindowTitle(QApplication::applicationName());
	return true;
}

void MainWindow::on_actionFind_node_activated() {
	FindNodeDialog fn(scene, this);
	if (fn.exec() && fn.found) {
		ui->graphicsView->centerOn(fn.found);
		scene->clearSelection();
		fn.found->setSelected(true);
	}
}

void MainWindow::on_actionHorizontal_center_align_activated() {
	if (scene->selectedItems().size() < 2) {
		return;
	}

	int y = scene->selectedItems()[0]->pos().y();

	Q_FOREACH(QGraphicsItem *item, scene->selectedItems()) {
		int x = item->pos().x();
		item->setPos(x, y);
	}
}

void MainWindow::on_actionAlign_vertically_center_activated() {
	if (scene->selectedItems().size() < 2) {
		return;
	}

	int x = scene->selectedItems()[0]->pos().x();

	Q_FOREACH(QGraphicsItem *item, scene->selectedItems()) {
		int y = item->pos().y();
		item->setPos(x, y);
	}
}

void MainWindow::on_actionCopy_activated() {
	scene->copy();
}

void MainWindow::on_actionPaste_activated() {
	scene->paste();
}

void MainWindow::on_actionClose_activated() {
	unload();
}

void MainWindow::checkThreadOk() {
	if (current_thread->hasFailed()) {
		PythonException e = current_thread->exception;
		QString type = QString::fromStdString(e.type);
		QString value = QString::fromStdString(e.value);
		QString msg = QString("failed to run simulation.\n"
							  "python error: \n\t%1\t\n%2")
							  .arg(type, value);
		Logger(Error) << e.type;
		Logger(Error) << e.value;
		QMessageBox::critical(this, "Python module failure", msg);
	}
}

void MainWindow::on_action_delete_activated() {
	scene->deleteSelectedItems();
}
