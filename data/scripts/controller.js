function controllAfter(time) {
	v0 = getFlow("musk1", "V[0]")
	print(v0.getValue("C1"))
	serialize("/tmp/states")

}

function controllBefore(time) {
	if (time >= 5100) {
		stopSimulation()
	}
	print("before")
}
