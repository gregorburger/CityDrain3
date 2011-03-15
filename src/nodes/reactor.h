#ifndef REACTOR_H
#define REACTOR_H

#include <node.h>
#include <flow.h>
#include <string>
#include <map>
#include <muParser.h>

CD3_DECLARE_NODE(Reactor)
public:
    Reactor();
    virtual ~Reactor();
    bool init(ptime start, ptime end, int dt);
    int f(ptime time, int dt);

private:
    Flow in, out, volume;
    std::vector<std::string> conc_formula, formula_name;
    std::vector<mu::Parser *> parsers;
    std::string constants;
    std::vector<double> init_v;
    double dt, k, reactor_volume;
    int nstep;
    double acc;

};

#endif // RIVER_H
