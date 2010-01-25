#ifndef SEWER_H
#define SEWER_H

#include <node.h>
#include <flow.h>
#include <vector>
#include <map>

CD3_DECLARE_NODE(Sewer)
public:
	Sewer();
	virtual ~Sewer();

	bool init(ptime start, ptime end, int dt);
	int f(ptime time, int dt);
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
	std::vector<Flow> V;
	std::map<int, std::pair<double, double> > musk_param;
};

#endif // SEWER_H
