/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

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
