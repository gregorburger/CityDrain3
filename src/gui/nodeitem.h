#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsItem>
class Node;

class NodeItem : public QGraphicsItem
{
public:
	NodeItem(Node *node);
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
	Node *node;
};

#endif // NODEITEM_H
