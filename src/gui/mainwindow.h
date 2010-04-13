#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
class QTreeWidgetItem;

namespace Ui {
	class MainWindow;
	class TimeControls;
}

class SimulationScene;
class SimulationThread;
class QDateTimeEdit;
class QSpinBox;
class QDateTime;
class QPushButton;
class GuiLogSink;
class QStateMachine;

class MainWindow : public QMainWindow {
Q_OBJECT
Q_PROPERTY(bool unsaved_changes READ getUnsavedChanges WRITE setUnsavedChanges);
public:
	MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();
	bool getUnsavedChanges() const { return unsaved_changes; }
	void setUnsavedChanges(bool unsaved) { unsaved_changes = unsaved; }

protected:
	void changeEvent(QEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void closeEvent(QCloseEvent *);

	void setupTimeControls();
	bool unload();
private:
	Ui::MainWindow *ui;
	SimulationScene *scene;
	SimulationThread *current_thread;
	bool unsaved_changes;
	Ui::TimeControls *time_controls;
	QWidget *tc_widget;
	GuiLogSink *log_updater;
	QStateMachine *state_machine;

	void setupStateMachine();

public Q_SLOTS:
	void on_actionAdd_Plugin_activated();
	void on_actionNewSimulation_activated();
	void on_actionSave_Simulation_activated();
	void on_actionSave_as_activated();
	void on_runButton_clicked();
	void on_stopButton_clicked();
	void on_actionAdd_Python_Module_activated();
	void on_action_exit_activated();
	void on_action_open_activated();
	void on_actionClose_activated();
	void on_actionZoom_in_activated();
	void on_actionZoom_out_activated();
	void on_actionZoom_reset_activated();
	void on_actionExport_to_pdf_activated();

	//edit stuff
	void on_actionFind_node_activated();
	void on_actionPaste_activated();
	void on_actionCopy_activated();

	//align stuff
	void on_actionHorizontal_center_align_activated();
	void on_actionAlign_vertically_center_activated();

	void start_stop_dateTimeChanged(const QDateTime &date);
	void dt_valueChanged(int value);
	void applyTime();

	void zoomIn(int times = 1);
	void zoomOut(int times = 1);
	void pluginsAdded();
	void simulationChanged();
	void simulationSaved();
	void updateTimeControls();

};

#endif // MAINWINDOW_H
