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
    //sp->sources = model->getSourceNodes();
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
    if (n->getId().compare("Reporting1") == 0) {
        int h;
        h  = 0;
    }
    if (sp->done.count(n) > 0) {
        return;
    }
    /*  if (n->getId().compare("rain")== 0) {
        int h;
        h  = 0;
    }
    if (n->getId().compare("rainsplitter")== 0) {
        int h;
        h  = 0;
    }*/
     BOOST_FOREACH(NodeConnection *con, model->backwardConnection(n)) {
         if (sp->done.count(con->source) == 0)
                 return;
     }

     if (n->getId().compare("Sewer_Conduit_WWTPConduit_0") == 0) {
         int h;
         h  = 0;
     }
    n->f(time, sim_param.dt);
    sp->done.insert(n);
    BOOST_FOREACH(NodeConnection *con, model->forwardConnection(n)) {
        con->push(0);
        depends[con->sink]--;
    }
    /* BOOST_FOREACH(NodeConnection *con, model->forwardConnection(n)) {
        if (depends[con->sink] > 0) {
            return;
        }
        if (con->sink->getId().compare("rainsplitter")== 0) {
            int h;
            h  = 0;
        }
        run(con->sink, time, depends);
    }*/
    return;
}

void VIBeDefaultSimulation::setModel(IModel *model) {
    ISimulation::setModel(model);
}
