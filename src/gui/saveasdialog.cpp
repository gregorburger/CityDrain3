#include "saveasdialog.h"
#include "ui_saveasdialog.h"
#include <QFileInfo>
#include <QPushButton>
#include <QDir>

SaveAsDialog::SaveAsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SaveAsDialog) {
	ui->setupUi(this);
	ui->projectRoot->setText(QDir::currentPath());
	ok = ui->buttonBox->button(QDialogButtonBox::Ok);
}

SaveAsDialog::~SaveAsDialog() {
	delete ui;
}

void SaveAsDialog::on_projectName_textChanged(QString s) {
	QFileInfo info(s);
	ok->setEnabled(!(info.exists() && info.isFile()));
}

QString SaveAsDialog::getSaveFileName() {
	SaveAsDialog d;
	if (d.exec() == QDialog::Accepted) {
		return QDir::cleanPath(QDir::currentPath()
							   + QDir::separator()
							   + d.ui->projectName->text());
	}
	return "";
}
