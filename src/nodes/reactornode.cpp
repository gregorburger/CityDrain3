#include "reactornode.h"
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include <QString>
#include <QStringList>
#include "reactor.h"

const char *ReactorNode::class_name = "Reactor"; 
const char *ReactorNode::getClassName() const { return ReactorNode::class_name; }

ReactorNode::ReactorNode() {
    addInPort(ADD_PARAMETERS(in));
    addOutPort(ADD_PARAMETERS(out));

    nc = Flow::countUnits(Flow::concentration);
    conc_formula.resize(nc);
    formula_name.resize(nc);
    
    reactor = new Reactor(volume);
    
    addParameter(ADD_PARAMETERS(constants));
    
    addParameter("reactor_volume", &reactor->reactor_volume).setUnit("m^3");
    addParameter("nstep", &reactor->nstep).setUnit("-");
    addParameter("init_v", &reactor->init_v).setUnit("[-]");

    for (int c = 0; c < nc; ++c) {
        std::string cname = Flow::getUnitNames(Flow::concentration)[c];
        formula_name[c] = cname + "_formula";
        conc_formula[c] = std::string("0");
        addParameter(formula_name[c], &conc_formula[c]);
    }
}


ReactorNode::~ReactorNode() {
    delete reactor;
}

bool ReactorNode::init(ptime start, ptime end, int dt) {
    return reactor->init(dt, constants, conc_formula);
}


int ReactorNode::f(ptime time, int dt) {
    (void)
    reactor->react(in, dt);
    // Ausgabe
    for (int i=0; i < nc; i++){
        out[i+1] = volume[i+1];
    }
    out[0] = in[0];
    
    return dt;
}
