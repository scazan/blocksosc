# BlocksOSC
A program to transmit OSC data directly from the ROLI Lightpad Block.

Currently transmits OSC messages to localhost, port 57120 with the following messages:

```
/block/touch/on (startX, startY, x, y, z) // Touch occured at startX, startY
/block/touch/off (startX, startY, x, y)// Touch that started at startX, startY has now ended
/block/touch/position (startX, startY, x, y, z) // Current touch position of touch that started at startX, startY
/block/touch/button (1) // The button on the side of the device was pushed
```

It gives startX and startY as an quick and dirty way to have multi-touch IDs.

Very early in development. Support for multi-modes and addressing the LEDs over OSC is planned.
