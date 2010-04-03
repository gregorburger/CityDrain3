#ifndef FINDNODEDIALOG_H
#define FINDNODEDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
	class FindNodeDialog;
}

class SimulationScene;
class NodeItem;

class FindNodeDialog : public QDialog {
	Q_OBJECT
public:
	explicit FindNodeDialog(SimulationScene *scene, QWidget *parent = 0);
	virtual ~FindNodeDialog();
	NodeItem *found;

protected:
	void changeEvent(QEvent *e);

private:
	void accept();
	Ui::FindNodeDialog *ui;
	SimulationScene *scene;
	QMap<QString, NodeItem*> items;
};

#endif // FINDNODEDIALOG_H
