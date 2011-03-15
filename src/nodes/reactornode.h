#ifndef REACTOR_H
#define REACTOR_H

#include <node.h>
#include <flow.h>
#include <string>

class Reactor;

CD3_DECLARE_NODE(ReactorNode)
public:
    ReactorNode();
    virtual ~ReactorNode();
    bool init(ptime start, ptime end, int dt);
    int f(ptime time, int dt);

private:
    Flow in, out, volume;
    std::vector<std::string> conc_formula, formula_name;
    std::string constants;
    Reactor *reactor;
    int nc;
};

#endif // RIVER_H
