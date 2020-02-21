Diagnostic files
################

Python logs a considerable amount of information as it runs.
Some of this information is printed to the screen but a much more voluminous version of progress of the program is placed in a sub- directory,
named diag_whatever, where whatever is the root name of the model being run.

In this directory one will find log files, e.g. **whatever_0.diag**, **whatever_1.diag**,
... where the in a multiprocessor run, the number refers to the log of a specific thread.

Inspecting these logs is important for understanding whether a Python run is successful,
and for understanding why if failed if that is the case.
