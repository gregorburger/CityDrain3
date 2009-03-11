
function init(start, stop, dt) {
	flow = new Flow();
	addInPort('in', flow);
	addOutPort('out', flow);
}

function f(time, dt) {
	return dt;
}
