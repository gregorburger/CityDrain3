#include "portitem.h"
#include "nodeitem.h"
#include "simulationscene.h"

#include <QFontMetrics>
#include <QPainter>

PortItem::PortItem(QString portName,
				   NodeItem *parent)
	: QGraphicsItem(parent), sink_of(0), source_of(0), node_item(parent), portName(portName), hovering(false) {
	setAcceptHoverEvents(true);
	//setFlag(Ac);
	setZValue(2);
}

PortItem::~PortItem() {

}

void PortItem::connectConnectionSignals(SimulationScene *scene) {
	QObject::connect(this, SIGNAL(mouseDown(PortItem*)), scene, SLOT(connectionStart(PortItem*)));
	QObject::connect(this, SIGNAL(mouseUp(PortItem*)), scene, SLOT(connectionEnd(PortItem*)));
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
	if (hovering) {
		painter->fillRect(boundingRect(), (isConnected() ? Qt::red : Qt::green));
	}
	QFont f;
	QFontMetricsF fm(f);
	painter->drawText(fm.boundingRect(portName), portName);
	painter->drawRect(boundingRect());
}

void PortItem::updateConnection() {
	if (sink_of) {
		QLineF l = sink_of->line();
		l.setP2(scenePos()+boundingRect().center());
		sink_of->setLine(l);
		scene()->update();
	}

	if (source_of) {
		QLineF l = source_of->line();
		l.setP1(scenePos()+boundingRect().center());
		source_of->setLine(l);
		scene()->update();
	}
}
