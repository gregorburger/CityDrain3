#include "nodeitem.h"
#include "portitem.h"
#include "nodeparametersdialog.h"
#include "simulationscene.h"

#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLinearLayout>
#include <boost/foreach.hpp>

#include <node.h>
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
	BOOST_FOREACH(port_pair item, *node->const_in_ports) {
		if (in_ports.contains(item.first)) {
			continue;
		}
		QString pname = QString::fromStdString(item.first);
		PortItem *pitem = new PortItem(pname, this);
		in_ports[item.first] = pitem;
	}

	BOOST_FOREACH(port_pair item, *node->const_out_ports) {
		if (out_ports.contains(item.first)) {
			continue;
		}
		QString pname = QString::fromStdString(item.first);
		PortItem *pitem = new PortItem(pname, this);
		out_ports[item.first] = pitem;
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
	if (change == ItemPositionHasChanged) {
		Q_FOREACH(PortItem *pitem, out_ports) {
			pitem->updateConnection();
		}
		Q_FOREACH(PortItem *pitem, in_ports) {
			pitem->updateConnection();
		}
		Q_EMIT(changed(this));
	}
	if (change == ItemSelectedHasChanged) {
		//update();
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

void NodeItem::changeParameters() {
	SimulationScene *parentscene = (SimulationScene*) scene();
	QMap<string, PortItem*> in_before = in_ports;
	QMap<string, PortItem*> out_before = out_ports;

	SimulationParameters sp = parentscene->getSimulation()->getSimulationParameters();
	MapBasedModel *model = parentscene->getModel();

	QMap<std::string, std::string> saved = saveParameters();

	while (true) {
		NodeParametersDialog np(getNode());
		if (!np.exec()) {
			restoreParameters(saved);
			return;
		}
		getNode()->deinit();
		np.updateNodeParameters();
		if (!node->init(sp.start, sp.stop, sp.dt)) {
			restoreParameters(saved);
			continue;
		}

		if (getId() == np.newId()) {
			break;
		}

		if (!model->renameNode(node, np.newId().toStdString())) {
			restoreParameters(saved);
			continue;
		}
		break;
	}

	QMap<string, Flow*> in_after(*getNode()->const_in_ports);
	QMap<string, Flow*> out_after(*getNode()->const_out_ports);

	QSet<string> in_removed = in_before.keys().toSet() - in_after.keys().toSet();

	Q_FOREACH(string s, in_removed) {
		PortItem *pitem = in_before[s];
		if (pitem->getSinkOf()) {
			parentscene->remove(pitem->getSinkOf());
		}
		in_ports.remove(s);
		delete pitem;
	}

	QSet<string> out_removed = out_before.keys().toSet() - out_after.keys().toSet();

	Q_FOREACH(string s, out_removed) {
		PortItem *pitem = out_before[s];
		if (pitem->getSourceOf()) {
			parentscene->remove(pitem->getSourceOf());
		}
		out_ports.remove(s);
		delete pitem;
	}

	updatePorts();
	parentscene->update();
	Q_EMIT(changed(this));
}

typedef std::pair<std::string, NodeParameter *> pair_type;
QMap<std::string, std::string> NodeItem::saveParameters() {
	QMap<std::string, std::string> saved;
	BOOST_FOREACH(pair_type p, node->getParameters()) {
		TypeConverter *con = TypeConverter::get(p.second->type);
		saved[p.first] = con->toStringExact(p.second->value);
	}
	return saved;
}

void NodeItem::restoreParameters(QMap<std::string, std::string> p) {
	Q_FOREACH(std::string name, p.keys()) {
		NodeParameter *param = node->getParameters()[name];
		TypeConverter *con = TypeConverter::get(param->type);
		con->fromStringExact(p[name], param->value);
	}
}
