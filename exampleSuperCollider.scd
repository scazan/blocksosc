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
