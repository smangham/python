#!/usr/bin/env python 

'''
                    Space Telescope Science Institute

Synopsis:  

Plot the emergent spectrum from the log_spec_tot file


Command line usage (if any):

    usage: plot_tot.py [-smooth 11] rootname

    where 
        rootname is the rootname of the files in the run

        -smooth is an opticnal parameter indicating how
            much smooth of the orignnal spectrum is to 
            be done.  The default is 11 bins


Description:  
    This routines plots to a file a boxcar smoothed version of the
    log spectrum. 
    

Primary routines:

Notes:
                                       
History:

130620    ksl    Coding begun
141125    ksl    Updated to use astropy.io

'''

import sys
import numpy
import pylab
from astropy.io import ascii
from scipy.signal import boxcar
from scipy.signal import convolve


C=2.997e18 # c in Angstroms/s
Lyman=C/912.
HeII=C/912.*4

def xsmooth(flux,smooth=21):
    '''
    boxcar smooth the flux
    '''

    if (smooth)>1:
        q=convolve(flux,boxcar(smooth)/float(smooth),mode='same')
        return(q)
    else:
        return(flux)




def doit(rootname='sphere',smooth=21,fig_no=2):
    '''
    Plot the spectra contained in the spec_tot 
    file

    141125    ksl    Updated for new formats which use astropy
    '''
    # Make sure we only have the rootname

    rootname=rootname.replace('.log_spec_tot','')
    filename=rootname+'.log_spec_tot'

    try:
        data=ascii.read(filename)
    except IOError:
        print('Error: Could not find %s' % filename)
        return

    # print(data.colnames)

    
    pylab.figure(fig_no,(6,6))
    pylab.clf()

    pylab.loglog(data['Freq.'],xsmooth(data['Created']),label='Created')
    # pylab.loglog(data['Freq.'],data['Created'],label='Created')

    pylab.loglog(data['Freq.'],xsmooth(data['Emitted']),label='Observed')
    # pylab.loglog(data['Freq.'],data['Emitted'],label='Observed')

    pylab.loglog(data['Freq.'],xsmooth(data['HitSurf']),label='Hit Surface')
    # pylab.loglog(data['Freq.'],data['HitSurf'],label='Hit Surface')

    pylab.loglog(data['Freq.'],xsmooth(data['Wind']),label='Wind Observed')
    # pylab.loglog(data['Freq.'],data['Wind'],label='Wind Observed')

    zz=pylab.axis()
    pylab.axis((zz[0],zz[1],zz[3]/1e8,zz[3]))

    pylab.text(Lyman,0.9*zz[3],'H',horizontalalignment='center',verticalalignment='top')
    pylab.text(HeII,0.9*zz[3],'HeII',horizontalalignment='center',verticalalignment='top')
    pylab.legend(loc='best')
    pylab.title(rootname)
    pylab.xlabel(r'$ {\nu} $')
    pylab.ylabel(r'L$_{\nu}$')


    pylab.draw()
    pylab.savefig(rootname+'.spec_tot.png')

    # Since the total spectra are basically written out as L_nu we need to 
    # integrate over frequency

    freq=numpy.array(data['Freq.'])
    dfreq=freq[1]-freq[0]  # For a linear scale the frequencies are all equally spaced

    lum_created=dfreq*numpy.sum(numpy.array(data['Created']))
    lum=dfreq*numpy.sum(data['Emitted'])
    # print(data['Created'])
    print('The total   luminosity was ',lum_created)
    print('The emitted luminosity was ',lum)

    return


def steer(argv):
    '''
    Parse the command line
    '''

    smooth=11

    i=1
    while i<len(argv):
        if argv[i]=='-h':
            print(__doc__)
            return
        elif argv[i]=='-smooth':
            i+=1
            smooth=eval(argv[i])
        elif argv[i][0]=='-':
            print('Error: Unknown switch ---  %s' % argv[i])
            return
        else:
            rootname=argv[i]
        i+=1

    doit(rootname,smooth)

    return




# Next lines permit one to run the routine from the command line
if __name__ == "__main__":
    import sys
    if len(sys.argv)>1:
        # doit(int(sys.argv[1]))
        doit(sys.argv[1])
    else:
        print(__doc__)
