#include "vibedefaultsimulation.h"
#include <imodel.h>
#include <node.h>
#include <flow.h>
#include <nodeconnection.h>
#include <boost/foreach.hpp>

CD3_DECLARE_SIMULATION_NAME(VIBeDefaultSimulation)

struct SimPriv {
    node_set_type sources;
    node_set_type done;
};

VIBeDefaultSimulation::VIBeDefaultSimulation() : ISimulation() {
    sp = new SimPriv();
}

VIBeDefaultSimulation::~VIBeDefaultSimulation() {
    delete sp;
}

void VIBeDefaultSimulation::start(ptime time) {
    sp->sources= *(model->getNodes());
    ISimulation::start(time);
}

int VIBeDefaultSimulation::run(ptime time, int dt) {
    sp->done.clear();

    con_count_type deps = model->getBackwardCounts();
    int diff = 0;
    int diff2 = 0;
    do {
        diff = sp->done.size();
        BOOST_FOREACH(Node *n,  sp->sources) {
            run(n, time, deps);
        }
        diff2 = sp->done.size();

    } while (diff - sp->done.size() > 0);

    return dt;
}

void VIBeDefaultSimulation::run(Node *n, ptime time, con_count_type &depends) {

    BOOST_FOREACH(NodeConnection *con, model->backwardConnection(n)) {
        if (sp->done.count(con->source) == 0)
            return;
    }
    n->f(time, sim_param.dt);
    sp->done.insert(n);
    BOOST_FOREACH(NodeConnection *con, model->forwardConnection(n)) {
        con->push(0);
        depends[con->sink]--;
    }

    return;
}

void VIBeDefaultSimulation::setModel(IModel *model) {
    ISimulation::setModel(model);
}
