#include "stringparameteredit.h"
#include <ui_stringparameteredit.h>

#include <QFileDialog>

StringParameterEdit::StringParameterEdit(QWidget *parent)
	: QWidget(parent) {
	ui = new Ui::StringParameterEdit();
	ui->setupUi(this);
}

void StringParameterEdit::on_fileButton_clicked() {
	QFileInfo info(ui->value->text());
	QString path = QDir::current().path();
	if (info.dir().exists()) {
		path = info.dir().path();
	}
	QString name = QFileDialog::getSaveFileName(this,
												"Choose File or FileName",
												path);
	if (name != "") {
		ui->value->setText(QDir::current().relativeFilePath(name));
	}
}

QString StringParameterEdit::value() const {
	return ui->value->text();
}

void StringParameterEdit::setValue(const QString &value) {
	ui->value->setText(value);
}

