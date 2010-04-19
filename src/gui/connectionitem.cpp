#include "connectionitem.h"
#include "portitem.h"
#include <QPainter>
#include <QDebug>

ConnectionItem::ConnectionItem(PortItem *source,
							   QPointF second,
							   QGraphicsItem *parent,
							   QGraphicsScene *scene)
			  : QGraphicsItem(parent, scene), source(source), sink(0), hovered(false) {
	this->second = second;
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	updatePositions();
}

ConnectionItem::ConnectionItem(PortItem *source,
							   PortItem *sink,
							   QGraphicsItem *parent,
							   QGraphicsScene *scene)
			  : QGraphicsItem(parent, scene), source(source), sink(sink), hovered(false) {
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	updatePositions();
}

ConnectionItem::~ConnectionItem() {
	if (source)
		source->setSourceOf(0);
	if (sink)
		sink->setSinkOf(0);
}

void ConnectionItem::setSink(PortItem *sink) {
	this->sink = sink;
}

void ConnectionItem::setSecond(QPointF second) {
	this->second = second;
	updatePositions();
}


qreal mid(qreal first, qreal second) {
	return qMin(first, second) + (qAbs(first - second) / 2.0);
}

void ConnectionItem::updatePositions() {
	connection_path = QPainterPath();
	first = source->scenePos();
	first.setX(first.x() + source->boundingRect().right());
	if (sink) {
		second = sink->scenePos();
		second.setX(second.x() + sink->boundingRect().left());
	}
	qreal x = (first - second).x();
	QPointF c1(first.x() - x / 2.0, first.y());
	QPointF c2(second.x() + x / 2.0, second.y());
	connection_path.moveTo(first);
	connection_path.cubicTo(c1, c2, second);
	handle_path = QPainterPath();
	handle_path.addEllipse(mid(first.x(), second.x()) - 5, mid(first.y(),second.y()) - 5, 10, 10);
	united = handle_path.united(connection_path);
	update();
}

QRectF ConnectionItem::boundingRect() const {
	return united.boundingRect();
}

void ConnectionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *w) {
	QColor c;
	c = isSelected() ? Qt::gray : Qt::white;
	if (hovered) {
		c = Qt::green;
	}
	QBrush brush(c);
	QPen pen(Qt::black);
	painter->strokePath(connection_path, pen);
	painter->fillPath(handle_path, brush);
	painter->strokePath(handle_path, pen);
}

void ConnectionItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
	hovered = true;
	update();
}

void ConnectionItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
	hovered = false;
	update();
}
