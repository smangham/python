Running Python
##############

The normal way to run Python is simply to enter

.. code :: bash

    py xxx

where ``xxx`` is the root name of a parameter file.  (The full name ``xxx.pf`` can also
be entered).

However Python features a number of command line options which can be used
to modify it's operation.  These include the following:

-h
  Causes Python to print out a brief help message and quit. The help message
  principally describes the command line options

-i (or --dry-run)
  Causes Python to read and verify the inputs, writing a clean version of the input
  file ``xxx.pf`` to the output file ``xxx.out.pf``, and then stop. This option is useful
  for setting up a proper ``.pf`` file.  (Often one will want to copy ``xxx.out.pf`` back
  to ``xxx.pf`` before proceeding.

-t time_max
  Limits a run of python to approximately time_max in sec.  This switch is
  used in situations where one would like to check whether the routine is operating
  properly be continuing, or where one needs to checkpoint the program after a certain
  period of time (due for example to time limits placed on jobs in a Beowulf cluster).
  The time is checked at the end of ionization and spectral cycles, immediately after
  saving the binary files that describe a model, and so one needs to leave a cushion
  between time_max and the maximum time one wants the program to run

-r
  Restarts a run that has been interrupted or halted, by reading a the ``xxx.windsave``
  and ``xxx.specsave`` file (if it exists).

-v n
  Changes the amount of information printed to the screen by Python during a run.
  The default is 4.  Larger numbers increase this. Smaller numbers decrease it.
  The log files are not affected.

--rseed
  Causes Python to use a random number seed that is time-based, rather than fixed.
  In most cases, a fixed seed is preferred so that problems can be replicated, but if
  is repeating the same calculation multiple times, then one may want a random seed.

--version
  Causes Python to print out the version number and commit hash (and whether
  uncommitted files exist, and then stop.

-p n_steps
  Changes the number of photons generated during ionization cycles so that the
  number increases logarithmically to the maximum value.  The number ``n_steps`` is optional,
  and specifies the number of decades over which the increase takes place.

  .. todo ::

    NEED TO VERIFY THIS


Special switches
================

Python has a number of other switches that are not intended for the general user, but
which may be useful in certain special cases.  These include:

-d
  Enables a variety of specialized diagnostic inputs which have been implemented
  to help with solving various problems, and were regarded (by someone) as useful
  enough to maintain in the program.  The user is then queried regarding which
  of these diagnostics to enable for a specific run.  These diagnostic queries all start
  with @ (and can co-exist in the ``.pf`` file, with normal commands.

-e n
  Where ``n`` is a number, changes the number of errors of a specific type that
  are allowed to occur before the program gives up.  For a variety of reasons,
  errors are expected during Python runs.
  Most of these errors are harmless in the sense that they occur rarely.
  But if an error occurs too often, something is seriously and so Python halts at that point.
  The default is :math:`10^{5}` (per thread).

-e write n
  Changes the number of times an error message of a specific type is written
  to a diagnostic file.  When errors occur, a line describing the error is written
  to the diagnostic file the first ``n`` times the error occurs. After that statistics
  are maintained as to the number of times the error occurred, but it is not printed
  to the diagnostic file. The default is 100 (per thread)
