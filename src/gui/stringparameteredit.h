#ifndef STRINGPARAMETEREDIT_H
#define STRINGPARAMETEREDIT_H

#include <QWidget>

namespace Ui {
	class StringParameterEdit;
}

class StringParameterEdit : public QWidget {
Q_OBJECT
public:
	StringParameterEdit(QWidget *parent = 0);

	QString value() const;
	void setValue(const QString &value);
private Q_SLOTS:
	void on_fileButton_clicked();
private:
	Ui::StringParameterEdit *ui;
};

#endif // STRINGPARAMETEREDIT_H
