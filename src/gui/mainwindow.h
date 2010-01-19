#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
class QTreeWidgetItem;

namespace Ui {
	class MainWindow;
}

struct MainWindowPrivate;
class NodeItem;
class NodeRegistry;
class SimulationRegistry;
class SimulationScene;
class SimulationThread;

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::MainWindow *ui;
	SimulationScene *scene;
	QStringList plugins;
	SimulationThread *current_thread;

public Q_SLOTS:
	void on_actionAdd_Plugin_activated();
	void on_actionNewSimulation_activated();
	void on_actionSave_Simulation_activated();
	void on_pluginsAdded();
	void on_runButton_clicked();
	void simulationFinished();
};

#endif // MAINWINDOW_H
