Corona.vel_frac
===============
For the coronal model, the azimuthal velocity is
given by the velocity of the underlying disk.  One
can also give the corona a radial velocity, which is
a fraction of the disk velocity.  (As coded, if this
number is positive, the velicty is the r direction is
toward the central object).

Type
  Double

Unit
  Disk velocity

Values
  Any, 0 implies no radial velocity.

File
  `corona.c <https://github.com/agnwinds/python/blob/master/source/corona.c>`_


Parent(s)
  * :ref:`Wind.type`: corona


