#ifndef VIBEMIXER_H
#define VIBEMIXER_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(VIBeMixer)
public:
        VIBeMixer();
	int f(ptime time, int dt);
        virtual ~VIBeMixer(){}
private:
        Flow in_1, in_2, in_3, in_4, in_5, in_6;
	Flow out;

        //double *Q;
};

#endif // VIBEMIXER_H
