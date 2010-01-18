#include "nodeitem.h"
#include "portitem.h"

#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLinearLayout>
#include <boost/foreach.hpp>

#include <node.h>

typedef pair<std::string, Flow *> port_pair;

NodeItem::NodeItem(Node* node)
	: QGraphicsItem(), node(node), margin(30) {
	setFlag(ItemIsMovable, true);
	setFlag(ItemIsSelectable, true);
	setCacheMode(DeviceCoordinateCache);
	setZValue(1);

	BOOST_FOREACH(port_pair item, *node->const_in_ports) {
		QString pname = QString::fromStdString(item.first);
		PortItem *item = new PortItem(pname, this);
		in_ports << item;
	}

	BOOST_FOREACH(port_pair item, *node->const_out_ports) {
		QString pname = QString::fromStdString(item.first);
		PortItem *item = new PortItem(pname, this);
		out_ports << item;
	}

	updateBoundingRect();
	moveItems();
}

QString NodeItem::getClassName() const {
	return QString::fromStdString(node->getClassName());
}

QString NodeItem::getId() const {
	return QString::fromStdString(node->getId());
}

QRectF NodeItem::boundingRect() const {
	return bounding;
}

void NodeItem::moveItems() {
	qDebug() << boundingRect();
	double y = boundingRect().top();
	Q_FOREACH(PortItem *item, in_ports) {
		QRectF ir = item->boundingRect();
		item->setPos(bounding.left() - ir.left(), y - ir.top());
		y += item->boundingRect().height();
	}

	y = boundingRect().top();
	Q_FOREACH(PortItem *item, out_ports) {
		QRectF ir = item->boundingRect();
		item->setPos(bounding.right() - ir.right(), y - ir.top());
		y += item->boundingRect().height();
	}
}

void NodeItem::connectItems(SimulationScene *scene) {
	Q_FOREACH(PortItem *item, in_ports) {
		item->connectConnectionSignals(scene);
	}
	Q_FOREACH(PortItem *item, out_ports) {
		item->connectConnectionSignals(scene);
	}
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	QPainterPath path;
	path.addRoundedRect(boundingRect(), 5, 5);
	if (isSelected()) {
		painter->fillPath(path, Qt::gray);
	} else {
		painter->fillPath(path, Qt::white);
	}
	painter->strokePath(path, painter->pen());
	painter->drawText(0, 0, node->getClassName());

	QFont f;
	QFontMetrics fm(f);
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsItem::mousePressEvent(event);
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsItem::mouseReleaseEvent(event);
}

void NodeItem::updateBoundingRect() {
	QFont f;
	QFontMetrics fm(f);
	QRectF r = QRectF(fm.boundingRect(node->getClassName()));
	qreal max_outp_width = 0;

	Q_FOREACH(PortItem *out, out_ports) {
		max_outp_width = max(max_outp_width, out->boundingRect().width());
	}

	qreal max_inp_width = 0;
	Q_FOREACH(PortItem *in, in_ports) {
		max_inp_width = max(max_inp_width, in->boundingRect().width());
	}

	qreal fh = fm.height() + 4;
	qreal height = fh * max(in_ports.size(), out_ports.size()) / 2;

	bounding = r.adjusted(-max(margin, max_inp_width), -max(margin,height),
						  max(margin, max_outp_width), max(margin,height));
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value) {
	if (change == ItemPositionChange) {
		//qDebug() << "ItemPositionChange";
		if (collidingItems().length() > 0) {
			//return pos();
			return value;
		}
	}
	if (change == ItemPositionHasChanged) {
		//qDebug() << "ItemPositionHasChanged";
	}
	if (change == ItemSelectedHasChanged) {
		update();
	}
	return QGraphicsItem::itemChange(change, value);
}
