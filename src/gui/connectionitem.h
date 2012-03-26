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

#ifndef CONNECTIONITEM_H
#define CONNECTIONITEM_H

#include <QGraphicsItem>
#include <QPen>

class PortItem;
class NodeConnection;
class SimulationScene;

class ConnectionItem : public QObject, public QGraphicsItem
{
Q_OBJECT
Q_INTERFACES(QGraphicsItem)
public:
	ConnectionItem(SimulationScene *scene, QString source, QString source_port_id);
	ConnectionItem(SimulationScene *scene, NodeConnection *con);

	virtual ~ConnectionItem();

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *w = 0);
	QPainterPath shape() const {
		return connection_path.united(handle_path);
	}

	void setSink(const QPointF &sink);
	void updatePositions();
	void updatePaths();
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	void setConnection(NodeConnection *con);
	inline NodeConnection *getConnection() const { return connection; }

	inline const QString &getSourceId() const {
		return source_id;
	}

	inline const QString &getSourcePortId() const {
		return source_port_id;
	}

	inline const QString &getSinkId() const {
		return sink_id;
	}

	inline const QString &getSinkPortId() const {
		return sink_port_id;
	}

	inline void setSourceId(const QString &new_source_id) {
		this->source_id = new_source_id;
	}

	inline void setSinkId(const QString &new_sink_id) {
		this->sink_id = new_sink_id;
	}

private:
	SimulationScene *scene;
	QString source_id, source_port_id, sink_id, sink_port_id;
	QPointF source, sink;
	QPainterPath connection_path, handle_path, united;
	NodeConnection *connection;
	bool hovered;
};

#endif // CONNECTIONITEM_H
