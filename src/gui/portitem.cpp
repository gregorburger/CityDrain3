#include "portitem.h"
#include "nodeitem.h"
#include "simulationscene.h"
#include "connectionitem.h"
#include <nodeconnection.h>
#include <node.h>

#include <QFontMetrics>
#include <QPainter>

PortItem::PortItem(QString portName,
				   NodeItem *parent,
				   Direction dir)
	: QGraphicsItem(parent), node_item(parent),
	  portName(portName), dir(dir),
	  hovering(false),
	  at_top(false), at_bottom(false)
{
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	setZValue(2);
}

PortItem::~PortItem() {

}

bool PortItem::isConnected() {
	SimulationScene *parentScene = static_cast<SimulationScene*>(scene());
	Q_FOREACH(ConnectionItem *item, parentScene->getConnectionsOf(node_item->getId())) {
		if (dir == In &&
			item->getSinkId() == node_item->getId() &&
			item->getSinkPortId() == portName) {
			return true;
		}
		if (dir == Out &&
			item->getSourceId() == node_item->getId() &&
			item->getSourcePortId() == portName) {
			return true;
		}
	}

	return false;
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
	NodeItem *parent = static_cast<NodeItem*>(this->parentItem());
	painter->setClipPath(mapFromParent(parent->shape().toReversed()));
	if (hovering) {
		painter->fillRect(boundingRect(), (isConnected() ? Qt::red : Qt::green));
	}
	QFont f;
	QFontMetricsF fm(f);
	painter->drawText(fm.boundingRect(portName), portName);
	painter->drawRect(boundingRect());
}
