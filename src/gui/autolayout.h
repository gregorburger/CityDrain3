#ifndef AUTOLAYOUT_H
#define AUTOLAYOUT_H

#include <QtContainerFwd>
#include <string>

class SimulationScene;
class MapBasedModel;
class Node;

class AutoLayout {
public:
	AutoLayout(SimulationScene *scene);
	void layout();
private:
	QMap<int, QList<QString> > rank();
	void rank_helper(QMap<int, QList<QString> > &ranks,
					 QSet<QString> &ranked,
					 Node *current,
					 int current_rank);

	QList<QString> sort(QList<QString> pred,
						QList<QString> current);

	SimulationScene *scene;
	MapBasedModel *model;

	//tuneables
	int vdist, hdist;
};

#endif // AUTOLAYOUT_H
