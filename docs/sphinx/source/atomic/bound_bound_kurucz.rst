Bound Bound (Kurucz)
####################

This is the data for computing bound bound, or line interactions in simple atoms. There are two main sources of dtaa currently used in Python.
This page discusses the "Kurucz" data

Source
======

The Kurucz data used to create simple lines was taken from the `Kurucz website <http://kurucz.harvard.edu/linelists.html>`_.  
The file gfall.dat was used, though a few extra lines known to have been missing were likely added.

  
Translation to Python format
============================
There are several steps to creating the data used in Python from that in gfall.dat, that are carried out by py_read_kurucz and py_link. The first routine reads the gfall.dat file and creates two output files, a file containing the lines and the associated such as the effective oscillatory strength and a file which contains information about the ion levels.  py_read_kurucz chooses only a portion of the Kurucz lines, namely those associated with ions with ionization potentials in a certain range and lines with gf factors exceeding a certain value. The second program py_link attempts to create a model ion with links between the levels and the ions.  Both of these routines are driven by .pf files, similar to what are used in python.  Examples of the .pf files are in the directory py_kurucz


Data format
===========

+------+---------+-----+----------------------+------+-----------+-----------+-----------+-----------+----------------+---------------+
|Label | Element | Ion | :math:`\lambda(\AA)` | f    |:math:`g_l`|:math:`g_u`|:math:`e_l`|:math:`e_u`|:math:`index_l` |:math:`index_u`|
+------+---------+-----+----------------------+------+-----------+-----------+-----------+-----------+----------------+---------------+
|Line  |1        |1    |926.2                 |0.003 |   2       |   4       |  0.0      | 13.4      |    0           |   9           |
+------+---------+-----+----------------------+------+-----------+-----------+-----------+-----------+----------------+---------------+
|Line  |1        |1    |930.7                 |0.005 |   2       |   4       |  0.0      | 13.3      |    0           |  8            |
+------+---------+-----+----------------------+------+-----------+-----------+-----------+-----------+----------------+---------------+
|Line  |1        |1    |937.8                 |0.008 |   2       |   4       |  0.0      | 13.2      |    0           | 7             |
+------+---------+-----+----------------------+------+-----------+-----------+-----------+-----------+----------------+---------------+

where f is the oscillator strength of the transition, the g nuumbers are the multiplicities of the upper and lower levels, 
the e values are the energy levels relative to ground state of the levels in eV and the two indices relate to the levels structure.



Python structure
================

Where the data is stored internally in Python

Comments
========
The version of gfall.dat that is used in Python is out of date, though whether this affects any of the lines actually used in python is unclear.  The website listed above has a link to newer versions of gfall.dat.


