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
class QDateTimeEdit;
class QSpinBox;
class QDateTime;
class LogUpdaterThread;

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

protected:
	void changeEvent(QEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void closeEvent(QCloseEvent *);

private:
	Ui::MainWindow *ui;
	SimulationScene *scene;
	SimulationThread *current_thread;
	bool model_unsaved;
	QDateTimeEdit *stop, *start;
	QSpinBox *dt;
	LogUpdaterThread *log_updater;

public Q_SLOTS:
	void on_actionAdd_Plugin_activated();
	void on_actionNewSimulation_activated();
	void on_actionSave_Simulation_activated();
	void on_runButton_clicked();
	void on_stopButton_clicked();
	void on_actionAdd_Python_Module_activated();
	void on_action_exit_activated();
	void on_action_open_activated();
	void on_actionZoom_in_activated();
	void on_actionZoom_out_activated();
	void on_actionZoom_reset_activated();

	void start_stop_dateTimeChanged(const QDateTime &date);
	void dt_valueChanged(int value);

	void simulationThreadStarted();
	void simulationThreadFinished();
	void zoomIn(int times = 1);
	void zoomOut(int times = 1);
	void pluginsAdded();
	void sceneChanged();
	void setNewSimulationParameters();
};

#endif // MAINWINDOW_H
