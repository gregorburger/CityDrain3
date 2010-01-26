#ifndef NEWSIMULATION_H
#define NEWSIMULATION_H

#include <QDialog>

namespace Ui {
	class NewSimulationDialog;
}
class SimulationRegistry;
class ISimulation;
class SimulationScene;

class NewSimulationDialog : public QDialog
{
public:
	NewSimulationDialog(QWidget *parent = 0,
						Qt::WindowFlags f = 0);

	SimulationScene *createSimulationScene();
private:
	Ui::NewSimulationDialog *ui;
	SimulationRegistry *registry;
	SimulationScene *scene;
	QStringList default_node_paths;
};

#endif // NEWSIMULATION_H
