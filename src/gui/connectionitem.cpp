#include "connectionitem.h"
#include "portitem.h"
#include <QPen>

ConnectionItem::ConnectionItem(PortItem *source,
							   QPointF second,
							   QGraphicsItem *parent,
							   QGraphicsScene *scene)
			  : QGraphicsPathItem(parent, scene), source(source), sink(0) {
	this->second = second;
	setAcceptHoverEvents(true);
	updatePositions();
}

ConnectionItem::ConnectionItem(PortItem *source,
							   PortItem *sink,
							   QGraphicsItem *parent,
							   QGraphicsScene *scene)
			  : QGraphicsPathItem(parent, scene), source(source), sink(sink) {
	setAcceptHoverEvents(true);
	updatePositions();
}

void ConnectionItem::setSink(PortItem *sink) {
	this->sink = sink;
}

void ConnectionItem::setSecond(QPointF second) {
	this->second = second;
	updatePositions();
}

void ConnectionItem::updatePositions() {
	first = source->scenePos() + source->boundingRect().center();
	if (sink) {
		second = sink->scenePos() + sink->boundingRect().center();
	}
	qreal x = (first - second).x();
	QPointF c1(first.x() - x / 2.0, first.y());
	QPointF c2(second.x() + x / 2.0, second.y());
	QPainterPath p;
	p.moveTo(first);
	p.cubicTo(c1, c2, second);
	this->setPath(p);
}

QPainterPath ConnectionItem::shape() const {
	return path();
}

void ConnectionItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
	QPen p = pen();
	p.setColor(Qt::red);
	setPen(p);
	update();
}

void ConnectionItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
	QPen p = pen();
	p.setColor(Qt::black);
	setPen(p);
	update();
}
