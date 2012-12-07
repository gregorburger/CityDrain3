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

#ifndef NODEITEM_H
#define NODEITEM_H

#include <string>
unsigned int qHash(std::string s);
#include <QGraphicsItem>
class Node;
class PortItem;
class QUndoCommand;

typedef QMap<std::string, std::string> SavedParameters;

class NodeItem : public QObject, public QGraphicsItem
{
Q_OBJECT
Q_INTERFACES(QGraphicsItem)
public:
	NodeItem(Node *node);
	virtual ~NodeItem();
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

	void updateBoundingRect();

	QString getId() const;
	QString getClassName() const;

	Node *getNode() const { return node; }
	QMap<std::string, PortItem *> in_ports, out_ports;

	QPainterPath shape() const;
	void updatePorts();

	PortItem *getInPort(QString id);
	PortItem *getOutPort(QString id);
	bool changeParameters(bool _new = false);
	SavedParameters saveParameters();
	void restoreParameters(SavedParameters p);

Q_SIGNALS:
	void changed(QUndoCommand *cmd);

private:
	void moveItems();

	QRectF bounding;
	Node *node;
	qreal margin;
};

#endif // NODEITEM_H
