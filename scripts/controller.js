function controllAfter(time) {
	serialize("/tmp/states")
}

function controllBefore(time) {
	if (time >= 5100) {
		stopSimulation()
	}
}
