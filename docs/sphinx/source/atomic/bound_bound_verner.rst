Bound Bound (Verner)
####################

This is the data for computing bound bound, or line interactions in simple atoms. There are two main sources of dtaa currently used in Python.
This page disucsses the "Verner" data


Source
======
Give original references

Translation to Python format
============================
Similar to the Kurucz list there is a program py_read_verner that can produce a line list from the Verner data.  Once done then py_link can be used to link the the levels with the lines.

Data format
===========
The data format is identical to that of produced for the Kurucz data

Python structure
================
Where the data is stored internally in Python

Comments
========
In practice we have not used these data for any Python publications. At some point early in the AGN project, NSH increased the number of lines, and generated lines\_linked\_ver\_2.py and levels\_ver\_2.py. I think this was because there was a small bug which meant the oscillator strength cut that was stated was not that which was applied.
