#ifndef SEWER_H
#define SEWER_H

#include <node.h>
#include <flow.h>


CD3_DECLARE_NODE(Sewer)
public:
	Sewer();
	virtual ~Sewer();

	void init(int start, int end, int dt);
	int f(int time, int dt);
	void deinit();

private:
	void addMuskParam(int dt);
	void setMuskParam(double *C_x, double *C_y, int dt);
private:
	Flow in;
	Flow out;
	int K;
	double X;
	int N;
	Flow **V;
	std::map<int, std::pair<double, double> > musk_param;
};

#endif // SEWER_H
