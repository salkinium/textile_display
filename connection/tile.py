#!/usr/bin/env python

import math
import cairo

size = 25
WIDTH, HEIGHT = 72/25.4*size, 72/25.4*size

def makeTile(upper, lower):
	string = "tile"
	if (upper):
		string += "_u"
	if (lower):
		string += "_l"
	surface = cairo.SVGSurface ("elements/"+string+".svg", WIDTH, HEIGHT)
	ctx = cairo.Context (surface)
	ctx.scale (WIDTH, HEIGHT)

	# white background
	ctx.rectangle (0, 0, 1, 1)
	ctx.set_source_rgb (1, 1, 1)
	ctx.fill ()

	# +     |
	#      3|
	#      2|
	# O  1  |
	# ------
	point0 = 0, 08.4938/size
	point1 = 5.08/size, 8.4938/size
	point2 = 8.3312/size, 3.75/size*1.5
	point3 = 8.3312/size, 3.75/size*0.5

	delta = 1.0/7

	if not upper:
		# Upper LOOP BACK line
		ctx.move_to (0, delta*0.5)
		ctx.line_to (1, delta*0.5)

	# VCC line
	ctx.move_to (0, delta*1.5)
	ctx.line_to (0.5-point2[0], 0.5-point2[1])
	ctx.move_to (1, delta*1.5)
	ctx.line_to (0.5+point2[0], 0.5-point2[1])

	# GND line
	ctx.move_to (0, delta*2.5)
	ctx.line_to (0.5-point3[0], 0.5-point3[1])
	ctx.move_to (1, delta*2.5)
	ctx.line_to (0.5+point3[0], 0.5-point3[1])

	# RX line
	ctx.move_to (0, delta*3.5)
	ctx.line_to (0.5-point3[0], 0.5+point3[1])
	ctx.line_to (0.55, 0.5+point3[1])
	ctx.line_to (0.7, 0.5+point2[1])
	ctx.line_to (0.5+point2[0], 0.5+point2[1])

	# TX line
	ctx.move_to (1, delta*3.5)
	ctx.line_to (0.5+point3[0], 0.5+point3[1])

	# SYNC line
	ctx.move_to (0, delta*4.5)
	ctx.line_to (0.5-point2[0], 0.5+point2[1])
	ctx.line_to (0.3, 0.5+point2[1])
	ctx.line_to (0.4, delta*4.5)
	ctx.line_to (1, delta*4.5)

	# EVENT line
	ctx.move_to (0, delta*5.5)
	ctx.line_to (0.5-point2[0], 0.5+point1[1])
	ctx.line_to (0.5+point1[0], 0.5+point1[1])
	ctx.line_to (0.5+point2[0], 0.5+point1[1])
	ctx.line_to (1, delta*5.5)

	if (lower):
		# Lower LOOP BACK line
		ctx.move_to (0, delta*6.5)
		ctx.line_to (1, delta*6.5)

	ctx.set_source_rgb (0, 0, 0)
	ctx.set_line_width (0.02)
	ctx.set_line_cap (cairo.LINE_CAP_ROUND)
	ctx.set_line_join (cairo.LINE_JOIN_ROUND)
	ctx.stroke ()

	# surface.write_to_png("png/"+string+".png")
