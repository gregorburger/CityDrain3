#ifndef SAVEASDIALOG_H
#define SAVEASDIALOG_H

#include <QDialog>

namespace Ui {
	class SaveAsDialog;
}

class QPushButton;

class SaveAsDialog : public QDialog
{
	Q_OBJECT
public:
	static QString getSaveFileName();
private Q_SLOTS:
	void on_projectName_textChanged(QString s);
private:
	explicit SaveAsDialog(QWidget *parent = 0);
	~SaveAsDialog();
	Ui::SaveAsDialog *ui;
	QPushButton *ok;
};

#endif // SAVEASDIALOG_H
