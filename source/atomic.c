
/***********************************************************/
/** @file  atomic.c
 * @author ksl
 * @date   March, 2018
 *
 * @brief  Routines to calculate rates and x-sections making
 * use of the atomic data
 *
 * ### Notes ###
 *
 * These routines are intended to be Python-independent so that
 * the can be used with other programs.  As a result python.h should
 * not be included.
 *
 * Note that other routines should be added here to make the atomic
 * data routines more indepenent of Python.
 ***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "atomic.h"


/**********************************************************/
/**
 * @brief      double (x_ptr,freq)	calculates the photionization crossection due to the transition
 * 	associated with x_ptr at frequency freq
 *
 * @param [in, out] struct photoionization *  x_ptr   structure containing the x-section information
 * @param [in, out] double  freq   frequency at which the x-section should be calculated
 * @return     xsection for this ion at the given frequency
 *
 * @details
 * sigma_phot uses Verner et al.'s interpolation formulae for the photoionization crossection
 * 	to calculate the bound free (or photoionization) optical depth.
 *
 * ### Notes ###
 * 	The data must
 * 	have been into the photoionization structures xphot with get_atomic_data
 *
 **********************************************************/

double
sigma_phot (x_ptr, freq)
     struct photoionization *x_ptr;
     double freq;
{
  double ft;
  double x, y;
  double f1, f2, f3;
  double xsection;

  ft = x_ptr->freq_t;           /* threshold frequency */

  if (ft < freq && freq < x_ptr->freq_max)
  {
    x = freq / x_ptr->freq0 - x_ptr->y0;
    y = sqrt (x * x + x_ptr->y1 * x_ptr->y1);

    /* This was line fixed by CK in 1998 Jul */
    f1 = (x - 1.0) * (x - 1.0) + x_ptr->yw * x_ptr->yw;

    f2 = pow (y, 0.5 * x_ptr->p - 5.5);
    f3 = pow (1.0 + sqrt (y / x_ptr->ya), -x_ptr->p);
    xsection = x_ptr->sigma * f1 * f2 * f3;     // the photoinization xsection

    return (xsection);
  }
  else
    return (0.0);
}





#define A21_CONSTANT 7.429297e-22       // 8 * PI * PI * E * E / (MELEC * C * C * C)

struct lines *a21_line_ptr;
double a21_a;

/**********************************************************/
/**
 * @brief      Calculate the Einstein A coefficient for a transition
 *
 * @param [in, out] struct lines *  line_ptr   Ptr containing data describing the line
 * @return     The Einsten (a21) value
 *
 * @details
 *
 * ### Notes ###
 *
 * The routine checks whether we are asking for the same a21 as
 * previously, and if so returns it without redoing the calculation.
 *
 **********************************************************/
double
a21 (line_ptr)
     struct lines *line_ptr;
{
  double freq;

  if (a21_line_ptr != line_ptr)
  {
    freq = line_ptr->freq;
    a21_a = A21_CONSTANT * line_ptr->gl / line_ptr->gu * freq * freq * line_ptr->f;
    a21_line_ptr = line_ptr;
  }

  return (a21_a);
}
