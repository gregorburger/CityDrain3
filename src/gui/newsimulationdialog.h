#ifndef NEWSIMULATION_H
#define NEWSIMULATION_H

#include <QDialog>
#include <boost/date_time/posix_time/ptime.hpp>

namespace Ui {
	class NewSimulationDialog;
}
class SimulationRegistry;
class ISimulation;
class SimulationScene;
class QDateTime;

class NewSimulationDialog : public QDialog
{
	Q_OBJECT
public:
	NewSimulationDialog(QWidget *parent = 0,
						Qt::WindowFlags f = 0);

	SimulationScene *createSimulationScene();
	Ui::NewSimulationDialog *ui;
private Q_SLOTS:
	void on_start_dateTimeChanged(const QDateTime &date);
private:
	void defineFlow();
	SimulationRegistry *registry;
	SimulationScene *scene;
};

boost::posix_time::ptime qttopt(const QDateTime &dt);

#endif // NEWSIMULATION_H
