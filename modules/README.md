Luminet2 Pixel Software
=======================

The sources are compiled using the [XPCC library][xpcc], which is used for
GPIO, Processing, Build System and Serial Debugging.

The fuses are set at 0xef (low), 0xd1 (high), 0x07 (extended).


Color generation
----------------

The color is generated using 8-bit PWM and a [gamma-corrected][gamma] look-up
table stored in flash memory.
This guarantees fast access times and correct color perception, however
generating very dark colors does not yield good results due to limited
PWM value range.


Photosensitive Diode
--------------------

Light intensity is measured using an oversampling and averaging algorithm.
The results are currently not used.


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


Installing XPCC
---------------

XPCC is provided as a git submodule, to use it run this in the root directory:

	$ git submodule init
	$ git submodule update


To install the XPCC build system on OS X (tested on Lion/Mountain Lion):

1.	Install [Homebrew][]:  
	`$ ruby -e "$(curl -fsSL https://raw.github.com/mxcl/homebrew/go)"`
2.	Install some dependencies:  
	`$ brew install scons python`  
	`$ pip install lxml jinja2`
3.	Install the latest precompiled version of [avr-gcc from ObDev][obdev].


To install the XPCC build system on Linux (tested on Ubuntu 12.04):

	$ sudo apt-get update
	$ sudo apt-get install python scons python-jinja2 python-lxml \
	gcc-avr binutils-avr avr-libc avrdude


Extensive information on installing [the XPCC build system can be found here][xpcc-install].


[xpcc]: https://github.com/roboterclubaachen/xpcc
[gamma]: http://en.wikipedia.org/wiki/Gamma_correction
[rpr]: https://raw.github.com/roboterclubaachen/xpcc/feature/resilient_packet_ring/src/xpcc/driver/connectivity/rpr.hpp
[homebrew]: http://mxcl.github.com/homebrew/
[obdev]: http://www.obdev.at/products/crosspack/download.html
[xpcc-install]: http://xpcc.kreatives-chaos.com/install.html
