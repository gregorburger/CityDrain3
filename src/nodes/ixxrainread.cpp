#include "ixxrainread.h"
#include "cd3assert.h"
#include "rainbuffer.h"
#include <fstream>

typedef pair<ptime, double> ixx_value;

struct IxxRainReadPrivate {
	ifstream in;
	ixx_value parseLine();
	int getDt();
	time_input_facet *in_facet;
	RainBuffer rain_buf;
	ptime current_time;
};

CD3_DECLARE_NODE_NAME(IxxRainRead)

IxxRainRead::IxxRainRead() {
	data = new IxxRainReadPrivate();
	addParameter(ADD_PARAMETERS(rain_file));
	addOutPort(ADD_PARAMETERS(out));
	data->in_facet = new boost::posix_time::time_input_facet();
	data->in_facet->format("%d.%m.%Y %H:%M:%S");
	data->in.imbue(locale(data->in.getloc(), data->in_facet));
}

IxxRainRead::~IxxRainRead() {
	delete data->in_facet;
}

void IxxRainRead::init(ptime start, ptime end, int dt) {
	(void) end;
	(void) dt;
	data->in.open(rain_file.c_str());
	cd3assert(data->in.good(), "its not possible to read from the given rainfile");
	int raindt = data->getDt();
	ptime first_time;
	data->in >> first_time;
	data->current_time = first_time - seconds(raindt);
	data->in.seekg(0, ios::beg);

	if (first_time < start) {
		while (data->current_time < start) {
			ixx_value v = data->parseLine();
			time_period p(data->current_time, v.first);
			data->rain_buf.put(p.length().total_seconds(), v.second);
		}

		int cut_off = time_period(first_time, start).length().total_seconds();
		Logger(Debug) << (Node*) this << "cutting of" << cut_off << "after ff rainbuffer";
		data->rain_buf.take(cut_off);
		return;
	}

	if (first_time > start) { //fill up with zeroes
		long zerosecs = time_period(data->current_time, start).length().total_seconds();
		Logger(Debug) << "filling up rainbuffer with" << zerosecs << "seconds of zero";
		while (zerosecs > 0) {
			data->rain_buf.put(dt, 0.0);
			zerosecs -= dt;
		}
		return;
	}
}

int IxxRainRead::f(ptime time, int dt) {
	while (data->rain_buf.available() < dt) {
		if (data->in.eof()) {
			data->rain_buf.put(dt, 0.0);
			continue;
		}
		ixx_value v = data->parseLine();
		time_period p(data->current_time, v.first);
		data->rain_buf.put(p.length().total_seconds(), v.second);
	}
	out[0] = data->rain_buf.take(dt);
	return dt;
}

ixx_value IxxRainReadPrivate::parseLine() {
	assert(!in.eof());
	ptime date;
	double value;
	string line;
	in >> date >> value;
	return ixx_value(date, value);
}

int IxxRainReadPrivate::getDt() {
	int pos = in.tellg();
	time_period p(parseLine().first, parseLine().first);
	in.seekg(pos);
	return -p.length().total_seconds();
}
