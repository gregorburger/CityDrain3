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
