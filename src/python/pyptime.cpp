#include "pyptime.h"
#include <boost/date_time.hpp>
#include <boost/python.hpp>
#include <logger.h>
using namespace boost::python;
using namespace boost::posix_time;
using namespace boost::gregorian;

object to_datetime(ptime &time) {
	object datetime = import("datetime");
	object datetime_datetime = datetime.attr("datetime");
	date d = time.date();
	time_duration dt = time.time_of_day();
	return datetime_datetime((int) d.year(), (int) d.month(), (int) d.day(),
					dt.hours(), dt.minutes(), dt.seconds());
}

void wrap_ptime() {
	class_<ptime>("ptime")
		.def("to_datetime", to_datetime)
		;
}
