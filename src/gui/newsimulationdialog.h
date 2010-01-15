#ifndef NEWSIMULATION_H
#define NEWSIMULATION_H

#include <QDialog>

namespace Ui {
	class NewSimulationDialog;
}
class SimulationRegistry;
class ISimulation;

class NewSimulationDialog : public QDialog
{
public:
	NewSimulationDialog(SimulationRegistry *registry,
				  QWidget *parent = 0,
				  Qt::WindowFlags f = 0);

	ISimulation *createSimulation();
private:
	Ui::NewSimulationDialog *ui;
	SimulationRegistry *registry;
};

#endif // NEWSIMULATION_H
