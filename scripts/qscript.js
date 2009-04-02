flow = new Flow();
addInPort('in', flow);
addOutPort('out', flow);
addParameter('teststring', 'empty')
addParameter('testdouble',  0.0)

function init(start, stop, dt) {
	print(teststring)
	print(testdouble)
}

function f(time, dt) {
	return dt;
}
