var in_flow = Flow()
var out1 = Flow()
var out2 = Flow()

addInPort('in', in_flow)
addOutPort('out1', out1)
addOutPort('out2', out2)

function init(start, stop, dt) {
	//nothing here
	//note behaves pretty static
}

function f(time, dt) {
	out1.copy(in_flow)
	out2.copy(in_flow)
	qhalve = in_flow.getIth(flow, 0) / 2.0
	out1.setIth(flow, 0, qhalve)
	out2.setIth(flow, 0, qhalve)
	return dt
}
