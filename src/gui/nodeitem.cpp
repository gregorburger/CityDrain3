#include "nodeitem.h"
#include "portitem.h"
#include "connectionitem.h"
#include "nodeparametersdialog.h"
#include "simulationscene.h"
#include "commands/nodemove.h"
#include "commands/changeparameters.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLinearLayout>
#include <boost/foreach.hpp>

#include <node.h>
#include <nodeconnection.h>
#include <simulation.h>
#include <mapbasedmodel.h>
#include <typeconverter.h>

typedef pair<std::string, Flow *> port_pair;

NodeItem::NodeItem(Node* node)
	: QGraphicsItem(), node(node), margin(30) {
	setFlag(ItemIsMovable, true);
	setFlag(ItemIsSelectable, true);
#if QT_VERSION >= QT_VERSION_CHECK(4,6,0)
	setFlag(ItemSendsGeometryChanges, true);
#endif
	setZValue(1);
	updatePorts();
}

PortItem *NodeItem::getInPort(QString id) {
	return in_ports[id.toStdString()];
}

PortItem *NodeItem::getOutPort(QString id) {
	return out_ports[id.toStdString()];
}

void NodeItem::updatePorts() {
	Q_FOREACH(PortItem *i, in_ports.values() + out_ports.values()) {
		delete i;
	}
	in_ports.clear();
	out_ports.clear();

	BOOST_FOREACH(port_pair item, *node->const_in_ports) {
		QString pname = QString::fromStdString(item.first);
		PortItem *pitem = new PortItem(pname, this);
		in_ports[item.first] = pitem;
	}

	BOOST_FOREACH(port_pair item, *node->const_out_ports) {
		QString pname = QString::fromStdString(item.first);
		PortItem *pitem = new PortItem(pname, this);
		out_ports[item.first] = pitem;
	}

	updateBoundingRect();
	moveItems();
	update();
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

QPainterPath NodeItem::shape() const {
	QPainterPath path;
	path.addRoundedRect(boundingRect(), 5, 5);
	return path;
}

void NodeItem::moveItems() {
	double y = boundingRect().top();
	Q_FOREACH(PortItem *pitem, in_ports) {
		QRectF ir = pitem->boundingRect();
		pitem->setPos(bounding.left() - ir.left(), y - ir.top());
		y += pitem->boundingRect().height();
	}

	y = boundingRect().top();
	Q_FOREACH(PortItem *pitem, out_ports) {
		QRectF ir = pitem->boundingRect();
		pitem->setPos(bounding.right() - ir.right(), y - ir.top());
		y += pitem->boundingRect().height();
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
	QString text = getId() + "\n" + node->getClassName();
	QFont f;
	QFontMetrics fm(f);
	painter->drawText(QRectF(fm.boundingRect(text)), Qt::AlignCenter, text);
}

void NodeItem::updateBoundingRect() {
	QFont f;
	QFontMetrics fm(f);
	QRectF r = QRectF(fm.boundingRect(getId() + "\n" + node->getClassName()));
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
	static QPointF old_pos;

	if (change == ItemPositionChange) {
		old_pos = pos();
	}
	if (change == ItemPositionHasChanged) {
		SimulationScene *parentScene = (SimulationScene*) scene();
		if (!parentScene)
			return QGraphicsItem::itemChange(change, value);
		Q_FOREACH(ConnectionItem *con, parentScene->getConnectionsOf(this->getId())) {
			con->updatePositions();
			parentScene->update();
		}
		Q_EMIT(changed(new NodeMove(parentScene, this, old_pos, pos())));
		return QVariant();
	}
	return QGraphicsItem::itemChange(change, value);
}

uint qHash(std::string s) {
	return qHash(QString::fromStdString(s));
}

void NodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
	(void) event;
	changeParameters();
}

bool NodeItem::changeParameters(bool _new) {
	SimulationScene *parentscene = (SimulationScene*) scene();
	QMap<string, PortItem*> in_before = in_ports;
	QMap<string, PortItem*> out_before = out_ports;

	SimulationParameters sp = parentscene->getSimulation()->getSimulationParameters();
	MapBasedModel *model = parentscene->getModel();

	SavedParameters saved;
	if (!_new)
		saved = saveParameters();

	std::string id_before = node->getId();
	while (true) {
		NodeParametersDialog np(getNode());
		if (!np.exec()) {
			restoreParameters(saved);
			return false; //user canceled dialog
		}
		if (!_new)
			getNode()->deinit();

		np.updateNodeParameters();
		if (!node->init(sp.start, sp.stop, sp.dt)) {
			restoreParameters(saved);
			continue; //parameters were not accepted by node
		}

		if (!model->renameNode(node, np.newId().toStdString())) {
			restoreParameters(saved);
			continue; //node id alread used
		}
		break;
	}

	if (!_new)
		parentscene->renameNodeItem(QString::fromStdString(id_before), getId());

	QMap<string, Flow*> in_after(*getNode()->const_in_ports);
	QMap<string, Flow*> out_after(*getNode()->const_out_ports);

	QSet<string> in_removed = in_before.keys().toSet() - in_after.keys().toSet();
	QSet<string> out_removed = out_before.keys().toSet() - out_after.keys().toSet();

	Q_FOREACH(ConnectionItem *citem, parentscene->getConnectionsOf(getId())) {
		NodeConnection *con = citem->getConnection();
		if (in_removed.contains(con->sink_port) || out_removed.contains(con->source_port)) {
			parentscene->remove(citem);
			continue;
		}
	}

	updatePorts();
	parentscene->update();
	if (!_new)
		Q_EMIT(changed(new ChangeParameters(parentscene, this,
											saved, id_before)));
	return true;
}

typedef std::pair<std::string, NodeParameter *> pair_type;
SavedParameters NodeItem::saveParameters() {
	SavedParameters saved;
	BOOST_FOREACH(pair_type p, node->getParameters()) {
		TypeConverter *con = TypeConverter::get(p.second->type);
		saved[p.first] = con->toStringExact(p.second->value);
	}
	return saved;
}

void NodeItem::restoreParameters(SavedParameters p) {
	Q_FOREACH(std::string name, p.keys()) {
		NodeParameter *param = node->getParameters()[name];
		TypeConverter *con = TypeConverter::get(param->type);
		con->setParameterExact(node, name, p[name]);
	}
	updatePorts();
	update();
}
