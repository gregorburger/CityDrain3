flow = new Flow();
print(flow)
addInPort('in', flow)
addOutPort('out', flow)
addParameter('teststring', 'empty')
addParameter('testdouble',  0.0)

function init(start, stop, dt) {
	print(teststring)
	print(testdouble)
	addState('mystate')
	addState('mystringstate')
	mystringstate="da gehts los"
	mystate=3
}

function f(time, dt) {
	mystate += 1000
	mystringstate += " immer weiter "
	return dt;
}
