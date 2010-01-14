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

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::MainWindow *ui;
	MainWindowPrivate *priv;

public Q_SLOTS:
	void on_actionAdd_Plugin_activated();
	void on_pluginsAdded();
	void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void nodeItemClicked(NodeItem *source);
};

#endif // MAINWINDOW_H
