Luminet2 Textile Display
========================

The Luminet2 Textile Display is a demonstrator for using the 
[Luminet2 Smart Pixel][luminet2] to display content on a flexible surface.
It consists out of an iOS App, many Luminet2 pixels and a textile connection matrix.

The project was done as a collaboration between [The Media Computing Group][i10] and
the [Institut für Textiltechnik][ITA] at RWTH Aachen University.

The main design goals of this project were to:

-	create a high density flexible display using Luminet2 pixels,
-	use a economically viable production technology,
-	design a scalable communication protocol.

The display is then to be integrated into a pillow case which protects the
display and encapsulates the battery and wireless communication adapter.


Design Concept
--------------

We received 80 Luminet2 pixels and decided to limit the display resolution to
8 x 8 pixels for a total of 64 pixels used.
The remaining pixels were used to testing connection techniques and buffer.

For the display to work, these four problems had to be solved:

1.	The pixels need to be connected with each other and placed on a textile
	using an industrial manufacturing technique
3.	A scalable method of communication between the pixels is required.
4.	The display should easily be connected to external data sources.


### Connection matrix

A matrix of flexible enameled copper wire was attached to jeans textile using
a CNC embroilment machine.
The wires have the function of fixing the Luminet2 PCB to the textile as
well as providing it with power and communication abilities.  
The design process is [described here][design].


### Communication

All pixels can exchange information between each other and an external node.  
Internally all nodes are attached to a wired ring bus, which is based on serial
communication and runs at high speed (0.5MBaud).
This bus is simply connected to a WiFly module which enables transparent
communication over WiFi using TCP/IP.  
Further information about [the embedded software][embedded].


### iOS app

An iOS application allow the drawing of custom 8x8 textures as well as
scrolling text in different colors and fonts.  
A UI description can be [found here][ui]


Organisation
------------

This repository is organized as follows:

-	The *connection* folder the python generators for the vector files for the 
	textile connection matrix.
- 	The *module* folder contains the embedded code for the Luminet2 pixels.
-	The *Textplay* folder contains the iOS App to control the display.

The Luminet2 Smart Pixel were designed by [Mariana Bocoi][bocoi].  
The connection matrix and embedded software were designed by [Niklas Hauser][hauser].


[luminet2]: http://hci.rwth-aachen.de/luminet2
[i10]: http://hci.rwth-aachen.de
[ita]: http://www.ita.rwth-aachen.de
[bocoi]: http://hci.rwth-aachen.de/bocoi
[hauser]: http://hci.rwth-aachen.de/hauser
[design]: https://amedeo.informatik.rwth-aachen.de/groups/textiledisplay/wiki/0f695/Textile_Connection_Matrix.html
[embedded]: https://amedeo.informatik.rwth-aachen.de/groups/textiledisplay/wiki/de7d8/Embedded_Software.html
[ui]: https://amedeo.informatik.rwth-aachen.de/groups/textiledisplay/wiki/711c2/Textplay_iOS_App.html