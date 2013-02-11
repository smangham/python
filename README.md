README
***
=========
Precursor - python_68b
Changes
Logging - kpar -Reduced the number of times the same error is recorded in the diag file to 100, and provided a capabiility to control this Log_print_max.
Logging 
- python - Continued modifying the level of error reporting, that is changing various errors from Error to Error_silent.
Format of spectrum files - spectra.c, kpar - Added code so that there ae comments in the .spec files that contain all of the input information. The .spec file now also contains a comment that indicates when the file was created. Currently one usually needs to keep copies of the .pf files with the .spec files to understand a spectrum; this is intended to allow one just to keep the .spec files.
gradv - In debugging the "7 angles" problem - one of the tools I was using noted that dvds_ave had a conditional jump that was operating on an undefined variable. It was not obvious that this had any practical effect, but I fixed the error. It would be sensible at some point to figure out whether the conditional was needed at all.
spectra.c - Fixed a problem that had developed in writing out spectra with more than 7 angles. The problem was due to a header that was being writting into a char string of fixed length. As the number of angles increased, the char string was overrrun. The fix should allow (essentially) an arbitrary number of of angle to be extracted at the same time. In the proces, NSPEC was removed from python.h and put in python.c. It is now used in not other routine in python.
Volume calculation - Changes to the way in which volumes are calculated have been made to make the calculations for spherical polar and cylindrical coordinates more nearly the same. The new method uses a brute force calcuation with 100 x 100 resolution to numerically integrated the volume in each cell that is partially in the wind. Before doing this though, it performs a "robust" check to find out if the cell is completely out of the wind or completly in the wind. This saves computing time since the 1000x1000 volume integrations were taking half the total time to run the program in short diagnostic runs.
Backgrond: Originally, we had used a 100 x 100 resolution numerical integration for the cylindrical volume. At some point, Stuart upped this to 1000 x 1000 for the cylindrical grid, the one we normally use. His motivation was likely to deal with the problme of a photon getting into a cell where the apparent volume was zero but which really was in the wind. This can happen if the volume is not calculated on a fine enough grid. The problem is recorrded as an error and if one runs a very long job the numbers of errors can be so large that the program stops itself because it thinks something is seriously wrong.
The same problem affected the spherical polar coordiante system, as ksl found out when he tried to generate long models in spherical polar coordaintes
The simplest solution to the program "falling over" was to cause the error message to print out less often. This is included in version 68c. The error message now only prints out every 100x a photon gets "stucK' in a cell with zerio apperent volume in the wind. This does not however deal with the performance problem.
What the program does now is to assume that if all four corners of a 2d grid cell are in the wind then all of the cell is in the wind. If this is not the case, it calculates whether any part of any of the 4 boundary lines are in the wind. If the answer to this is yes, then it does the volume integration. If not, it concludes the volume integral would turn out to be zero. (Note - It is not enough just to check the 4 corners because at the outer edge of the wiind can intersect the cell without necessarily intersectiong one of the corners.)
Scattering and absorption - The plasma ptr contains space to record scatters by ion in the wind, and absorption by ion in the wind (for one line of sight). This had been added in python_68a and b, as diagnostics to try to gain a qqualliatitve understanding of where things were happening. Neither of these affect the spect