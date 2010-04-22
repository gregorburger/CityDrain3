#ifndef RENAMENODEDIALOG_H
#define RENAMENODEDIALOG_H

#include <QDialog>

class NodeItem;

namespace Ui {
	class RenameNodeDialog;
}

class RenameNodeDialog : public QDialog {
	Q_OBJECT

public:
	explicit RenameNodeDialog(NodeItem *item, QStringList node_names, QWidget *parent = 0);
	~RenameNodeDialog();

	QString newId() const;
private Q_SLOTS:
	void on_node_id_textChanged(const QString &text);

protected:
	void changeEvent(QEvent *e);

private:
	NodeItem *item;
	QStringList node_names;
	Ui::RenameNodeDialog *ui;
};

#endif // RENAMENODEDIALOG_H
