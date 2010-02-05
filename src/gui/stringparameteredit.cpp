#include "stringparameteredit.h"
#include <ui_stringparameteredit.h>

#include <QFileDialog>

StringParameterEdit::StringParameterEdit(QWidget *parent)
	: QWidget(parent) {
	ui = new Ui::StringParameterEdit();
	ui->setupUi(this);
}

void StringParameterEdit::on_fileButton_clicked() {
	QString name = QFileDialog::getSaveFileName(this, "Choose File or FileName", QDir::current().path());
	if (name != "") {
		ui->value->setText(name);
	}
}

QString StringParameterEdit::value() const {
	return ui->value->text();
}

void StringParameterEdit::setValue(const QString &value) {
	ui->value->setText(value);
}

