s.boot;


NetAddr.langPort; // Make sure your language port is 57120


(
	OSCdef(\blockPosition, { | msg |
		var touchID = msg[1],
			x = msg[2],
			y = msg[3],
			z = msg[4];

		["Position", touchID, x, y, z ].postln;

	}, '/block/lightpad/1/position');

	OSCdef(\blockOn, { | msg |
		var touchID = msg[1],
			x = msg[2],
			y = msg[3],
			z = msg[4];

		["ON", touchID, x, y, z ].postln;

		// If you want to know which button was pressed, see which area was pressed
		[( x*4 ).quantize(1,0.5), ( y*4 ).quantize(1,0.5)].postln;

	}, '/block/lightpad/0/on');

	OSCdef(\blockOff, { | msg |
		var touchID = msg[1],
			x = msg[2],
			y = msg[3],
			z = msg[4];

		["OFF", touchID, x, y, z ].postln;

	}, '/block/lightpad/0/off');

	OSCdef(\blockButton, { | msg |
		var buttonPressed = msg[1];

		["BUTTON", buttonPressed ].postln;

	}, '/block/lightpad/0/button');
)


// Adding buttons to the grid (purely visual)
n = NetAddr.new("127.0.0.1", 57140);
		n.sendMsg("/block/lightpad/0/setButton", 0, 4, 2); // Add a button on the first block at the top left (middle of the lightpad)
(
5.do({ | i |

	5.do({ | k |
		n.sendMsg("/block/lightpad/0/setButton", i, 0, 7); // Add purple buttons on the bottom row (middle of the lightpad)
	});
});
)

n.sendMsg("/block/lightpad/1/setButton", 3, 2, 5); // Add a cyan button on the second block at the middle row at index 3
