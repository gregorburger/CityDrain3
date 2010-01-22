#include "portitem.h"
#include "nodeitem.h"
#include "simulationscene.h"
#include "connectionitem.h"

#include <QFontMetrics>
#include <QPainter>

PortItem::PortItem(QString portName,
				   NodeItem *parent)
	: QGraphicsItem(parent), sink_of(0), source_of(0),
	node_item(parent), portName(portName), hovering(false),
	at_top(false), at_bottom(false)
{
	setAcceptHoverEvents(true);
	setZValue(2);
}

PortItem::~PortItem() {

}

QRectF PortItem::boundingRect() const {
	QFont f;
	QFontMetricsF fm(f);

	return fm.boundingRect(portName).adjusted(-2, -2, 2, 2);
}

void PortItem::paint(QPainter *painter,
					 const QStyleOptionGraphicsItem *option,
					 QWidget *widget) {
	(void) widget;
	(void) option;
	if (hovering) {
		painter->fillRect(boundingRect(), (isConnected() ? Qt::red : Qt::green));
	}
	QFont f;
	QFontMetricsF fm(f);
	painter->drawText(fm.boundingRect(portName), portName);
	painter->drawRect(boundingRect());
}

void PortItem::updateConnection() {
	if (sink_of) {
		sink_of->updatePositions();
		scene()->update();
	}

	if (source_of) {
		source_of->updatePositions();
		scene()->update();
	}
}
