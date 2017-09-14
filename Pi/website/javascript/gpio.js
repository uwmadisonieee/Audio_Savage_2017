function toggleGPIO(pin) {
	ws.send("gpio:" + pin);
}