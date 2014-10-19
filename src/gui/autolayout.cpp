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

#include "autolayout.h"


#include <simulationscene.h>
#include <mapbasedmodel.h>
#include <node.h>
#include <connectionitem.h>
#ifdef WITH_AUTOLAYOUT
#include <graphviz/gvc.h>

#define DECL_DPI(d) double dpi = d; char *dpi_str = #d;
#define	TAILX				1
#define HEADX				2
#endif

AutoLayout::AutoLayout(SimulationScene *scene) : scene(scene), model(scene->getModel()) {
}

void AutoLayout::layout() {
#ifdef WITH_AUTOLAYOUT
	int e;
	DECL_DPI(72.0);

	GVC_t *gvc = gvContext();
	Agraph_t *G = agopen("G", AGRAPH);
	e = agsafeset(G, "dpi", dpi_str, "");
	e = agsafeset(G, "rankdir", "LR", "");
	e = agsafeset(G, "nodesep", "0.5", "");
	e = agsafeset(G, "ranksep", "1.0", "");

	QMap<QString, Agnode_t *> nodes;

	Q_FOREACH(NodeItem *n, scene->getNodeItems()) {
		Agnode_t *ag_n = agnode(G, n->getId().toAscii().data());
		QRectF br = n->boundingRect();
		QStringList inports;
		Q_FOREACH(std::string ip, n->in_ports.keys()) {
			inports << QString("<%1> %1").arg(QString::fromStdString(ip));
		}
		QStringList outports;
		Q_FOREACH(std::string op, n->out_ports.keys()) {
			outports << QString("<%1> %1").arg(QString::fromStdString(op));
		}

		QString label = QString("{{%1}|%2|{%3}}").arg(
					inports.join("|"),
					n->getId(),
					outports.join("|")
					);

		agsafeset(ag_n, "label", label.toAscii().data(), "");
		agsafeset(ag_n, "width", QString("%1").arg(br.width()/dpi).toAscii().data(), "");
		agsafeset(ag_n, "height", QString("%1").arg(br.height()/dpi).toAscii().data(), "");
		agsafeset(ag_n, "fixedsize", "true", "");
		agsafeset(ag_n, "shape", "record", "");
		nodes[n->getId()] = ag_n;
	}

	Q_FOREACH(ConnectionItem *e, scene->getConnectionsItems()) {
		Agedge_t *edge = agedge(G, nodes[e->getSourceId()], nodes[e->getSinkId()]);
		agxset(edge, HEADX, e->getSinkPortId().toAscii().data());
		agxset(edge, TAILX, e->getSourcePortId().toAscii().data());
	}

	e = gvLayout(gvc, G, "dot");
	e = gvRender(gvc, G, "dot", NULL);

	QMap<QString, NodeItem *> sim_nodes = scene->getNodeItems();

	QStringList bb = QString(agget(G, "bb")).split(",");
	Q_ASSERT(bb.size() == 4);
	double width = bb[2].toDouble();
	double height = bb[3].toDouble();

	Q_FOREACH(QString node_name, nodes.keys()) {
		Agnode_t *ag_n = nodes[node_name];
		NodeItem *n = sim_nodes[node_name];
		QString pos(agget(ag_n, "pos"));
		QStringList pos_items = pos.split(",");
		Q_ASSERT(pos_items.size() == 2);
		bool ok;
		double x = pos_items[0].toDouble(&ok);
		Q_ASSERT(ok);
		double y = pos_items[1].toDouble(&ok);
		Q_ASSERT(ok);
		QRectF br = n->boundingRect();
		n->setPos(x - br.width()/2.0, height - (y - br.height()/2.0));
		/*QString w(agget(ag_n, "width"));
		QString h(agget(ag_n, "height"));
		qDebug() << w << h;*/
	}

	e = gvFreeLayout(gvc, G);
	agclose(G);
	e = gvFreeContext(gvc);
#endif
}
