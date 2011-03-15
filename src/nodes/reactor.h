#ifndef REACT_H
#define REACT_H

#include <string>
#include <vector>
#include <flow.h>

namespace mu {
   class Parser;
}

class Reactor
{
public:
    Reactor(Flow &volume);
    virtual ~Reactor();
    bool init(int dt, 
              std::string constants, 
              std::vector<std::string> conc_formula);
    void react(Flow &in, int dt);
    
    std::vector<double> init_v;
    double k, reactor_volume;
    int nstep;
private:
    Flow &volume;
    double acc;
    std::vector<mu::Parser *> parsers;
};

#endif // REACT_H
