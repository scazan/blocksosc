s.boot;


NetAddr.langPort; // Make sure your language port is 57120

(
	OSCdef(\block, { | msg |
		var touchID = msg[1],
			x = msg[2],
			y = msg[3],
			z = msg[4];

		[ touchID, x, y, z ].postln;

	}, '/block/lightpad/0/position');
)

