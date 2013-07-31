#!/usr/bin/env python 

'''
                    Space Telescope Science Institute


Synopsis:   carlo is a package of python routines intended to
	aid in the interpretation of fitting our python generated
	models 

Notes:
	At present there are two sets of routines, neither of which
	is really finished.  
	
	The first set is intended to help to 
	locate models and allow one to select models that are
	"close" to one another to display.  This portion of the routines
	here are not finished.

	The second set of routines displays an individual model which
	must first have been run through py_wind with the -s option
	in order to produce the data files one needs.  It works, but
	could still be improved.
History:

090116	ksl	Coding begun
090120	ksl	Combined routines that were working in order to have
		a single set of routines for this general topic

'''

import sys
import os
import glob
import pylab
import numpy
import math

from matplotlib.mlab import griddata

# These are the global variables
python_version='unknown'
current_inclination=0.0



def get_filenames(dirname='.',descriptor='*.pf'):
	'''
	get_filename(dirname,descriptor) locates and returns all 
	the filenames of the type specified by descriptor
	'''
	searchlist=dirname+'/'+descriptor
	names=glob.glob(searchlist)
	return names

def split_filename(name='test/test/foo.dat'):
	'''
	split a filene into a path, filename, a rootname, and an extension.

	In short get every possible thing one could want to know about a filename
	(except of course whether it exists)


	'''

	try:
		i=name.rindex('/')
		dirname=name[0:i+1]
		name=name[i+1:len(name)]
	except ValueError:
		dirname='./'
	
	try:
		i=name.rindex('.')
		root=name[0:i]
		ext=name[i:len(name)]
	except ValueError:
		root=name
		ext=''
	return dirname,name,root,ext
	



def read_pf(filename='test'):
	'''
	read and parse a parameter file.  Note that
	this routine can handle the extension or not,
	but it will always try to read the .pf file
	'''

	# Check whether .pf has been added
	try:
		n=filename.rindex('.')
		name=filename[0:n]+'.pf'
	except ValueError:
		name=filename+'.pf'

	x=[]


	f=open(name,'r')
	lines=f.readlines()
	f.close()
	i=0
	while i<len(lines):
		q=lines[i].strip()
		if q[0]!='#' and len(q)>1:
			q=q.split()
			words=q[0].split('(')
			try:
				value=eval(q[1])
			except NameError:
				value=q[1]
		x=x+[[words[0],value]]
		i=i+1
	return x

def make_table(filelist):
	'''
	Make the main table containing all of the pf files in a list. 
	
	filelist should be a list of names, e.g ['test1', 'test2' of
	parameter file names.  This routine reads all of the parameter
	files in a list and returns them as a single list
	'''
	table=[]
	i=0
	while i<len(filelist):
		print filelist[i]
		table=table+[read_pf(filelist[i])]
		i=i+1
	return table

def get_choices(table):
	'''
	get_choices determine what variables are changed in a set of parameter files
	and values of the variables.

	Note: There is no guarantee that all possible combinations exist,
	since typically a complete grid would contain some non-physical
	choices, such as a mass lost rate exceeding the disk accretion 
	rate
	'''

	unique=[]
	
	i=0
	while i<len(table[0]):
		j=0
		values=[]
		while j<len(table):
			record=table[j]
			values=values+[record[i][1]]
			j=j+1
		values.sort()
		v=[record[i][0],values[0]]
		j=1
		while j<len(values):
			if v[len(v)-1]!=values[j]:
				v=v+[values[j]]
			j=j+1
		if len(v)>2:
			unique=unique+[v]

		i=i+1
	
	return unique

def compare_pf(pf1,pf2):
	'''
	Compare two parameter files to identify which variables are different in them.  A summary
	is printed out, and a list containing the differences is returned.

	This will likely fail if the variables that are different are strings
	'''
	filelist=[pf1,pf2] # It does not matter here whether the extensions are attached or not
	table=make_table(filelist)
	unique=get_choices(table)  # In this case is should be true that the first value corresponds to the first file

	i=0
	print '\n Summary: \n'
	print 'file1:  %40s' % pf1
	print 'file2:  %40s\n' % pf2
	while i<len(unique):
		print '%30s %8.2g %8.2g' % (unique[i][0],unique[i][1],unique[i][2])
		i=i+1
	return unique

def compare_pfs(dirname='.',descriptor='*.pf'):
	'''
	Compare many pfs in a directory and summarize what variables are
	changing.  This is the routine that is used to summarize the various
	grids on my web pages.

	This is almost identical to compare_pf, and they should probably
	be combined
	'''
	filelist=get_filenames(dirname,descriptor)
	table=make_table(filelist)
	unique=get_choices(table)


	i=0
	print '\n Summary of variables that are changing in the various pf files: \n'
	while i<len(unique):
		printstring='%30s ' %  (unique[i][0])
		j=1
		while j<len(unique[i]):
			try:
				printstring=printstring+' %8.2g' % (unique[i][j])
			except TypeError:
				print 'Error: compare_pfs: problem with unique[%d][%d]' % (i,j)
				print 'line was: ',unique[i]
			j=j+1
		print printstring
		i=i+1
	return unique



# Next set of routines are for producing plots showing aspects of
# specific models

def read_py_spec(filename,angle=50.0):
	'''
	read_py_spec(filename,angle=50.0)

	read a spectrum file produced by python.  At present
	the routine expects the full name of the file.  The
	routine finds the spectrum that was produced with an
	inclination that is closest to the one desired.  It
	does not try to interpolate, since it's not obvious
	that is a good thing to do.  The order of the spectra
	are flipped after reading them, so that they are in
	ascending order in wavelength
	'''
	try:
		f=open(filename,'r')
		print 'Reading %s' % filename
	except IOError :
		print "This file %s does not exist" % filename
		return IOError
	
	#Get the header information


	'''
	There are two slightly different versions of files
	that have been produced by python.  One has the
	line with the angles starting with #Freq. and the 
	other with # Freq.  This affects both locating the
	line with the Angles and the identification of
	the column associated with an angle.
	'''

	global python_version
	global current_inclination

	line=f.readline()
	z=line.split()
	while z != 'EOF':
		if len(z)>1 and z[2]=='Version':
			python_version=z[3]
			print 'The version of the spectrum file was ',python_version
		if z[0]=='#Freq.':
			filetype='original'
			break
		if len(z)>1 and  z[1]=='Freq.':
			filetype='new'
			break
		line=f.readline()
		z=line.split()
	
	diff=1.e30
	i=0
	while i<len(z):
		q=z[i].split(':') # Separate Ang from the value
		if q[0]=='Ang':
			ang=float(q[1])
			xdiff=math.fabs(angle-ang)
			# print xdiff
			if xdiff<diff:
				best=ang
				diff=xdiff
				ncol=i
				if filetype=='new':
					ncol=i-1
				# print 'xxx', best,ncol,len(z)
		i=i+1
	
	if diff>1.0:
		print 'The closest angle was %f which was %f from desired value of %f\n' % (best,diff,angle) 

	current_inclination=best

	# print best,diff,angle,ncol
	
	wave=[]
	flux=[]
	line=f.readline()
	z=line.split()
	while line!='EOF' and ncol<len(z):
		wave=wave+[float(z[1])]
		flux=flux+[float(z[ncol])]
		line=f.readline()
		z=line.split()
	f.close()

	# Now boxcar smooth the image 

	half_width = 0.5   # Half the size of the bocar in Ang

	# print 'test ', len(wave)
	dw=(wave[0]-wave[len(wave)-1])/len(wave)

	n=int(half_width/dw)
	ntot=2*n+1 # This assures n is odd

	
	# Create the boxcar
	boxcar=1./ntot*numpy.ones(ntot,'d')

	# Convolve f with the boxcar (which produces an oversized array)
	ff=numpy.convolve(flux,boxcar)

	# Strip off the excess bits
	fff=ff[n:-n]

	wave=numpy.array(wave)


	flux=[]
	i=0
	while i<len(wave):
		flux=flux+[fff[i]]
		i=i+1

	wave=numpy.array(wave)
	flux=numpy.array(flux)

	if wave[2]<wave[1]:
		ilen=len(wave)

		wave.reshape(ilen,1)
		wave=numpy.flipud(wave)
		wave.reshape(ilen)

		flux.reshape(ilen,1)
		flux=numpy.flipud(flux)
		flux.reshape(ilen)




	return wave,flux





def read_py_dat(filename,create='no'):
	'''

	read_py_dat(filename,create='no')

	Read an output ascii file produced by py_wind, e.g the 
	densities of OVI in the grid

	if create is set to something other than 'no' try to create
	the files, on the assumption that this is a standard file
	that py_wind creates.
	'''
	x=[]
	y=[]
	z=[]

	if os.path.isfile(filename)==False:
		if create=='no':
			print 'Error: read_py_dat: %s does not exist' % filename
			return x,y,z
		else:
			xname=filename[0:filename.rindex('.dat')]
			print 'Running py_wind %s',xname
			py_wind(xname,ver='none')

	

	try:
		f=open(filename,'r')
	except IOError :
		print "This file %s does not exist" % filename
		raise
		return x,y,z
	line=f.readline()
	line=f.readline()
	line=f.readline()
	line=f.readline()
	while line!='':
		q=line.split()
		x=x+[float(q[0])]
		y=y+[float(q[1])]
		z=z+[float(q[2])]
		line=f.readline()
	f.close()
	return x,y,z

def read_spectrum(filename,errors='yes'):
	'''
	Read a standard spectrum with wavelengths, fluxes [and errors] in the first two (3)
	columns.  Return wave,flux,err as 1 1d lists.  This is what one would use to read
	a typical data spectrum
	'''
	try:
		f=open(filename,'r')
	except IOError :
		print "This file %s does not exist" % filename
		sys.exit(0)
	wave=[]
	flux=[]
	err=[]

	ncols=3
	if errors != 'yes':
		ncols=2

	line=f.readline()
	while line!='':
		z=line.split()
		if len(z)>=ncols and z[0][0]=='#':
			wave=wave+[float(z[0])]
			flux=flux+[float(z[1])]
			if ncols == 3:
				err=err+[float(z[2])]
		line=f.readline()
	print 'read_spectrum: Read %s with %d wavelengths' % (filename,len(wave))
	return wave,flux,err





def plot_spec(model='sscyg.spec',wmin=900,wmax=1700,angle=50.,data='none'):
	'''
	plot_spec(model='sscyg.spec',wmin=900,wmax=1700,angle=50.,data='none'

	Plot a spectrum from a model produced by python.  Usage should be obvious.  
	
	At present data is not used.  The intent was to allow one to plot data
	against the model.
	'''

	pylab.clf()
	wave,flux=read_py_spec(model,angle)

	print 'return',len(wave),len(flux)

	pylab.plot(wave,flux)
	pylab.xlim(wmin,wmax)
	pylab.draw()



def get_one_plane(xi,yi,filename,zmin=-20):
	'''
	get_one_plane(xi,yi,filename,zmin)

	This routine reads a .dat file produced by
	py_wind and converts it to a logarithm

	zmin is the min value allowed for the algorithm
	and should be set to something reasonable since
	it affects how the gridding works.

	090303 - Updated so that this version does not
		rescale anything.  This means that for
		rgb images one needs to rescale somewhere
		else.  The rationale for this is that one
		actually wants to rescale based on the three
		frames that go into to makeing the image
		and not a single frame.

	'''
	try:
		x,y,z=read_py_dat(filename,'yes')
	except IOError:
		print 'Error: get_one_plane ',filename
		raise
		return 

	z=numpy.array(z)
	xmin=numpy.min(z)
	xmax=numpy.max(z)
	print 'The mininum and maximum values of the plane were',xmin,xmax
	print 'The log of these are ',numpy.log10(xmin),numpy.log10(xmax)

	z=numpy.log10(z)
	z=numpy.maximum(z,zmin)

	values=griddata(x,y,z,xi,yi)

	values=numpy.array(values)
	values.shape=(len(xi)*len(yi))
	x=numpy.max(values)
	xmin=numpy.min(values)


	print 'The minimum and maximum were ',xmin,x
	# values=values/x

	return values




def xget_one_plane(xi,yi,filename,delta=4):
	'''
	xget_one_plane(xi,yi,filename,zmin)

	This routine reads a .dat file produced by
	py_wind and converts it to a logarithm

	This version, tries to produce an array whose
	maximmum the maximum of the .dat file and
	whose range is determined by delta



	'''
	try:
		x,y,z=read_py_dat(filename,'yes')
	except IOError:
		print 'Error: get_one_plane ',filename
		raise
		return 

	z=numpy.array(z)
	xmin=numpy.min(z)
	xmax=numpy.max(z)

	xdelta=xmax/math.pow(10.,delta)


	print '\nxget_one_plane : ',filename,delta
	print 'xget_one_plane: The mininum (allowed) and maximum values of the plane were %g (%g) %g' % (xmin,xdelta,xmax)
	print 'The log of these are %g (%g) %g ' % (numpy.log10(xmin),numpy.log10(xdelta),numpy.log10(xmax))

	xmin=numpy.maximum(xmin,xdelta)
	z=numpy.maximum(z,xmin)

	print 'The new min and max in the array are %g %g before taking log' % (numpy.min(z),numpy.max(z))


	z=numpy.log10(z)
	xmax=numpy.max(z)

	# limit the dynamic range of the plot
	# z=numpy.maximum(z,xmax-delta)

	values=griddata(x,y,z,xi,yi)

	values=numpy.array(values)
	values.shape=(len(xi)*len(yi))

	xmax=numpy.max(values)
	xmin=numpy.min(values)


	print 'The minimum and maximum in the gridded data were  %g %g\n' % (xmin,xmax)
	# values=values/x

	return values


def plot_one_plane(filename,wsize=1e10,delta=4):
	'''
	plot_one_plane is intended to allow
	one to plot any ascii output file
	from py_wind
	'''
	xi=numpy.linspace(0.0,wsize,100)
	yi=numpy.linspace(0.0,wsize,100)

	print 'plot_one_plane ',wsize,delta

	xy=xget_one_plane(xi,yi,filename,delta)
	xy.shape=(100,100)
	# pylab.imshow(xy)
	pylab.imshow(xy,extent=(0,wsize/1e9,0,wsize/1e9),interpolation=None)
	pylab.draw()

def xone_ion(rootname='sscyg',ion='O6',wsize=1e10):
	'''
	one_ion(rootname='sscyg',ion='O6',angle=50,wsize=1e10)
	'''

	# The only point of next line is to get the version number
	wave,flux=read_py_spec(rootname+'.spec')



	ne_name=rootname+'.ne.dat'
	te_name=rootname+'.te.dat'
	tr_name=rootname+'.tr.dat'


	scatter_name=rootname+'.ions'+ion+'.dat'
	absorp_name=rootname+'.iona'+ion+'.dat'
	concen_name=rootname+'.ionc'+ion+'.dat'


	pylab.figure(3,figsize=(12,8))
	pylab.clf()

	pylab.subplot(231)

	plot_one_plane(ne_name,wsize,delta=4)
	pylab.title(r'n$_{e}$')

	pylab.subplot(232)
	plot_one_plane(te_name,wsize,delta=4)
	pylab.title(r'T$_{e}$')

	pylab.subplot(233)
	plot_one_plane(tr_name,wsize,delta=4)
	pylab.title(r'T$_{r}$')

	pylab.subplot(234)
	plot_one_plane(concen_name,wsize,delta=4)
	pylab.title('Ion Density')

	pylab.subplot(235)
	plot_one_plane(scatter_name,wsize,delta=4)
	pylab.title('Absorp')

	pylab.subplot(236)
	plot_one_plane(absorp_name,wsize,delta=4)
	pylab.title('Scat.')

	pylab.figtext(0.07,0.5,r'z ($10^{9} cm$)',verticalalignment='center',rotation='vertical')
	pylab.figtext(0.5, 0.02,r'x($10^{9} cm$)',horizontalalignment='center')

	pylab.draw()
	pylab.savefig(rootname+'_'+ion+'.jpg')

def overview(rootname='sscyg',wsize=1e10):
	'''
	overview(rootname='sscyg',wsize=1e10)
	'''

	# The only point of next line is to get the version number
	wave,flux=read_py_spec(rootname+'.spec')



	ne_name=rootname+'.ne.dat'
	te_name=rootname+'.te.dat'
	tr_name=rootname+'.tr.dat'

	vel_name=rootname+'.vel.dat'
	vel_name=rootname+'.vrho.dat'

	vz_name=rootname+'.vz.dat'
	vtheta_name=rootname+'.vtheta.dat'


	pylab.figure(3,figsize=(12,8))
	pylab.clf()

	pylab.subplot(231)

	plot_one_plane(ne_name,wsize,delta=4)
	pylab.title(r'n$_{e}$')

	pylab.subplot(232)
	plot_one_plane(te_name,wsize,delta=1)
	pylab.title(r'T$_{e}$')

	pylab.subplot(233)
	plot_one_plane(tr_name,wsize,delta=1)
	pylab.title(r'T$_{r}$')

	pylab.subplot(234)
	plot_one_plane(vel_name,wsize,delta=3)
	pylab.title(r'V$_{x}$')

	pylab.subplot(235)
	plot_one_plane(vtheta_name,wsize,delta=3)
	pylab.title(r'V$_{y}$')

	pylab.subplot(236)
	plot_one_plane(vz_name,wsize,delta=3)
	pylab.title(r'V$_{z}$')

	pylab.figtext(0.03,0.5,r'z ($10^{9} cm$)',verticalalignment='center',rotation='vertical')
	pylab.figtext(0.5, 0.02,r'x($10^{9} cm$)',horizontalalignment='center')

	pylab.draw()
	pylab.savefig(rootname+'_overview.jpg')

def plot_line(rootname,ion='O6',angle=60,wave_min=1000,wave_max=1050,wsize=1e11):
	'''
	The idea of this plot is to capture the scattering and the absorption
	of a line
	'''
	from matplotlib.ticker  import MultipleLocator, FormatStrFormatter

	pylab.figure(2,figsize=(12,12))
	pylab.clf()
	pylab.title(ion)

	wave,flux=read_py_spec(rootname+'.spec',angle)

	scatter_name=rootname+'.ions'+ion+'.dat'
	absorp_name=rootname+'.iona'+ion+'.dat'
	concen_name=rootname+'.ionc'+ion+'.dat'



	ax=pylab.subplot(221)
	pylab.plot(wave,flux,'r')
	pylab.xlim(wave_min,wave_max)
	zzz=get_median(wave,flux,wave_min,wave_max)
	pylab.ylim(0,2.*zzz)

	if current_inclination>0:
		string='i=%d' % current_inclination
		pylab.text(0.25*wave_min+0.75*wave_max,0.2*zzz,string)

	majorLocator=MultipleLocator(30)
	ax.xaxis.set_major_locator(majorLocator)



	pylab.subplot(222)
	plot_one_plane(concen_name,wsize,delta=4)
	pylab.title('Ion Density')

	pylab.subplot(223)
	plot_one_plane(absorp_name,wsize,delta=4)
	pylab.title('Absorp')

	pylab.subplot(224)
	plot_one_plane(scatter_name,wsize,delta=1)
	pylab.title('Scat.')

	pylab.figtext(0.07,0.5,r'z ($10^{9} cm$)',verticalalignment='center',rotation='vertical')
	pylab.figtext(0.5, 0.02,r'x($10^{9} cm$)',horizontalalignment='center')

	pylab.draw()
	pylab.savefig(ion+'.jpg')


def cno(rootname='sscyg',angle=50,wsize=1e10):
	'''

	Usage: cno(rootname='sscyg',angle=50,wsize)

	where 
		rootname is rootname for a model that has been run
		ang      is the inclination angle of the observer
		wsize    is the x/y sized of portion of the wind to
		            be displayed
		  

	Note that the rootname could include a directory path.
	'''
	from matplotlib.ticker  import MultipleLocator, FormatStrFormatter


	# Get the spectrum.  The reason for getting the spectrum is
	# to establish what version of python was used to create the
	# spectrum

	wave,flux=read_py_spec(rootname+'.spec',angle)

	xi=numpy.linspace(0.0,wsize,100)
	yi=numpy.linspace(0.0,wsize,100)

	# Get carbon concentrations
	car3=get_one_plane(xi,yi,rootname+'.ioncC3.dat',zmin=0)
	car4=get_one_plane(xi,yi,rootname+'.ioncC4.dat',zmin=0)
	car5=get_one_plane(xi,yi,rootname+'.ioncC5.dat',zmin=0)

	print 'length c3',len(car3)


	i=0
	carbon=[]
	while i<len(car3):
		carbon=carbon+[car3[i]]+[car4[i]]+[car5[i]]
		i=i+1
	carbon=numpy.array(carbon)
	carbon.shape=(100,100,3)
	print 'Carbon rgb',carbon.min(),carbon.max()
	carbon=numpy.maximum(carbon,0)
	print 'Carbon rgb',carbon.min(),carbon.max()
	carbon=carbon/carbon.max()
	print 'Carbon   rgb',carbon.min(),carbon.max()



	# Get nitrogen
	nit4=get_one_plane(xi,yi,rootname+'.ioncN4.dat',zmin=0)
	nit5=get_one_plane(xi,yi,rootname+'.ioncN5.dat',zmin=0)
	nit6=get_one_plane(xi,yi,rootname+'.ioncN6.dat',zmin=0)
	i=0
	nitrogen=[]
	while i<len(nit4):
		nitrogen=nitrogen+[nit4[i]]+[nit5[i]]+[nit6[i]]
		i=i+1
	nitrogen=numpy.array(nitrogen)
	nitrogen.shape=(100,100,3)

	print 'nitrogen rgb',nitrogen.min(),nitrogen.max()
	nitrogen=numpy.maximum(nitrogen,0)
	print 'nitrogen rgb',nitrogen.min(),nitrogen.max()
	nitrogen=nitrogen/nitrogen.max()
	print 'nitrogen rgb',nitrogen.min(),nitrogen.max()


	# Get oxygen
	# oxy4=get_one_plane(xi,yi,rootname+'.ioncO4.dat')
	oxy5=get_one_plane(xi,yi,rootname+'.ioncO5.dat',zmin=0)
	oxy6=get_one_plane(xi,yi,rootname+'.ioncO6.dat',zmin=0)
	oxy7=get_one_plane(xi,yi,rootname+'.ioncO7.dat',zmin=0)
	i=0
	oxygen=[]
	while i<len(nit4):
		oxygen=oxygen+[oxy5[i]]+[oxy6[i]]+[oxy7[i]]
		i=i+1
	oxygen=numpy.array(oxygen)
	oxygen.shape=(100,100,3)

	print 'oxygen   rgb',oxygen.min(),oxygen.max()
	oxygen=numpy.maximum(oxygen,0)
	print 'oxygen   rgb',oxygen.min(),oxygen.max()
	oxygen=oxygen/oxygen.max()
	print 'oxygen   rgb',oxygen.min(),oxygen.max()



	# Make the panels containging the spectrum 
	pylab.figure(1,figsize=(12,8))
	pylab.clf()

	pylab.subplot(231)
	pylab.imshow(oxygen,cmap=None,extent=(0,wsize/1e9,0,wsize/1e9),interpolation=None)
	pylab.title('Oxygen V-VII')
	pylab.subplot(232)
	pylab.imshow(nitrogen,cmap=None,extent=(0,wsize/1e9,0,wsize/1e9),interpolation=None)
	pylab.title('Nitrogen IV-VI')
	pylab.subplot(233)
	pylab.title('Carbon III-V')
	pylab.imshow(carbon,cmap=None,extent=(0,wsize/1e9,0,wsize/1e9),interpolation=None)



	# Now plot the spectra for each band

	wcenter=1033
	width=25
	ax=pylab.subplot(234)
	pylab.plot(wave,flux,'r')
	pylab.xlim(wcenter-width,wcenter+width)
	zzz=get_median(wave,flux,wcenter-width,wcenter+width)
	pylab.ylim(0,2.*zzz)

	if current_inclination>0:
		string='i=%d' % current_inclination
		pylab.text(wcenter+0.5*width,0.2*zzz,string)

	majorLocator=MultipleLocator(30)
	ax.xaxis.set_major_locator(majorLocator)


	bx=pylab.subplot(235)
	pylab.plot(wave,flux,'r')

	wcenter=1238
	pylab.xlim(wcenter-width,wcenter+width)
	zzz=get_median(wave,flux,wcenter-width,wcenter+width)
	pylab.ylim(0,2.*zzz)
	if current_inclination>0:
		string='i=%d' % current_inclination
		pylab.text(wcenter+0.5*width,0.2*zzz,string)


	bx_majorLocator=MultipleLocator(30)
	bx.xaxis.set_major_locator(bx_majorLocator)

	cx=pylab.subplot(236)
	cx_majorLocator=MultipleLocator(30)
	cx.xaxis.set_major_locator(cx_majorLocator)
	pylab.plot(wave,flux,'r')
	wcenter=1550
	pylab.xlim(wcenter-width,wcenter+width)
	zzz=get_median(wave,flux,wcenter-width,wcenter+width)
	pylab.ylim(0,2.*zzz)
	if current_inclination>0:
		string='i=%d' % current_inclination
		pylab.text(wcenter+0.5*width,0.2*zzz,string)



	# Now plot the global labels for the plot 
	pylab.figtext(0.07,0.30,'Flux',verticalalignment='center',rotation='vertical')
	pylab.figtext(0.07,0.70,r'z ($10^{9} cm$)',verticalalignment='center',rotation='vertical')
	pylab.figtext(0.5, 0.480,r'x($10^{9} cm$)',horizontalalignment='center')

	pylab.figtext(0.5, 0.025,r'Wavelength ($\AA$) ',horizontalalignment='center')



	pylab.draw()
	pylab.savefig(rootname+'_cno.jpg')

def one_ion(rootname='sscyg',ion='C4',wsize=1e10):
	'''

	Usage: one_ion(rootname='sscyg',ion='C4',wsize=1e10

	where 
		rootname is rootname for a model that has been run
		ion      is the ion name as part of the name in the
			filename produced by py_wind
		wsize    is the x/y sized of portion of the wind to
		            be displayed
		  
	produces a 4 panel dispaly of this ion only, showing 

	'''
	from matplotlib.ticker  import MultipleLocator, FormatStrFormatter


	xi=numpy.linspace(0.0,wsize,100)
	yi=numpy.linspace(0.0,wsize,100)

	# Get ion    
	try:
		den=get_one_plane(xi,yi,rootname+'.ionc'+ion+'.dat',zmin=0.0)
		frac=get_one_plane(xi,yi,rootname+'.ion'+ion+'.dat',zmin=-5)
		scat=get_one_plane(xi,yi,rootname+'.ions'+ion+'.dat',zmin=-1)
		abs=get_one_plane(xi,yi,rootname+'.iona'+ion+'.dat', zmin=0.0)
	except IOError:
		print 'Could not find eith this root (%s) or this (%s) ion' % (rootname,ion)
		return

	den.shape=(100,100)
	frac.shape=(100,100)
	scat.shape=(100,100)
	abs.shape=(100,100)

	# Adjust some of the scalings
	print 'density ',den.min(),den.max()
	den=numpy.maximum(den,den.max()-5.)
	print 'density',den.min(),den.max()


	print 'frac',frac.min(),frac.max()
	frac=numpy.maximum(frac,-5.)
	print 'frac',frac.min(),frac.max()

	print 'scat',scat.min(),scat.max()
	scat=numpy.maximum(scat,scat.max()-2.)
	print 'scat',scat.min(),scat.max()

	print ' abs', abs.min(), abs.max()
	abs=numpy.maximum( abs,abs.max()-2.)
	print ' abs', abs.min(), abs.max()

	# Do plot
	pylab.figure(1,figsize=(12,12))
	pylab.clf()
	pylab.title(ion)

	pylab.subplot(221)
	pylab.title('Density')
	pylab.imshow(den,cmap=None,extent=(0,wsize/1e9,0,wsize/1e9),interpolation=None)
		
	pylab.subplot(222)
	pylab.title('Ion Fraction')
	pylab.imshow(frac,cmap=None,extent=(0,wsize/1e9,0,wsize/1e9),interpolation=None)
		
	pylab.subplot(223)
	pylab.title('Scattering')
	pylab.imshow(scat,cmap=None,extent=(0,wsize/1e9,0,wsize/1e9),interpolation=None)
		
	pylab.subplot(224)
	pylab.title('Abs')
	pylab.imshow(abs,cmap=None,extent=(0,wsize/1e9,0,wsize/1e9),interpolation=None)
		
	pylab.draw()
	pylab.savefig(ion+'.jpg')
		
def get_median(wave,flux,wmin=0,wmax=1e6):
	'''
	get_median(wave,flux,wmin,wmax) gets the median
	value of the flux in the interval wmin,wmax
	'''

	i=0
	while wave[i]<wmin and i<len(wave):
		i=i+1
	imin=i
	while i< len(wave) and wave[i]<wmax: 
		i=i+1
	imax=i
	xmed=numpy.median(flux[imin:imax])
	return(xmed)
	

def uvcompare_two_models(model_root1,model_root2,angle=50.):
	'''
	Compare two python models to one another at a specific inclination

	This produces plots of the models
	'''
	from matplotlib.ticker  import MultipleLocator, FormatStrFormatter

	w,f=read_py_spec(model_root1+'.spec',angle)
	ww,ff=read_py_spec(model_root2+'.spec',angle)


	pylab.figure(2,figsize=(10,6))
	pylab.clf()

	ax=pylab.subplot(231)
	ax_majorLocator=MultipleLocator(30)
	ax.xaxis.set_major_locator(ax_majorLocator)
	pylab.xticks(fontsize=10)
	pylab.yticks(fontsize=10)
	
	# Oxygen doublet 1031.93 (stronger) 1037.62

	delta_wave=40.
	wcenter=1031.93
	wmin=wcenter-delta_wave
	wmax=wcenter+delta_wave

	med_one=get_median(w,f,wmin,wmax)
	med_two=get_median(ww,ff,wmin,wmax)
	fff=med_one/med_two*ff

	pylab.plot(w,f)
	pylab.plot(ww,fff)
	pylab.xlim(wmin,wmax)
	pylab.ylim(0,2*med_one)
	pylab.axvline(wcenter,linewidth=1,color='r',linestyle='-')
	pylab.axvline(1037.62,linewidth=1,color='r',linestyle='-')
	# This is Lyman Beta
	pylab.axvline(1025.72,linewidth=1,color='b',linestyle='-')

	bx=pylab.subplot(232)
	bx_majorLocator=MultipleLocator(30)
	bx.xaxis.set_major_locator(bx_majorLocator)
	pylab.xticks(fontsize=10)
	pylab.yticks(fontsize=10)
	
	# Nitrogen doublet at  1238.321 (stronger), 1242.804
	wcenter=1238.321
	wmin=wcenter-delta_wave
	wmax=wcenter+delta_wave

	med_one=get_median(w,f,wmin,wmax)
	med_two=get_median(ww,ff,wmin,wmax)
	fff=med_one/med_two*ff

	pylab.plot(w,f)
	pylab.plot(ww,fff)
	pylab.xlim(wmin,wmax)
	pylab.ylim(0,2*med_one)
	pylab.axvline(wcenter,linewidth=1,color='r',linestyle='-')
	pylab.axvline(1242.80,linewidth=1,color='r',linestyle='-')
	pylab.axvline(1216.67,linewidth=1,color='b',linestyle='-')

# These are out of the range we are plotting here
#	pylab.axvline(1174.90,linewidth=1,color='b',linestyle='-')
#	pylab.axvline(1176.37,linewidth=1,color='b',linestyle='-')

	print 'check',wcenter,wmin,wmax

	cx=pylab.subplot(233)
	cx_majorLocator=MultipleLocator(30)
	cx.xaxis.set_major_locator(cx_majorLocator)
	pylab.xticks(fontsize=10)
	pylab.yticks(fontsize=10)
	
	# Silicon 1393.756 (stronger), 1402.7697
	wcenter=1393.756
	wmin=wcenter-delta_wave
	wmax=wcenter+delta_wave

	med_one=get_median(w,f,wmin,wmax)
	med_two=get_median(ww,ff,wmin,wmax)
	fff=med_one/med_two*ff

	pylab.plot(w,f)
	pylab.plot(ww,fff)
	pylab.xlim(wmin,wmax)
	pylab.ylim(0,2*med_one)
	pylab.axvline(wcenter,linewidth=1,color='r',linestyle='-')
	pylab.axvline(1402.77,linewidth=1,color='r',linestyle='-')
#	# This is OV
	pylab.axvline(1371.29,linewidth=1,color='b',linestyle='-')


	cx=pylab.subplot(234)
	cx_majorLocator=MultipleLocator(30)
	cx.xaxis.set_major_locator(cx_majorLocator)
	pylab.xticks(fontsize=10)
	pylab.yticks(fontsize=10)
	
	# Carbon IV at 1548.203 (stronger), 1550.777
	wcenter=1548.203
	wmin=wcenter-delta_wave
	wmax=wcenter+delta_wave

	med_one=get_median(w,f,wmin,wmax)
	med_two=get_median(ww,ff,wmin,wmax)
	fff=med_one/med_two*ff

	pylab.plot(w,f)
	pylab.plot(ww,fff)
	pylab.xlim(wmin,wmax)
	pylab.ylim(0,2*med_one)
	pylab.axvline(wcenter,linewidth=1,color='r',linestyle='-')
	pylab.axvline(1550.78,linewidth=1,color='r',linestyle='-')


# This is the whole shebang

	cx=pylab.subplot(235)
	cx_majorLocator=MultipleLocator(200)
	cx.xaxis.set_major_locator(cx_majorLocator)
	pylab.xticks(fontsize=10)
	pylab.yticks(fontsize=10)

	wmin=900
	wmax=1700
	

	med_one=get_median(w,f,wmin,wmax)
	med_two=get_median(ww,ff,wmin,wmax)
	fff=med_one/med_two*ff

	pylab.plot(w,f)
	pylab.plot(ww,fff)
	pylab.xlim(wmin,wmax)
	pylab.ylim(0,2*med_one)
	pylab.axvline(wcenter,linewidth=1,color='r',linestyle='-')




	pylab.draw()


	results=compare_pf(model_root1,model_root2)

	pylab.subplot(236)
#	pylab.axes([0,0,1,1])
#	pylab.text(0.5,0.5,'hello \n Knox')


	i=0
	xxx= ' Summary for Angle: %.2g\n' % (angle)
	xxx=xxx+ 'file1:%30s\n' % model_root1
#	xxx='file1:%30s\n' % model_root1
	xxx=xxx+ 'file2:%30s\n' % model_root2
	while i<len(results):
		xxx=xxx+ '%-25s\n %8.2g %8.2g\n' % (results[i][0],results[i][1],results[i][2])
		i=i+1
#	pylab.text(0.0,0.5,xxx,fontsize=8,fontname='monospace',horizontalalignment='left',verticalalignment='center')
	pylab.text(0.0,0.5,xxx,fontsize=8,horizontalalignment='left',verticalalignment='center',bbox=dict(facecolor='white', alpha=1.0))
	pylab.axis('off')
	pylab.draw()

	pylab.savefig('compare.jpg')

def py_wind(root='sv1_0202010202020102',ver='none'):
	'''
	Produce a standard set of data files from py_wind.  If ver is 'none', then the latest version of python is assumed
	'''

	curdir=os.getcwd()

	#Check to see whether the files are here or in another directory by parsing the rootname

	dirname,root,x,xx=split_filename(root)

	os.chdir(dirname)

	# Check to see if atomic is linked to the current directory and if not run Setup_Py_Dir
	if os.path.exists('atomic'):
		print 'atomic exists'
	else:
		os.system('Setup_Py_Dir')

	# Write a text file used to execute py_wind

	f=open('tmp.py_wind','w')
	f.write('%s\n' % root)
	f.write('1\nF\nq\n')
	f.close()
	command='py_wind'

	# Commented this out
	# if ver!='none':
	# 	command=command+ver
	# elif python_version !='unknown':
# 		command=command+python_version

	print 'Executing ',command

	command=command + ' <tmp.py_wind >tmp.py_wind.out'
	os.system(command)

	# Get a limited amount of information about what happened
	os.system("grep 'Reading Windfile' tmp.py_wind.out")
	os.system('grep Error tmp.py_wind.out')

	# Next line is needed to primarily to get the cursor whre one wants it
	os.chdir(curdir)
	print '\nDone'
