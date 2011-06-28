#include "ixxqread.h"
#include "cd3assert.h"
#include "ixxqbuffer.h"
#include <fstream>
#include <algorithm>
#include <boost/filesystem.hpp>

typedef std::pair<ptime, std::vector<double> > ixx_values;

struct IxxQReadPrivate {
	std::ifstream in;
        ixx_values parseLine();
        int getDt();
		int filedt;
	time_input_facet *in_facet;
		IxxQBuffer q_buf;
	ptime current_time;
};

CD3_DECLARE_NODE_NAME(IxxQRead)

IxxQRead::IxxQRead() {
        data = new IxxQReadPrivate();
	addParameter(ADD_PARAMETERS(in_file));
	addOutPort(ADD_PARAMETERS(out));
	data->in_facet = new boost::posix_time::time_input_facet();
	data->in_facet->format("%d.%m.%Y %H:%M:%S");
	data->in.imbue(std::locale(data->in.getloc(), data->in_facet));
}

IxxQRead::~IxxQRead() {
	delete data->in_facet;
}

bool IxxQRead::init(ptime start, ptime end, int dt) {
	(void) end;
	(void) dt;
	data->in.close();

	if (!boost::filesystem::exists(in_file)) {
		Logger(Error) << "can not open ixx file" << in_file;
		return false;
	}

	data->in.open(in_file.c_str());
	if (!data->in.is_open()) {
		Logger(Warning) << "it is not possible to read from the given file";
		return false;
	}
		data->filedt = data->getDt();
		Logger(Debug) << (Node*) this << "file dt = " << data->filedt;
	ptime first_time;
	data->in >> first_time;
	data->current_time = first_time - seconds(data->filedt);
	data->in.seekg(0, std::ios::beg);

        if (first_time < start) {  //first time = file , start = simulation
		while (data->current_time < start) {
                        ixx_values v = data->parseLine();
			time_period p(data->current_time, v.first);
			data->q_buf.put(p.length().total_seconds(), v.second);
			data->current_time = v.first;
		}

		int cut_off = time_period(first_time, start).length().total_seconds();
		Logger(Debug) << (Node*) this << "cutting of" << cut_off << "after ff buffer";
		data->q_buf.take(cut_off);
		return true;
	}

	if (first_time > start) { //fill up with zeroes
		long zerosecs = time_period(start, data->current_time + seconds(data->filedt)).length().total_seconds();
		Logger(Debug) << "filling up buffer with" << zerosecs << "seconds of zero";
		while (zerosecs > 0) {
						data->q_buf.put(dt, std::vector<double>(1));
			zerosecs -= dt;
		}
		return true;
	}
        return true;
}

void IxxQRead::deinit() {
	data->q_buf.clear();
	data->in.close();
	data->in.clear();
}

int IxxQRead::f(ptime time, int dt) {
	(void) time;
	while (data->q_buf.available() < dt) {
		if (data->in.eof()) {
						data->q_buf.put(dt, std::vector<double>(1));
			continue;
		}
        ixx_values v = data->parseLine();
		time_period p(data->current_time, v.first);
		data->q_buf.put(p.length().total_seconds(), v.second);
		data->current_time = v.first;
        }

		std::vector<double> outflow = data->q_buf.take(dt);
        out.clear();
		for(unsigned int i=0; i<std::min(outflow.size(), Flow::countUnits(Flow::concentration)+1); i++){
				out[i] = outflow[i];
        }
	return dt;
}

ixx_values IxxQReadPrivate::parseLine() {
	assert(!in.eof());
	ptime date;
        std::string line;
        std::vector<double> values;
        in >> date;
        getline(in, line);
        std::istringstream is(line);
        while(is.good()){
                double value;
                is >> value;
                values.push_back(value);
		}
        if(values.empty())
                values.push_back(0);
		if(values.size()>2)
				values.pop_back();
        return ixx_values(date, values);
}

int IxxQReadPrivate::getDt() {
	int pos = in.tellg();
	time_period p(parseLine().first, parseLine().first);
	in.seekg(pos);
	return -p.length().total_seconds();
}
