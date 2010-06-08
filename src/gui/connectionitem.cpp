#include "connectionitem.h"
#include "portitem.h"
#include <QPainter>
#include <simulationscene.h>
#include <nodeconnection.h>
#include <node.h>

QPointF center(PortItem *item) {
	Q_ASSERT(item);
	return item->mapToScene(item->boundingRect().center());
}

ConnectionItem::ConnectionItem(SimulationScene *scene, QString source_id, QString source_port_id)
	: QGraphicsItem(0, scene), scene(scene),
	  source_id(source_id), source_port_id(source_port_id),
	  hovered(false) {
	source = center(scene->findItem(source_id)->getOutPort(source_port_id));
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
}

ConnectionItem::ConnectionItem(SimulationScene *scene, NodeConnection *con)
	: QGraphicsItem(0, scene), scene(scene), hovered(false) {
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setConnection(con);
}

ConnectionItem::~ConnectionItem() {
}

void ConnectionItem::setSink(const QPointF &sink) {
	this->sink = sink;
	updatePaths();
}

void ConnectionItem::setConnection(NodeConnection *con) {
	this->connection = con;
	source_id = QString::fromStdString(con->source->getId());
	source_port_id = QString::fromStdString(con->source_port);
	sink_id = QString::fromStdString(con->sink->getId());
	sink_port_id = QString::fromStdString(con->sink_port);
	updatePositions();
}

qreal mid(qreal start, qreal stop) {
	return qMin(start, stop) + (qAbs(start - stop) / 2.0);
}

void ConnectionItem::updatePositions() {
	source = center(scene->findItem(source_id)->getOutPort(source_port_id));
	sink = center(scene->findItem(sink_id)->getInPort(sink_port_id));
	updatePaths();
}

void ConnectionItem::updatePaths() {
	connection_path = QPainterPath();
	qreal x = (source - sink).x();
	QPointF c1(source.x() - x / 2.0, source.y());
	QPointF c2(sink.x() + x / 2.0, sink.y());
	connection_path.moveTo(source);
	connection_path.cubicTo(c1, c2, sink);
	handle_path = QPainterPath();
	handle_path.addEllipse(mid(source.x(), sink.x()) - 5, mid(source.y(),sink.y()) - 5, 10, 10);
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
