#!/usr/bin/env python

import math
import cairo
import os

size = 25
WIDTH, HEIGHT = 72/25.4*size, 72/25.4*size

def makeTile(right):
	string = "tile"
	if (right):
		string += "_r"
	surface = cairo.SVGSurface ('elements/'+string+'.svg', WIDTH, HEIGHT)
	print 'generating elements/tile'
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

	delta = 1.0/6

	# VCC line
	ctx.move_to (0, 0.5-point2[1])
	ctx.line_to (0.5-point2[0], 0.5-point2[1])
	if not right:
		ctx.move_to (1, 0.5-point2[1])
		ctx.line_to (0.5+point2[0], 0.5-point2[1])

	# GND line
	ctx.move_to (0, 0.5-point3[1])
	ctx.line_to (0.5-point3[0], 0.5-point3[1])
	if not right:
		ctx.move_to (1, 0.5-point3[1])
		ctx.line_to (0.5+point3[0], 0.5-point3[1])

	# Spacing line for GND signal
	if right:
		ctx.move_to (0.5+point2[0], 0.5-point2[1])
		ctx.line_to (0.5+point2[0], 0.5-point3[1])	

	# Lower LOOP BACK line
	ctx.move_to (0, delta*0.5)
	ctx.line_to (1, delta*0.5)
	if right:
		ctx.line_to (1, 0.5+point3[1])

	# RX line
	ctx.move_to (0, 0.5+point3[1])
	ctx.line_to (0.5-point3[0], 0.5+point3[1])
	ctx.line_to (0.5+point3[0], 0.5+point3[1])

	# TX line
	ctx.move_to (1, 0.5+point3[1])
	ctx.line_to (0.5+point2[0], 0.5+point2[1])

	# Spacing line for RX/TX signal
	ctx.move_to (0.2, 0.7)
	ctx.line_to (0.4, 0.8)

	# # SYNC line
	# ctx.move_to (0, delta*4.5)
	# ctx.line_to (0.5-point2[0], 0.5+point2[1])
	# if not right:
	# 	ctx.line_to (0.42-point2[0], 0.4+point2[1])
	# 	ctx.line_to (0.58-point2[0], 0.4+point2[1])
	# 	ctx.line_to (0.5-point2[0], 0.5+point2[1])
	# 	ctx.line_to (0.5, delta*4.5)
	# 	ctx.line_to (0.5+point2[0], 0.5+point2[1]*1.25)
	# 	ctx.line_to (1, delta*4.5)

	# # EVENT line
	# ctx.move_to (0, delta*5.5)
	# ctx.line_to (0.5, delta*5.5)
	# ctx.line_to (0.5+point1[0], 0.5+point1[1])
	# if not right:
	# 	ctx.line_to (0.42+point1[0], 0.6+point1[1])
	# 	ctx.line_to (0.58+point1[0], 0.6+point1[1])
	# 	ctx.line_to (0.5+point1[0], 0.5+point1[1])
	# 	ctx.line_to (1, delta*5.5)

	ctx.set_source_rgb (0, 0, 0)
	ctx.set_line_width (0.02)
	ctx.set_line_cap (cairo.LINE_CAP_ROUND)
	ctx.set_line_join (cairo.LINE_JOIN_ROUND)
	ctx.stroke ()

	surface.finish ()
	
	# os.system('/Applications/Inkscape.app/Contents/Resources/bin/inkscape -E elements/tile.eps elements/tile.svg')# > /dev/null 2>&1')
	# os.system('pstoedit -dt -f dxf:-polyaslines elements/tile.eps elements/tile.dxf')# > /dev/null 2>&1')

	# surface.write_to_png('png/'+string+'.png')
