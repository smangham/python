Overview
########

Python uses a keyword based parameter file the specify a model.   A portion of a parameter file (which must have the extension .pf) is as follows:

.. code::

   Wind.radiation(yes,no)                          yes
   Wind.number_of_components                  1
   Wind.type(SV,star,hydro,corona,kwd,homologous,yso,shell,imported)                   sv
   Wind.coord_system(spherical,cylindrical,polar,cyl_var)          cylindrical
   Wind.dim.in.x_or_r.direction               30
   Wind.dim.in.z_or_theta.direction           30


Each line begins with a keyword followed optionally by a comment in parentheses, and then a value, e.g

* **Keyword:** :code:`Wind.type`
* **Comment:** :code:`SV,star,hydro,corona,kwd,homologous,shell,imported`
* **Value:** :code:`SV`

The comment generally specifies a set of valid choices or the units in which information is expected.

When a series of choices is presented, one does not need to enter the complete word, just enough to provide unique match to the choice.

One does not need to create a parameter file before running Python.
Instead, assuming one is not working from a template parameter file, one simply invokes Python.

.. code:: console

   py my_new_model

or

.. code:: console

   py -n my_new_model


Python then queries the user for answers to a series of question, creating in the process a pf file, my_new_model.pf,
that can be edited and used in future runs.

An example of a line presented to the user in interactive mode is:

.. code::

   Disk.mdot(msol/yr) (1e-08) :

There the number in the second set of parenthesis is a suggested value of the parameter.
The user types in a new value and a carriage return, or, if the the suggested value seems appropriate,
responds with a carriage return, in which case the suggested value will be used.

The :code:`-n` switch above indicates that Python should accumulate all of the necessary inputs, write out the parameter file,
and exit, which is useful if one is not completely sure what one wants.
