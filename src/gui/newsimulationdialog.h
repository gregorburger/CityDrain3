#ifndef NEWSIMULATION_H
#define NEWSIMULATION_H

#include <QDialog>
#include <boost/date_time/posix_time/ptime.hpp>

namespace Ui {
	class NewSimulationDialog;
}
class SimulationRegistry;
class ISimulation;
class QDateTime;
class QPushButton;

class NewSimulationDialog : public QDialog
{
	Q_OBJECT
public:
	NewSimulationDialog(SimulationRegistry *sim_reg,
						QWidget *parent = 0,
						Qt::WindowFlags f = 0);

	ISimulation *createSimulation();
	SimulationRegistry *registry;
	Ui::NewSimulationDialog *ui;

private Q_SLOTS:
	void on_projectRootLineEdit_textEdited(QString );
	void on_chooser_clicked();
	void on_start_dateTimeChanged(const QDateTime &date);
private:
	void defineFlow();
	QPushButton *ok;
};

boost::posix_time::ptime qttopt(const QDateTime &dt);

#endif // NEWSIMULATION_H
