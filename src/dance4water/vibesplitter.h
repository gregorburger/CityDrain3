#ifndef SPLITTER_H
#define SPLITTER_H

#include <node.h>
#include <flow.h>
class Node;

CD3_DECLARE_NODE(VIBeSplitter)
public:
        VIBeSplitter();
        virtual ~VIBeSplitter();
	int f(ptime time, int dt);
private:
        Flow * in;
        std::vector<Flow*> out;
        int outports;

};

#endif // SPLITTER_H
