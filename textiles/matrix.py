#!/usr/bin/env python

import math
import cairo
import svgutils.transform as sg
import tile, term_l, term_r
import sys

tile.makeTile(False, False)
tile.makeTile(True, False)
tile.makeTile(False, True)
tile.makeTile(True, True)

term_r.makeRightTermination(False, False)
term_r.makeRightTermination(True, False)
term_r.makeRightTermination(False, True)
term_r.makeRightTermination(True, True)

term_l.makeLeftTermination(False, False)
term_l.makeLeftTermination(True, False)
term_l.makeLeftTermination(False, True)
term_l.makeLeftTermination(True, True)

def makeMatrix(colomns, rows):
	if rows == 0:
		rows == 1
	fig = sg.SVGFigure(str(50+25*colomns)+'mm', str(25*rows)+'mm')
	
	for row in range(rows):
		selection = ''
		if row == 0:
			selection += '_u'
		if row == rows-1:
			selection += '_l'

		# Left termination
		fig_term_l = sg.fromfile('svg/term_l'+selection+'.svg')
		plot_term_l = fig_term_l.getroot()
		plot_term_l.moveto(0, 72.0/25.4*25*row)
		fig.append([plot_term_l])

		# right termination
		fig_term_r = sg.fromfile('svg/term_r'+selection+'.svg')
		plot_term_r = fig_term_r.getroot()
		plot_term_r.moveto(72.0/25.4*25*(colomns+1), 72.0/25.4*25*row)
		fig.append([plot_term_r])

		# add the luminet2 tiles
		for colomn in range(colomns):
			fig_tile = sg.fromfile('svg/tile'+selection+'.svg')
			plot_tile = fig_tile.getroot()
			plot_tile.moveto(72.0/25.4*25*(1+colomn), 72.0/25.4*25*row)
			fig.append([plot_tile])

	fig.save('matrix/matrix_'+str(colomns)+'x'+str(rows)+'.svg')

for index in range(8):
	makeMatrix(index, 1)
	makeMatrix(index+1, index+1)
makeMatrix(8, 1)