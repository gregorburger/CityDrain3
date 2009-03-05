
function init(start, stop, dt) {
	in_out = new Flow();
	addInPort("in", in_out);
	addOutPort("out", in_out);
}

function f(time, dt) {
	//print("f", time, dt);
	return dt;
}
