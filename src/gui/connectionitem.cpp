#include "connectionitem.h"
#include "portitem.h"

ConnectionItem::ConnectionItem(PortItem *source,
							   QPointF second,
							   QGraphicsItem *parent,
							   QGraphicsScene *scene)
			  : QGraphicsPathItem(parent, scene), source(source), sink(0) {
	first = source->scenePos() + source->boundingRect().center();
	this->second = second;
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
	QPainterPath qpath = path();
	qpath.lineTo(qpath.currentPosition() + QPointF(2, 0));
	qpath.lineTo(qpath.currentPosition() + QPointF(-2, 2));
	qpath.closeSubpath();
	qpath.lineTo(qpath.currentPosition() + QPointF(2, 0));
	qpath.lineTo(qpath.currentPosition() + QPointF(-2, 2));
	return qpath;
}
