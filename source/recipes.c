
/***********************************************************/
/** @file  recipes.c
 * @author ksl
 * @date   May, 2018
 *
 * @brief  These are collection of Numberical Recipes routines
 * used in Python
 *
 *
 * The routines have been modified to make them ansi compatable
 *
 ***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_min.h>
#include <gsl/gsl_errno.h>

#include "atomic.h"
#include "python.h"
#include "recipes.h"
#include "log.h"


/******************************
 * The next two routines were written by ksl.  They were not part of
   the recipes programs which I had but I think they are what was intended
********************************/

double *
vector (i, j)
     int i, j;
{
  double dummy, *d;
  d = calloc (sizeof (dummy), (j - i + 1) + 1);
  return (d);
}

void
free_vector (a, i, j)
     double *a;
     int i, j;
{
  free (a);
}



/**********************************************************/
/**
 * @brief      A wrapper function that carries out numerical integration on a supplied function between limits
 *
 *
 * @param [in] func - a function to integrate, needs to be of the form (double (double,void*))
 * @param [in] a - lower bound
 * @param [in] b - upper bound
 * @param [in] eps - the relative accuracy desired.
 * @return   The integral                          .
 *
 * @details
 * This routine carries out numerical integration of the function func from a to b. It currently
 * uses the gsl implementation of the ROMBERG integration scheme. Initial tests using QAG showed that
 * the type of integrals we do - often with exponential tails - is not well treated. There is an 
 * internal test to ensure that the user isnt asking for an integral where the function is zero everywhere.
 * The gsl routine doesnt handle this well, and takes ages to (sometimes) return zero.
 *
 * ### Notes ###
 *
 **********************************************************/


double
num_int (func, a, b, eps)
     double (*func) (double, void *);
     double a, b;
     double eps;
{
  double result, error, result2;
  double alpha = 0.0;
  void *test = NULL;
  double delta;
  int zflag, i;
  int status = 0;
  int status2 = 0;

  int npoints;
  size_t neval;
  gsl_function F;
  F.function = func;
  F.params = &alpha;
  zflag = 1;
  npoints = 1000;
  if (func (a, test) == 0.0 && func (b, test) == 0.0)
  {
    zflag = 0;
    delta = (b - a) / 101;
    for (i = 0; i < 100; i++)
    {
      if (func (a + delta * i, test) != 0)
        zflag = 1;
    }
  }
  if (zflag == 1)
  {
    gsl_set_error_handler_off ();       //We need to be able to catch and handle gsl errors 

    gsl_integration_workspace *w = gsl_integration_workspace_alloc (1000);
    status = gsl_integration_qags (&F, a, b, 0, eps, 1000, w, &result, &error);
    if (status)
    {
      if (status == GSL_EROUND) //The rounding error has been triggered - try a different integrator
      {
        gsl_integration_workspace_free (w);
        gsl_integration_romberg_workspace *w = gsl_integration_romberg_alloc (30);
        status2 = gsl_integration_romberg (&F, a, b, 0, eps, &result2, &neval, w);
        gsl_integration_romberg_free (w);
        if (status2)
        {
          Error ("num_init: some kind of error in romberg and qags integration\n");
        }
      }
    }
    else
    {
      gsl_integration_workspace_free (w);
    }
  }
  else
  {
    result = 0.0;
  }

  return (result);
}

/**********************************************************/
/**
* @brief      A wrapper function that finds the root of a function between two limits
*
*
* @param [in] func - the function we want to find the root of, needs to be of the form (double (double,void*))
* @param [in] a - lower bound
* @param [in] b - upper bound
* @param [in] eps - the relative accuracy desired.
* @return   The location between a and b of the zero point                         .
*
* @details
* This routine finds the root  of the function func from a to b. It currently
* uses the gsl implementation of the BRENT root finding scheme. This replaced the zbrent 
* numerical recipie, and the call is intended to be indentical
*
* ### Notes ###
*
**********************************************************/


double
zero_find (func, x_lo, x_hi, tol)
     double (*func) (double, void *);
     double x_lo, x_hi;
     double tol;
{
  double result;
  double alpha = 0.0;
  double r = 0;
  const gsl_root_fsolver_type *T;
  gsl_root_fsolver *s;
  int iter = 0, max_iter = 100;
  int status;


  gsl_function F;
  F.function = func;
  F.params = &alpha;



  T = gsl_root_fsolver_brent;
  s = gsl_root_fsolver_alloc (T);
  gsl_root_fsolver_set (s, &F, x_lo, x_hi);



  do
  {
    iter++;
    status = gsl_root_fsolver_iterate (s);
    r = gsl_root_fsolver_root (s);
    x_lo = gsl_root_fsolver_x_lower (s);
    x_hi = gsl_root_fsolver_x_upper (s);
    status = gsl_root_test_interval (x_lo, x_hi, tol, 0);



  }
  while (status == GSL_CONTINUE && iter < max_iter);

  result = (x_lo + x_hi) / 2.0;

  return (result);
}


/**********************************************************/
/**
* @brief      A routine that mimimizes a function f
*
*
* @param [in] ax - lower bound
* @param [in] bx - guess where the minimum is found
* @param [in] cx - upper bound
* @param [in] f  the function to minimize
* @param [in] tol A tolerance
* @param [out] * xmin - The place where the minimum occors
* @return   The minimum of the function f                           .
*
* @details
* This is a wrapper routine which uses gsl calls to find the minimum of a function
*
* ### Notes ###
* added in 2019 to replace the function 'golden' which is a numerical recipie
* 
**********************************************************/



double
func_minimiser (a, m, b, func, tol, xmin)
     double (*func) (double, void *);
     double a, m, b;
     double tol, *xmin;

{
  int status = 0;
  void *test = NULL;


  int iter = 0, max_iter = 100;
  const gsl_min_fminimizer_type *T;
  gsl_min_fminimizer *s;


  gsl_function F;
  F.function = func;
  F.params = 0;



  T = gsl_min_fminimizer_brent;

  s = gsl_min_fminimizer_alloc (T);
  gsl_set_error_handler_off ();
  status = gsl_min_fminimizer_set (s, &F, m, a, b);
  if (status)
  {
    if (status == GSL_EINVAL)   //THere is no minimum 
    {
      return fmin (func (a, test), func (b, test));     //keep old behaviour
    }
  }


  do
  {
    iter++;
    status = gsl_min_fminimizer_iterate (s);
    m = gsl_min_fminimizer_x_minimum (s);
    a = gsl_min_fminimizer_x_lower (s);
    b = gsl_min_fminimizer_x_upper (s);
    status = gsl_min_test_interval (a, b, 0.0, tol);
  }
  while (status == GSL_CONTINUE && iter < max_iter);
  *xmin = m;

  return func (m, test);
}
