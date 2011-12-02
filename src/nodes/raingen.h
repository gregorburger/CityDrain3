#ifndef RAINGEN_H
#define RAINGEN_H

#include <node.h>
#include <flow.h>


CD3_DECLARE_NODE(RainGen)
public:
	RainGen();
	virtual ~RainGen();
	int f(ptime time, int dt);
	bool init(ptime start, ptime end, int dt);

private:
	//polar-method, see Knuth books
	//return the first, save the second
	//or return the second
	double getNormRandNumbers();
	double a,b,c,d;
	double t_rain, t_dry;
	double i_rain;
	double i_rain_laststep;
	Flow out;
	double norm_rand_nr;
	bool norm_rand_nr_is_good;
};

#endif // RAINGEN_H
