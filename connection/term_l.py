#!/usr/bin/env python

import math
import cairo
import os

size = 25
WIDTH, HEIGHT = 72/25.4*size, 72/25.4*size
scale = 1

def makeLeftTermination(lower):
	string = 'term_l'
	if (lower):
		string += '_l'
	surface = cairo.SVGSurface ('elements/'+string+'.svg', WIDTH, HEIGHT)
	print 'generating elements/'+string
	ctx = cairo.Context (surface)
	ctx.scale (WIDTH, HEIGHT)

	# white background
	ctx.rectangle (0, 0, 1, 1)
	ctx.set_source_rgb (1, 1, 1)
	ctx.fill ()

	delta = 1.0/6

	# +     |
	#      3|
	#      2|
	# O  1  |
	# ------
	point0 = 0, 08.4938/size
	point1 = 5.08/size, 8.4938/size
	point2 = 8.3312/size, 3.75/size*1.5
	point3 = 8.3312/size, 3.75/size*0.5

	# VCC line
	ctx.move_to (delta*4, 0)
	ctx.line_to (delta*4, 0.5-point2[1])
	# if not lower:
	# 	ctx.move_to (delta*4, 0.5-point2[1])
	ctx.line_to (1, 0.5-point2[1])
	if not lower:
		ctx.move_to (delta*4, 0.5-point2[1])
		ctx.line_to (delta*4, 1)

	# GND line
	ctx.move_to (delta*2, 0)
	ctx.line_to (delta*2, 0.5-point3[1])
	# if not lower:
	# 	ctx.move_to (delta*3, 0.5-point3[1])
	ctx.line_to (1, 0.5-point3[1])
	if not lower:
		ctx.move_to (delta*2, 0.5-point3[1])
		ctx.line_to (delta*2, 1)

	# RX line
	ctx.move_to (1, delta*0.5)
	ctx.line_to (delta*5.5, delta*0.5)
	ctx.line_to (delta*5.5, 0)

	# TX line
	ctx.move_to (delta*0.5, 0)
	if not lower:
		ctx.line_to (delta*0.5, 1)
		ctx.move_to (1, 0.5+point3[1])
		ctx.line_to (delta*5.5, 0.5+point3[1])
		ctx.line_to (delta*5.5, 1)
	else:
		ctx.line_to (delta*0.5, 0.5+point3[1])
		ctx.line_to (1, 0.5+point3[1])

	# # SYNC line
	# ctx.move_to (delta*1.5, 0)
	# ctx.line_to (delta*1.5, delta*4.5)
	# if not lower:
	# 	ctx.move_to (delta*1, delta*4.5)
	# ctx.line_to (1, delta*4.5)
	# if not lower:
	# 	ctx.move_to (delta*1.5, delta*4.5)
	# 	ctx.line_to (delta*1.5, 1)

	# # EVENT line
	# ctx.move_to (delta*0.5, 0)
	# ctx.line_to (delta*0.5, delta*5.5)
	# if not lower:
	# 	ctx.move_to (delta*0, delta*5.5)
	# ctx.line_to (1, delta*5.5)
	# if not lower:
	# 	ctx.move_to (delta*0.5, delta*5.5)
	# 	ctx.line_to (delta*0.5, 1)

	ctx.set_source_rgb (0, 0, 0)
	ctx.set_line_width (0.02)
	ctx.set_line_cap (cairo.LINE_CAP_ROUND)
	ctx.set_line_join (cairo.LINE_JOIN_ROUND)
	ctx.stroke ()

	surface.finish ()
	
	# os.system('/Applications/Inkscape.app/Contents/Resources/bin/inkscape -E elements/'+string+'.eps elements/'+string+'.svg')# > /dev/null 2>&1')
	# os.system('pstoedit -dt -f dxf:-polyaslines elements/'+string+'.eps '+'elements/'+string+'.dxf')# > /dev/null 2>&1')
	# os.system('rm intermediate.eps'+' > /dev/null 2>&1')

	# surface.write_to_png('png/'+string+'.png')
