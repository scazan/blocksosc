# BlocksOSC
A program to transmit OSC data directly from the ROLI Lightpad Block.
By default, it also turns off the LEDs and only uses them to show touch data while performing.

https://scazan.github.io/BlocksOSC/

Currently transmits OSC messages to localhost, port 57120 with the following messages:

```
/block/lightpad/0/on - (fingerIndex, x, y, z)
/block/lightpad/0/off - (fingerIndex)
/block/lightpad/0/position - (fingerIndex, x, y, z)
/block/lightpad/0/button - (1)
```
Receives OSC data on port 57140 in order to add "buttons" to the block:
```
/block/lightpad/0/addButton - (x, y, (int)color)
The x,y coordinates specify a button created on the Lightpad in a 5x5 matrix.
The color is an integer representing:
0 black
1 red
2 orange
3 yellow
4 green
5 cyan
6 blue
7 purple
8 white
```

Early in development in terms of features. Support for multiple-modes is planned.
