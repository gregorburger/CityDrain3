#include "nodeitem.h"
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <boost/foreach.hpp>

#include <node.h>

typedef pair<std::string, Flow *> port_pair;

NodeItem::NodeItem(Node* node)
	: QGraphicsItem(), node(node), margin(30) {
	BOOST_FOREACH(port_pair item, *node->const_in_ports)
		in_ports << QString::fromStdString(item.first);

	BOOST_FOREACH(port_pair item, *node->const_out_ports)
		out_ports << QString::fromStdString(item.first);

	updateBoundingRect();
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

void NodeItem::paintOutPorts(QPainter *painter, QFontMetrics &fm) {
	int x = bounding.right();
	int y = bounding.top();

	int fh = fm.height() + 4;

	Q_FOREACH(QString s, out_ports) {
		int fw = fm.width(s) + 3;
		painter->drawText(x - fw, y + fh - 2, s);
		painter->drawLine(x - fw - 3, y, x - fw - 3, y + fh);
		painter->drawLine(x - fw - 3, y + fh, x, y + fh);
		y += fh;
	}
}

void NodeItem::paintInPorts(QPainter *painter, QFontMetrics &fm) {
	int x = bounding.left();
	int y = bounding.top();

	int fh = fm.height() + 4;

	Q_FOREACH(QString s, in_ports) {
		painter->drawText(x + 3, y + fh - 2, s);
		y += fh;
	}
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	painter->drawRoundedRect(boundingRect(), 5, 5);
	painter->drawText(0, 0, node->getClassName());

	QFont f;
	QFontMetrics fm(f);

	paintOutPorts(painter, fm);
	paintInPorts(painter, fm);
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsItem::mousePressEvent(event);
	first_down = event->scenePos();
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsItem::mouseReleaseEvent(event);
	if (event->scenePos() == first_down) {
		Q_EMIT(clicked(this));
	}
}

void NodeItem::updateBoundingRect() {
	QFont f;
	QFontMetrics fm(f);
	QRectF r = QRectF(fm.boundingRect(node->getClassName()));
	int max_outp_width = 0;

	Q_FOREACH(QString out, out_ports) {
		max_outp_width = max(max_outp_width, fm.width(out) + 15);//15 is space between ClassName and outports
	}

	int max_inp_width = 0;
	Q_FOREACH(QString in, in_ports) {
		max_inp_width = max(max_inp_width, fm.width(in) + 15);//15 is space between ClassName and outports
	}

	int fh = fm.height() + 4;
	int height = fh * max(in_ports.size(), out_ports.size()) / 2;

	bounding = r.adjusted(-max(margin, max_inp_width), -max(margin,height),
						  max(margin, max_outp_width), max(margin,height));
}
