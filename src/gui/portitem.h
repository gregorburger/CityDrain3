#ifndef PORTITEM_H
#define PORTITEM_H

#include <QGraphicsItem>
#include <QObject>

class NodeItem;
class SimulationScene;
class ConnectionItem;

class PortItem : public QObject, public QGraphicsItem {
Q_OBJECT
public:
	PortItem(QString portName,
			 NodeItem *parent);

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

	ConnectionItem *getSinkOf() const { return sink_of; }
	void setSinkOf(ConnectionItem *connection) {sink_of = connection; }
	ConnectionItem *getSourceOf() const { return source_of; }
	void setSourceOf(ConnectionItem *connection) {source_of = connection; }

	void setIsAtTop() { at_top = true; }
	void setIsAtBottom() { at_bottom = true; }

private:
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event) { hovering = true; update(); }
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { hovering = false; update(); }

	ConnectionItem *sink_of, *source_of;
	NodeItem *node_item;
	QString portName;
	bool hovering, at_top, at_bottom;
};

#endif // PORTITEM_H
