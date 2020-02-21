KWD.acceleration_exponent
=========================
Sets the length scale over which the accleration to v_inf is accomplished.
It is the value of the exponent beta for the Caster & Lamers equation of a
stellar wind,
v(r) = v_0 + (v_inf - v_0) * (1 - R_s/r) ** beta.

Type
  Double

Values
  Greater than 0

File
  `knigge.c <https://github.com/agnwinds/python/blob/master/source/knigge.c>`_


Parent(s)
  * :ref:`Wind.type`: kwd


