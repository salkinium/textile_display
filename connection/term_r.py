#!/usr/bin/env python

import math
import cairo

width, height = 25, 25
scale = 1

def makeRightTermination(upper, lower):
	string = "term_r"
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

	delta = 1.0/7

	if not upper:
		# Upper LOOP BACK line
		ctx.move_to (0, delta*0.5)
		ctx.line_to (0.125, delta*0.5)
		ctx.line_to (0.125, 0)

	# RX line
	ctx.move_to (0, delta*3.5)
	ctx.line_to (0.125, delta*3.5)
	if (lower):
		ctx.line_to (0.125, delta*6.5)
		ctx.line_to (0, delta*6.5)
	else:
		ctx.line_to (0.125, 1)

	ctx.set_source_rgb (0, 0, 0)
	ctx.set_line_width (0.02)
	ctx.set_line_cap (cairo.LINE_CAP_ROUND)
	ctx.set_line_join (cairo.LINE_JOIN_ROUND)
	ctx.stroke ()

	# surface.write_to_png("png/"+string+".png")
