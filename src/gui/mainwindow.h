#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
class QTreeWidgetItem;

namespace Ui {
	class MainWindow;
}

class SimulationScene;
class SimulationThread;

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

protected:
	void changeEvent(QEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void wheelEvent(QWheelEvent *);

private:
	Ui::MainWindow *ui;
	SimulationScene *scene;
	QStringList plugins;
	QStringList python_modules;
	SimulationThread *current_thread;

public Q_SLOTS:
	void on_actionAdd_Plugin_activated();
	void on_actionNewSimulation_activated();
	void on_actionSave_Simulation_activated();
	void pluginsAdded();
	void on_runButton_clicked();
	void on_actionAdd_Python_Module_activated();
	void on_action_exit_activated();
	void on_action_open_activated();
	void simulationFinished();
	void zoomIn(int times = 1);
	void zoomOut(int times = 1);
};

#endif // MAINWINDOW_H
