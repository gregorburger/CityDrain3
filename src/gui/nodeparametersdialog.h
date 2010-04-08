#ifndef NODEPARAMETERSDIALOG_H
#define NODEPARAMETERSDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
	class NodeParametersDialog;
}

class Node;
class NodeParameter;
class QGridLayout;
class QLineEdit;

class NodeParametersDialog : public QDialog
{
public:
	NodeParametersDialog(Node *node, QWidget *parent = 0);
	virtual ~NodeParametersDialog();

	bool updateNodeParameters();

	QString newId();

private:
	QWidget *widgetForParameter(NodeParameter *);
	Ui::NodeParametersDialog *ui;
	Node *node;
	QMap<std::string, QWidget *> widgets;
	QLineEdit *nodeId;
};

#endif // NODEPARAMETERSDIALOG_H
