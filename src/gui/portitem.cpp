#include "portitem.h"
#include "nodeitem.h"
#include "simulationscene.h"

#include <QFontMetrics>
#include <QPainter>

PortItem::PortItem(QString portName,
				   NodeItem *parent)
	: QGraphicsItem(parent), node_item(parent), portName(portName), connected(false), hovering(false) {
	setAcceptHoverEvents(true);
	setZValue(2);
}

PortItem::~PortItem() {

}

void PortItem::connectConnectionSignals(SimulationScene *scene) {
	QObject::connect(this, SIGNAL(mouseDown(PortItem*)), scene, SLOT(connectionStart(PortItem*)));
	QObject::connect(this, SIGNAL(mouseUp(PortItem*)), scene, SLOT(connectionEnd(PortItem*)));
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
		painter->fillRect(boundingRect(), (connected ? Qt::red : Qt::green));
	}
	QFont f;
	QFontMetricsF fm(f);
	painter->drawText(fm.boundingRect(portName), portName);
	painter->drawRect(boundingRect());
}
