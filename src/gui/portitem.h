#ifndef PORTITEM_H
#define PORTITEM_H

#include <QGraphicsItem>
#include <QObject>

class NodeItem;
class SimulationScene;

class PortItem : public QObject, public QGraphicsItem {
Q_OBJECT
public:
	PortItem(QString portName,
			 NodeItem *parent);

	void connectConnectionSignals(SimulationScene *scene);
	virtual ~PortItem();

	void paint(QPainter *painter,
			   const QStyleOptionGraphicsItem *option,
			   QWidget *widget);

	QRectF boundingRect() const;

	bool isConnected() {
		return sink_of || source_of;
	}

	void updateConnection();

	QString getPortName() { return portName; }
	NodeItem *getNodeItem() const { return node_item; }

	void setSinkOf(QGraphicsLineItem *connection) {sink_of = connection; }
	void setSourceOf(QGraphicsLineItem *connection) {source_of = connection; }

private:
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event) { hovering = true; update(); }
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { hovering = false; update(); }

	QGraphicsLineItem *sink_of, *source_of;
	NodeItem *node_item;
	QString portName;
	bool hovering;
};

#endif // PORTITEM_H
