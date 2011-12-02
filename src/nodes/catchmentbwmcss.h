#ifndef CATCHMENTBWMCSS_H
#define CATCHMENTBWMCSS_H

#include <node.h>
#include <vector>
#include <map>
#include <flow.h>


CD3_DECLARE_NODE(CatchmentBWMCSS)
public:
	CatchmentBWMCSS();
	virtual ~CatchmentBWMCSS();
	int f(ptime time, int dt);
	bool init(ptime start, ptime end, int dt);
	void deinit();
private:
	void addMuskParam(int dt);
	void setMuskParam(double *C_x, double *C_y, int dt);

	Flow rain_in, dwf_in, parasite_in, q_upstream, out; //ports

	Flow loss_basin; //states
	std::vector<Flow *> V;
	double area;
	double run_off_coeff;
	double initial_loss;
	double permanent_loss;
	int N;
        double K, X;
        //Build-up-wash-off
        std::vector<double> mass_init,
							mass_soluted,
                            mass,
                            buildup_a,
                            buildup_b,
                            washoff_k,
                            washoff_w;

	std::map<int, std::pair<double, double> > musk_param;
};

#endif // CATCHMENTBWMCSS_H
