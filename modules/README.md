
Luminet2 Pixel Software
=======================

The hardware is powered by the [XPCC library][xpcc], used for its communication
and peripheral modules, written in C++.


RGB LED
-------

The color is generated using 8-bit PWM and a gamma-corrected look-up table,
stored in flash memory.
This guarantees fast access times and correct color perception, however
generating very dark colors does not yield good results, due to limited
PWM value range.


Photosensitive Diode
--------------------

Light intensity is measured using an oversampling and averaging algorithm.
The results are currenlty not used.


Communication
-------------

To transfer the bitmap data into the display, a simplified Resilient Packet Ring
protocol is used over serial.
The data is split into four 48-byte packets, which are sent as multicasts to
16 pixels each.
This allows for a small buffer on each pixel with acceptable overhead.

Each pixel has a unique 15bit address, and belongs to one of the four group
addresses, which are stored in EEPROM.
The bitmap values are sent as 8bit RGB values, and the node then calculated
the offset of its values in the data.

The protocol is currently not yet fully implemented, and still in development
at XPCC, however basic functionality, such as sending and receiving messages
works.

A more detailed description of the frame format [can be found here][rpr].

The pixels communicate directly with a WiFly module in ad-hoc mode, at a
baudrate of 0.5Mbps (500000 bits/s).

[xpcc]: https://github.com/roboterclubaachen/xpcc
[rpr]: https://raw.github.com/roboterclubaachen/xpcc/feature/resilient_packet_ring/src/xpcc/driver/connectivity/rpr.hpp
