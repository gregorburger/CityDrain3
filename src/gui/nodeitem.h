#ifndef NODEITEM_H
#define NODEITEM_H

#include <string>
unsigned int qHash(std::string s);
#include <QGraphicsItem>
class Node;
class PortItem;
class QUndoCommand;

typedef QMap<std::string, std::string> SavedParameters;

class NodeItem : public QObject, public QGraphicsItem
{
Q_OBJECT
Q_INTERFACES(QGraphicsItem)
public:
	NodeItem(Node *node);
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

	void updateBoundingRect();

	QString getId() const;
	QString getClassName() const;

	Node *getNode() const { return node; }
	QMap<std::string, PortItem *> in_ports, out_ports;

	QPainterPath shape() const;
	void updatePorts();

	PortItem *getInPort(QString id);
	PortItem *getOutPort(QString id);
	bool changeParameters(bool _new = false);
	SavedParameters saveParameters();
	void restoreParameters(SavedParameters p);

Q_SIGNALS:
	void changed(QUndoCommand *cmd);

private:
	void moveItems();

	QRectF bounding;
	Node *node;
	qreal margin;
};

#endif // NODEITEM_H
