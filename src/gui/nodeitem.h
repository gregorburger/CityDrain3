#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsItem>
class Node;
class PortItem;
class SimulationScene;

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

	Node *getNode() const { return node; }
	void connectItems(SimulationScene *scene);

private:
	void moveItems();

	QRectF bounding;
	Node *node;
	qreal margin;

	QList<PortItem *> in_ports, out_ports;

Q_SIGNALS:
	void clicked(NodeItem *source);
};

#endif // NODEITEM_H
