#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsItem>
class Node;

class NodeItem : public QObject, public QGraphicsItem
{
Q_OBJECT
public:
	NodeItem(Node *node);
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

	void updateBoundingRect();

	QString getId() const;
	QString getClassName() const;
	QStringList getInPorts() const { return in_ports; }
	QStringList getOutPorts() const { return out_ports; }

	Node *getNode() const { return node; }

private:
	void paintOutPorts(QPainter *painter, QFontMetrics &fm);
	void paintInPorts(QPainter *painter, QFontMetrics &fm);

	QRectF bounding;
	Node *node;
	int margin;

	QStringList in_ports, out_ports;
	QPointF first_down;

Q_SIGNALS:
	void clicked(NodeItem *source);
};

#endif // NODEITEM_H
