#include "nodeitem.h"
#include <QPainter>
#include <QDebug>

#include <node.h>

NodeItem::NodeItem(Node* node)
	: node(node) {
}



QRectF NodeItem::boundingRect() const {
	//qDebug() << "boundingRect";
	qreal penWidth = 1;
	QFont f;
	QFontMetrics fm(f);

	QRectF r = QRectF(fm.boundingRect(node->getClassName()));

	return r.adjusted(-30, -30, 30, 30);
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	qDebug() << "paint";
	//painter->drawRoundedRect(-10, -10, 20, 20, 5, 5);
	painter->drawRoundedRect(boundingRect(), 5, 5);
	painter->drawText(0, 0, node->getClassName());
}
