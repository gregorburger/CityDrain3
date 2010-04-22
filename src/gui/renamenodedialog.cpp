#include "renamenodedialog.h"
#include "ui_renamenodedialog.h"
#include <QPushButton>
#include <nodeitem.h>

RenameNodeDialog::RenameNodeDialog(NodeItem *item, QStringList node_names, QWidget *parent) :
	QDialog(parent), item(item), node_names(node_names),
	ui(new Ui::RenameNodeDialog) {
	ui->setupUi(this);
	ui->node_class->setText(item->getClassName());
	ui->node_id->setText(item->getId());
}

RenameNodeDialog::~RenameNodeDialog() {
	delete ui;
}

void RenameNodeDialog::changeEvent(QEvent *e) {
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void RenameNodeDialog::on_node_id_textChanged(const QString &text) {
		QPalette p = ui->node_class->palette();
		p.setColor(p.Base, node_names.contains(text) ? Qt::red : Qt::white);
		ui->node_id->setPalette(p);
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!node_names.contains(text));
}

QString RenameNodeDialog::newId() const {
	return ui->node_id->text();
}
