Bound Free (Topbase)
####################


Source
======

Obtained from The `Opacity project <http://cdsweb.u-strasbg.fr/topbase/topbase.html>`_. See also `Cunto et at 1993, A&A, 275, L5 <http://articles.adsabs.harvard.edu/full/1993A%26A...275L...5C>`_


Translation to Python format
============================

ksl - It's not clear that we are now making use of the topbase data in this way but my original attempt to incorporate topbase photoinization data into Python is contained in the directory topbase. Processing of these files was done by py_top_phot. My feeling is that we can replace these remarks with those that are more up to date, once Nick and James discuss this section, and delete any mention of my original attempt on this in the data-gen archive.



Data format
===========
Explain the ascii format of the file which is read into Python

Python structure
================
Where the data is stored internally in Python

Comments
========

**Extrapolation to higher energies**

Around about Python 76 we discovered that some topbase cross-sections have maximum energy cutoffs, for no good reason.
This causes unrealistic edges to appear in spectra. To counter this, JM wrote a 
script to extrapolate the cross-section to higher energies. This is done by 
calculating the gradient in log-space at the maximum energy and extrapolating
to 100 keV. A number of cross-sections had unrealistic gradients at the original 
maximum 
energy, and were identified by eye and then forced to have a :math:`\nu^{-3}` shape.
This is the shape of a hydrogenic cross-section and whilst it is not accurate 
for non-hydrogenic ions, it is more realistic (and conservative) than some of 
the unphysically shallow gradients that were being found.
This is also briefly described in section~3.7.2 of Matthews PhD thesis.
The python scripts can be found in progs/extrapolate\_xs/ 
with docstrings describing their use.

