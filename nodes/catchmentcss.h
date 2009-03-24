#ifndef CATCHMENTCSS_H
#define CATCHMENTCSS_H

#include <node.h>
#include <vector>
class Flow;

CD3_DECLARE_NODE(CatchmentCSS)
public:
	CatchmentCSS();
	virtual ~CatchmentCSS();
	int f(int time, int dt);
	void init(int start, int end, int dt);
	void deinit();
private:
	void addMuskParam(int dt);
	void setMuskParam(double *C_x, double *C_y, int dt);
	void initFlows();

	Flow *rain_in, *dwf_in, *parasite_in, *q_upstream, *out; //ports

	Flow *loss_basin; //states
	std::vector<Flow *> V;
	int area;
	double run_off_coeff;
	double initial_loss;
	double permanent_loss;
	int N;
	double K, X;
	int n_rain_conc;
	std::vector<double *> rain_con_value;
	std::vector<std::string *> rain_con_name;

	std::map<int, std::pair<double, double> > musk_param;
};

#endif // CATCHMENTCSS_H
