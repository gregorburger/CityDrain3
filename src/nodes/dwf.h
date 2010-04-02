#ifndef DWF_H
#define DWF_H

#include <node.h>
#include <flow.h>
#include <deque>

typedef pair<int, vector<double> > dwf_entry;

CD3_DECLARE_NODE(DWF)
public:
	DWF();
	virtual ~DWF();
	bool init(ptime start, ptime end, int dt);
	void deinit();
	int f(ptime time, int dt);
private:
	bool load(int dt);
	void setOut();
	string dwf_definition_file;
	double q_scale;
	Flow out;
	deque<dwf_entry> dwf, dwf_work;
};

#endif // DWF_H
