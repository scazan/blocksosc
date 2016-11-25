# BlocksOSC
A program to transmit OSC data directly from the ROLI Lightpad Block.
By default, it also turns off the LEDs and only uses them to show touch data while performing.

Currently transmits OSC messages to localhost, port 57120 with the following messages:

```
/block/lightpad/0/on - (fingerIndex, x, y, z)
/block/lightpad/0/off - (fingerIndex)
/block/lightpad/0/position - (fingerIndex, x, y, z)
/block/lightpad/0/button - (1)
```
Very early in development. Support for multiple-modes and addressing the LEDs over OSC is planned.
