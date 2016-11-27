s.boot;


NetAddr.langPort; // Make sure your language port is 57120


(
	OSCdef(\blockPosition, { | msg |
		var touchID = msg[1],
			x = msg[2],
			y = msg[3],
			z = msg[4];

		["Position", touchID, x, y, z ].postln;

	}, '/block/lightpad/0/position');

	OSCdef(\blockOn, { | msg |
		var touchID = msg[1],
			x = msg[2],
			y = msg[3],
			z = msg[4];

		["ON", touchID, x, y, z ].postln;

		// If you want to know which button was pressed, see which area was pressed
		[x.quantize(1,0.5), y.quantize(1,0.5)].postln;

	}, '/block/lightpad/0/on');

	OSCdef(\blockOff, { | msg |
		var touchID = msg[1];

		["OFF", touchID ].postln;

	}, '/block/lightpad/0/off');

	OSCdef(\blockButton, { | msg |
		var buttonPressed = msg[1];

		["BUTTON", buttonPressed ].postln;

	}, '/block/lightpad/0/button');
)


// Adding buttons to the grid (purely visual)
n = NetAddr.new("127.0.0.1", 57140);
n.sendMsg("/block/lightpad/0/addButton", 4, 2, 7); // Add a button at the bottom row at index 2 (middle of the lightpad)

