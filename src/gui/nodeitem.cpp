/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

#include <typeconverter.h>
#include "nodeitem.h"
#include "portitem.h"
#include "connectionitem.h"
#include "nodeparametersdialog.h"
#include "simulationscene.h"
#include "commands/nodemove.h"
#include "commands/changeparameters.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QTextStream>
#include <boost/foreach.hpp>

#include <node.h>
#include <nodeconnection.h>
#include <simulation.h>
#include <mapbasedmodel.h>

typedef std::pair<std::string, Flow *> port_pair;

NodeItem::NodeItem(Node* node)
	: QGraphicsItem(), node(node), margin(30) {
	QGraphicsDropShadowEffect *ds = new QGraphicsDropShadowEffect(this);
	ds->setBlurRadius(3);
	this->setGraphicsEffect(ds);
	setFlag(ItemIsMovable, true);
	setFlag(ItemIsSelectable, true);
#if QT_VERSION >= QT_VERSION_CHECK(4,6,0)
	setFlag(ItemSendsGeometryChanges, true);
#endif
	setZValue(1);
	updatePorts();
}

NodeItem::~NodeItem() {
	QList<PortItem*> items = in_ports.values() + out_ports.values();
	Q_FOREACH(PortItem *i, items) {
		delete i;
	}
}

PortItem *NodeItem::getInPort(QString id) {
	return in_ports[id.toStdString()];
}

PortItem *NodeItem::getOutPort(QString id) {
	return out_ports[id.toStdString()];
}

void NodeItem::updatePorts() {
	QList<PortItem*> items = in_ports.values() + out_ports.values();
	Q_FOREACH(PortItem *i, items) {
		delete i;
	}
	in_ports.clear();
	out_ports.clear();

	Q_ASSERT(childItems().size() == 0);

	BOOST_FOREACH(port_pair item, *node->const_in_ports) {
		QString pname = QString::fromStdString(item.first);
		PortItem *pitem = new PortItem(pname, this, PortItem::In);
		in_ports[item.first] = pitem;
	}

	BOOST_FOREACH(port_pair item, *node->const_out_ports) {
		QString pname = QString::fromStdString(item.first);
		PortItem *pitem = new PortItem(pname, this, PortItem::Out);
		out_ports[item.first] = pitem;
	}

	updateBoundingRect();
	moveItems();
	//update();
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
	QString text;
	QTextStream ts(&text);
	ts << getId() << endl << node->getClassName();
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
		max_outp_width = std::max(max_outp_width, out->boundingRect().width());
	}

	qreal max_inp_width = 0;
	Q_FOREACH(PortItem *in, in_ports) {
		max_inp_width = std::max(max_inp_width, in->boundingRect().width());
	}

	qreal fh = fm.height() + 4;
	qreal height = fh * std::max(in_ports.size(), out_ports.size()) / 2;

	bounding = r.adjusted(-max(margin, max_inp_width), -max(margin,height),
						  std::max(margin, max_outp_width), std::max(margin,height));
	update();
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value) {
	static QPointF old_pos;

	if (change == ItemPositionChange) {
		old_pos = pos();
	}
	if (change == ItemPositionHasChanged) {
		SimulationScene *parentScene = static_cast<SimulationScene*>(scene());
		if (!parentScene)
			return QGraphicsItem::itemChange(change, value);
		parentScene->updateConnections(this);
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
	if (!node->getParameters().size())
		return true;

	SimulationScene *parentscene = static_cast<SimulationScene*>(scene());
	QMap<std::string, PortItem*> in_before = in_ports;
	QMap<std::string, PortItem*> out_before = out_ports;

	SimulationParameters sp = parentscene->getSimulation()->getSimulationParameters();

	SavedParameters saved;
	if (!_new)
		saved = saveParameters();

	while (true) {
		NodeParametersDialog np(getNode());
		if (!np.exec()) {
			restoreParameters(saved);
			return false; //user canceled dialog
		}
		if (!_new)
			getNode()->deinit();

		np.updateNodeParameters();
		if (node->init(sp.start + seconds(sp.dt), sp.stop, sp.dt)) {
			break;
		}
		restoreParameters(saved);
	}

	QMap<std::string, Flow*> in_after(*getNode()->const_in_ports);
	QMap<std::string, Flow*> out_after(*getNode()->const_out_ports);

	QSet<std::string> in_removed = in_before.keys().toSet() - in_after.keys().toSet();
	QSet<std::string> out_removed = out_before.keys().toSet() - out_after.keys().toSet();

	Q_FOREACH(ConnectionItem *citem, parentscene->getConnectionsOf(getId())) {
		NodeConnection *con = citem->getConnection();

		if (in_removed.contains(con->sink_port) || out_removed.contains(con->source_port)) {
			parentscene->remove(citem);
		}
	}

	updatePorts();
	//parentscene->update();
	if (!_new)
		Q_EMIT(changed(new ChangeParameters(parentscene, this, saved)));
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
	//update();
}
