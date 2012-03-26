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

#ifndef PORTITEM_H
#define PORTITEM_H

#include <QGraphicsItem>
#include <QObject>

class NodeItem;
class SimulationScene;
class ConnectionItem;

class PortItem : public QObject, public QGraphicsItem {
Q_OBJECT
Q_INTERFACES(QGraphicsItem)
public:
	enum Direction { Out, In};
	PortItem(QString portName,
			 NodeItem *parent,
			 Direction dir);

	virtual ~PortItem();

	void paint(QPainter *painter,
			   const QStyleOptionGraphicsItem *option,
			   QWidget *widget);

	QRectF boundingRect() const;

	bool isConnected();

	QString getPortName() { return portName; }
	NodeItem *getNodeItem() const { return node_item; }

	void setIsAtTop() { at_top = true; }
	void setIsAtBottom() { at_bottom = true; }

private:
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event) { hovering = true; update(); }
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { hovering = false; update(); }

	NodeItem *node_item;
	QString portName;
	Direction dir;
	bool hovering, at_top, at_bottom;
};

#endif // PORTITEM_H
