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

private:
	QWidget *widgetForParameter(NodeParameter *);
	Ui::NodeParametersDialog *ui;
	Node *node;
	QGridLayout *layout;
	QMap<std::string, QWidget *> widgets;
	QMap<std::string, QLineEdit *> array_widgets;
};

#endif // NODEPARAMETERSDIALOG_H
