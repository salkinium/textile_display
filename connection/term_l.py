#!/usr/bin/env python

import math
import cairo

width, height = 25, 25
scale = 1

def makeLeftTermination(upper, lower):
	string = "term_l"
	if (upper):
		string += "_u"
	if (lower):
		string += "_l"
	surface = cairo.SVGSurface ("elements/"+string+".svg", 72.0/25.4*width*scale, 72.0/25.4*height*scale)
	ctx = cairo.Context (surface)
	ctx.scale (72.0/25.4*width*scale, 72.0/25.4*height*scale)

	# white background
	ctx.rectangle (0, 0, 1, 1)
	ctx.set_source_rgb (1, 1, 1)
	ctx.fill ()

	deltaX = 1.0/6
	deltaY = 1.0/7

	if (upper):
		# TX line
		ctx.move_to (deltaX*5.5, 0)
	else:
		# Upper LOOP BACK line
		ctx.move_to (1, deltaY*0.5)
	
	ctx.line_to (deltaX*5.5, deltaY*0.5)
	ctx.line_to (deltaX*5.5, deltaY*3.5)
	ctx.line_to (1, deltaY*3.5)

	# VCC line
	ctx.move_to (deltaX*0.5, 0)
	ctx.line_to (deltaX*0.5, deltaY*1.5)
	ctx.line_to (1, deltaY*1.5)
	if not lower:
		ctx.move_to (deltaX*0.5, deltaY*1.5)
		ctx.line_to (deltaX*0.5, 1)

	# GND line
	ctx.move_to (deltaX*1.5, 0)
	ctx.line_to (deltaX*1.5, deltaY*2.5)
	ctx.line_to (1, deltaY*2.5)
	if not lower:
		ctx.move_to (deltaX*1.5, deltaY*2.5)
		ctx.line_to (deltaX*1.5, 1)

	# SYNC line
	ctx.move_to (deltaX*2.5, 0)
	ctx.line_to (deltaX*2.5, deltaY*4.5)
	ctx.line_to (1, deltaY*4.5)
	if not lower:
		ctx.move_to (deltaX*2.5, deltaY*4.5)
		ctx.line_to (deltaX*2.5, 1)

	# EVENT line
	ctx.move_to (deltaX*3.5, 0)
	ctx.line_to (deltaX*3.5, deltaY*5.5)
	ctx.line_to (1, deltaY*5.5)
	if not lower:
		ctx.move_to (deltaX*3.5, deltaY*5.5)
		ctx.line_to (deltaX*3.5, 1)

	# RX line
	ctx.move_to (deltaX*4.5, 0)
	ctx.line_to (deltaX*4.5, deltaY*6.5)
	if not lower:
		ctx.line_to (deltaX*4.5, 1)
	else:
		ctx.line_to (1, deltaY*6.5)

	ctx.set_source_rgb (0, 0, 0)
	ctx.set_line_width (0.02)
	ctx.set_line_cap (cairo.LINE_CAP_ROUND)
	ctx.set_line_join (cairo.LINE_JOIN_ROUND)
	ctx.stroke ()

	# surface.write_to_png("png/"+string+".png")
