#include "portitem.h"
#include <nodeitem.h>

#include <QFontMetrics>
#include <QPainter>

PortItem::PortItem(QString portName,
				   NodeItem *parent)
	: QGraphicsItem(parent), portName(portName), hovering(false) {
}

PortItem::~PortItem() {

}

QRectF PortItem::boundingRect() {
	QFont f;
	QFontMetricsF fm(f);

	return fm.boundingRect(portName).adjusted(-3, -3, 3, 3);
}

void PortItem::paint(QPainter *painter,
					 const QStyleOptionGraphicsItem *option,
					 QWidget *widget) {
	(void) widget;
	(void) option;
	if (hovering) {
		painter->fillRect(boundingRect(), (connected ? Qt::red : Qt::green));
	}
	QFont f;
	QFontMetricsF fm(f);
	painter->drawText(fm.boundingRect(portName), portName);
	painter->drawRect(boundingRect());
}
