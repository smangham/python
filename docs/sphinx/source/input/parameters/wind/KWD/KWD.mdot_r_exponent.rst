KWD.mdot_r_exponent
===================
The exponent for the mass loss rate as defined in the KWD model,
m_dot(r) = F(r) ** alpha = T(r) ** (4 * alpha).
F is the local luminous flux and T is the local temperature at a radius R. A
value of 0 sets a uniform mass loss rate.

Type
  Double

Values
  Greater than or equal to 0

File
  `knigge.c <https://github.com/agnwinds/python/blob/master/source/knigge.c>`_


Parent(s)
  * :ref:`Wind.type`: kwd


