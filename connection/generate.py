#!/usr/bin/env python

import math
import cairo
import svgutils.transform as sg
import tile, term_l
import os, sys

tile.makeTile(False)
tile.makeTile(True)

term_l.makeLeftTermination(False)
term_l.makeLeftTermination(True)

def makeMatrix(colomns, rows):
	if rows == 0:
		rows == 1
	fig = sg.SVGFigure(str(50+25*colomns)+'mm', str(25*rows)+'mm')
	
	for row in range(rows):
		selection = ''
		if row == rows-1:
			selection += '_l'

		# Left termination
		fig_term_l = sg.fromfile('elements/term_l'+selection+'.svg')
		plot_term_l = fig_term_l.getroot()
		plot_term_l.moveto(0, 72.0/25.4*25*row)
		fig.append([plot_term_l])

		# add the luminet2 tiles
		for colomn in range(colomns):
			if (colomn == colomns-1):
				fig_tile = sg.fromfile('elements/tile_r.svg')
			else:
				fig_tile = sg.fromfile('elements/tile.svg')
			plot_tile = fig_tile.getroot()
			plot_tile.moveto(72.0/25.4*25*(1+colomn), 72.0/25.4*25*row)
			fig.append([plot_tile])

	name = 'matrix/matrix_'+str(colomns)+'x'+str(rows)
	print 'generating '+name
	fig.save(name+'.svg')
	
	os.system('/Applications/Inkscape.app/Contents/Resources/bin/inkscape -E '+name+'.eps '+name+'.svg')#+' > /dev/null 2>&1')
	os.system('pstoedit -dt -f dxf:-polyaslines -xscale 31.75 -yscale 31.75 '+name+'.eps '+name+'.dxf')#+' > /dev/null 2>&1')
	os.system('rm '+name+'.eps')#+' > /dev/null 2>&1')

for index in range(8):
	makeMatrix(index, 1)
	makeMatrix(index+1, index+1)
makeMatrix(8, 1)