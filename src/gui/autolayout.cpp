#include "autolayout.h"
#include <QMap>
#include <QDebug>

#include <simulationscene.h>
#include <mapbasedmodel.h>
#include <nodeconnection.h>
#include <node.h>

#include <boost/foreach.hpp>

AutoLayout::AutoLayout(SimulationScene *scene) : scene(scene), model(scene->getModel()) {
	vdist = 40;
	hdist = 130;
}

void AutoLayout::layout() {
	QMap<int, QList<QString> > ranks = rank();

	QMap<QString, NodeItem*> items = scene->getNodeItems();

	int h = 0;

	QList<QString> prev_rank, sorted;

	Q_FOREACH(int rank, ranks.keys()) {
		int v = 0;
		int mnw = 0; //max node width per rank

		if (prev_rank.empty()) {//first round
			prev_rank = ranks[rank];
			sorted = ranks[rank];
		} else {//not the first round
			sorted = sort(prev_rank, ranks[rank]);
			prev_rank = sorted;
		}

		Q_FOREACH(QString node, sorted) {
			NodeItem *item = items[node];

			item->setPos(h, v);

			//update mnw
			int cnw = item->boundingRect().width(); //current node width
			mnw = cnw > mnw ? cnw : mnw;

			v -= item->boundingRect().height() + vdist;
		}

		h += mnw + hdist;
	}

	qDebug() << ranks;
}

void AutoLayout::rank_helper(QMap<int, QList<QString> > &ranks,
							 QSet<QString> &ranked,
							 Node *current,
							 int current_rank) {
	BOOST_FOREACH(NodeConnection *c, model->forwardConnection(current)) {
		QString id = QString::fromStdString(c->sink->getId());
		if (ranked.contains(id))
			continue;
		ranks[current_rank+1].append(id);
		ranked << id;
		rank_helper(ranks, ranked, c->sink, current_rank+1);
	}

	BOOST_FOREACH(NodeConnection *c, model->backwardConnection(current)) {
		QString id = QString::fromStdString(c->source->getId());
		if (ranked.contains(id))
			continue;
		ranks[current_rank-1].append(id);
		ranked << id;
		rank_helper(ranks, ranked, c->source, current_rank-1);
	}
}

QMap<int, QList<QString> > AutoLayout::rank() {
	QMap<int, QList<QString> > ranks;
	QSet<QString> ranked;
	Node *current = *model->getSourceNodes().begin();
	QString id = QString::fromStdString(current->getId());
	ranks[0].append(id);
	ranked << id;
	rank_helper(ranks, ranked, current, 0);
	return ranks;
}

QList<QString> AutoLayout::sort(QList<QString> pred,
								QList<QString> current) {
#if 0
	QList<QString> sorted;
	Q_FOREACH(QString n, pred) {
		QList<QString> per_node;

		Node *node = model->getNode(n.toStdString());
		BOOST_FOREACH(NodeConnection *con, model->forwardConnection(node)) {
			QString sink_node = QString::fromStdString(con->sink->getId());
			if (current.contains(sink_node))
				per_node.append(sink_node);
		}
		sorted.append(per_node);
	}

	return sorted;
#endif
	return current;
}
