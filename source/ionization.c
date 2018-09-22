
/***********************************************************/
/** @file  ionization.c
 * @author ksl
 * @date   May, 2018
 *
 * @brief  Routines used to calculate and update ion densities
 *
 ***********************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "atomic.h"
#include "python.h"



/**********************************************************/
/**
 * @brief      ionization routines for the wind one cell at a time
 *
 * @param [in,out] PlasmaPtr  xplasma   The cell in which the ioniztion is to be calculated
 * @param [in] int  mode   A parameter descibing how to calculate the ioniztion
 * @return     The routine returns status messages dereived from the individual routines
 * used to calculate the abundances.
 *
 * @details
 * The intent is that the routine ion_abundances is the steering routine for
 * all calculations of the abundances
 *
 * ### Notes ###
 *
 **********************************************************/

int
ion_abundances (xplasma, mode)
     PlasmaPtr xplasma;
     int mode;
{
  int ireturn;


  if (mode == IONMODE_ML93_FIXTE)
  {
/* on-the-spot approximation using existing t_e.   This routine does not attempt
to match heating and cooling in the wind element! */

    if ((ireturn = nebular_concentrations (xplasma, NEBULARMODE_ML93)))
    {
      Error ("ionization_abundances: nebular_concentrations failed to converge\n");
      Error ("ionization_abundances: j %8.2e t_e %8.2e t_r %8.2e w %8.2e\n", xplasma->j, xplasma->t_e, xplasma->w);
    }
  }
  else if (mode == IONMODE_LTE_TR)
  {
/* LTE using t_r */
    ireturn = nebular_concentrations (xplasma, NEBULARMODE_TR);
  }
  else if (mode == IONMODE_LTE_TE)
  {
/* LTE using t_e */
    ireturn = nebular_concentrations (xplasma, NEBULARMODE_TE);
  }
  else if (mode == IONMODE_FIXED)
  {                             //  Hardwired concentrations

    ireturn = fix_concentrations (xplasma, 0);
  }
  else if (mode == IONMODE_ML93)
  {
/* On the spot, with one_shot at updating t_e before calculating densities */

/* Shift values to old */
    xplasma->dt_e_old = xplasma->dt_e;
    xplasma->dt_e = xplasma->t_e - xplasma->t_e_old;
    xplasma->t_e_old = xplasma->t_e;
    xplasma->t_r_old = xplasma->t_r;
    xplasma->lum_tot_old = xplasma->lum_tot;

    ireturn = one_shot (xplasma, mode);

/* Convergence check */
    convergence (xplasma);
  }
  else if (mode == IONMODE_PAIRWISE_ML93 || mode == IONMODE_MATRIX_BB)
  {
    /* Feb 2012 new for mode 6. New abundances have been computed using pairwise Saha equation
       approach. We can now attempt to balance heating and cooling with the new abundance in the
       same way as mode 3. */

/* Shift values to old */
    xplasma->dt_e_old = xplasma->dt_e;
    xplasma->dt_e = xplasma->t_e - xplasma->t_e_old;
    xplasma->t_e_old = xplasma->t_e;
    xplasma->t_r_old = xplasma->t_r;
    xplasma->lum_tot_old = xplasma->lum_tot;

    ireturn = one_shot (xplasma, mode);

/* Convergence check */
    convergence (xplasma);
  }
  else if (mode == IONMODE_PAIRWISE_SPECTRALMODEL || mode == IONMODE_MATRIX_SPECTRALMODEL)
  {

/*  spectral_estimators does the work of getting banded W and alpha. Then oneshot gets called. */

    ireturn = spectral_estimators (xplasma);

    xplasma->dt_e_old = xplasma->dt_e;
    xplasma->dt_e = xplasma->t_e - xplasma->t_e_old;
    xplasma->t_e_old = xplasma->t_e;
    xplasma->t_r_old = xplasma->t_r;
    xplasma->lum_tot_old = xplasma->lum_tot;


    ireturn = one_shot (xplasma, mode);


/* Convergence check */
    convergence (xplasma);
  }
  else
  {
    Error ("ion_abundances: Could not calculate abundances for mode %d\n", mode);
    exit (0);
  }

  /* If we want the Auger effect deal with it now. Initially, this is
     put in here, right at the end of the ionization calculation -
     the assumption is that the Auger effect is only for making minor
     ions so that the ionization balance of the other ions is not
     affected in an important way. */

//  if (geo.auger_ionization == 1)
//    {
//      auger_ionization (xplasma);
//    }


  return (ireturn);

}



/**********************************************************/
/**
 * @brief      checks to see whether a single cell
 * 	is or is not converging
 *
 * @param [in out] PlasmaPtr  xplasma   The cell of interest
 * @return    A number which describes the degree to which
 * the cell is converging
 *
 * @details
 * The routine attempts to determine whether a cell is
 * on the track to a final solution by checking whether
 * the electron and radiation temperatures are getting
 * smaller with cycle and whetehr the difference between
 * heating and cooling is drroping.
 *
 * The routine returns a number between 0 and 3,
 * depending on the number of convergence checks that
 * are passed.  If all convergence tests are pssed
 * then the number returned will be 0
 *
 * The routine also adjust the gain which controls how
 * far the electron temperture can change in a cycle.
 *
 * ### Notes ###
 *
 **********************************************************/

int
convergence (xplasma)
     PlasmaPtr xplasma;
{
  int trcheck, techeck, hccheck, whole_check, converging;
  double epsilon;

  trcheck = techeck = hccheck = converging = 0;
  xplasma->trcheck = xplasma->techeck = xplasma->hccheck = 0;   //NSH 70g - zero the global variables
  epsilon = 0.05;

  /* Check the fractional change in tempperatature and if is less than
   * epsiolong increment trcheck and techeck
   */

  if ((xplasma->converge_t_r = fabs (xplasma->t_r_old - xplasma->t_r) / (xplasma->t_r_old + xplasma->t_r)) > epsilon)
    xplasma->trcheck = trcheck = 1;
  if (xplasma->t_e < TMAX)
  {
    if ((xplasma->converge_t_e = fabs (xplasma->t_e_old - xplasma->t_e) / (xplasma->t_e_old + xplasma->t_e)) > epsilon)
      xplasma->techeck = techeck = 1;
    if ((xplasma->converge_hc = fabs (xplasma->heat_tot - xplasma->cool_tot) / fabs (xplasma->heat_tot + xplasma->cool_tot)) > epsilon)
      xplasma->hccheck = hccheck = 1;
  }
  else                          //If the cell has reached the maximum temperature
  {
    xplasma->techeck = techeck = xplasma->hccheck = hccheck = 2;        //we mark it as overlimit
  }

//110919 nsh modified line below to include the adiabatic cooling in the check that heating equals cooling
//111004 nsh further modification to include DR and compton cooling, now moved out of lum_tot

  /* Check whether the heating and colling balance to within epsilon and if so set hccheck to 1 */
  /* 130722 added a fabs to the bottom, since it is now conceivable that this could be negative if
     cool_adiabatic is large and negative - and hence heating */

/* NSH 130711 - also changed to have fabs on top and bottom, since heating can now be negative!) */

/* NSH 130725 - moved the hc check to be within the if statement about overtemp - we cannot expect hc to converge if we are hitting the maximum temperature */
  /* whole_check is the sum of the temperature checks and the heating check */

  xplasma->converge_whole = whole_check = trcheck + techeck + hccheck;

  /* Converging is a situation where the change in electron
   * temperature is dropping with time and the cell is oscillating
   * around a temperature.  If that is the case, we drop the
   * amount by which the temperature can change in this cycle
   */

  if (xplasma->dt_e_old * xplasma->dt_e < 0 && fabs (xplasma->dt_e) > fabs (xplasma->dt_e_old))
    converging = 1;
  xplasma->converging = converging;

  if (converging == 1)
  {                             // Not converging
    xplasma->gain *= 0.7;
    if (xplasma->gain < 0.1)
      xplasma->gain = 0.1;
  }
  else
  {
    xplasma->gain *= 1.1;
    if (xplasma->gain > 0.8)
      xplasma->gain = 0.8;
  }

  return (whole_check);
}



/**********************************************************/
/**
 * @brief      The routien summarizes the how well the wind converging
 * to a solution as a whole
 *
 * @return     Always returns 0
 *
 * @details
 * Basically the routine just looks at the numbers of cells which haave passed/failed
 * the various convergence tests and writes this to the log file
 *
 * ### Notes ###
 * @bug It would make sense to write this information to a separate file so that
 * plots of the rate of convergence could be easily made.
 *
 **********************************************************/

int
check_convergence ()
{
  int n;
  int nconverge, nconverging, ntot;
  int nte, ntr, nhc;            //NSH 70g - three new counters for the different convergence criteria
  int nmax;                     //NSH 130725 - counter for cells which are marked as converged, but over temp
  double xconverge, xconverging;

  nconverge = nconverging = ntot = 0;
  ntr = nte = nhc = nmax = 0;

  for (n = 0; n < NPLASMA; n++)
  {
    ntot++;
    if (plasmamain[n].converge_whole == 0)
      nconverge++;
    if (plasmamain[n].trcheck == 0)
      ntr++;
    if (plasmamain[n].techeck == 0)
      nte++;
    if (plasmamain[n].hccheck == 0)
      nhc++;
    if (plasmamain[n].techeck == 2)
      nmax++;
    if (plasmamain[n].converging == 0)
      nconverging++;

  }

  xconverge = ((double) nconverge) / ntot;
  xconverging = ((double) nconverging) / ntot;
  geo.fraction_converged = xconverge;
  Log
    ("!!Check_converging: %4d (%.3f) converged and %4d (%.3f) converging of %d cells\n",
     nconverge, xconverge, nconverging, xconverging, ntot);
  Log ("!!Check_convergence_breakdown: t_r %4d t_e(real) %4d t_e(maxed) %4d hc(real) %4d\n", ntr, nte, nmax, nhc);
  Log
    ("Summary  convergence %4d %.3f  %4d  %.3f  %d  #  n_converged fraction_converged  converging fraction_converging total cells\n",
     nconverge, xconverge, nconverging, xconverging, ntot);
  Log_flush ();
  return (0);
}




PlasmaPtr xxxplasma;


/**********************************************************/
/**
 * @brief      calculates new densities of ions in a single element of the wind
 * 	after (usually) having found the
 * 	temperature which matches heating and cooling for the previous
 * 	densities
 *
 * @param [in,out] PlasmaPtr  xplasma   The plasma cell of interest
 * @param [in] int  mode   A switch describing what approximation to use in determinging the
 * densities
 * @return     Always returns 0
 *
 * @details
 * This routine attempts to match heating and cooling in the wind element!
 * To do this it calls calc_te.  Based on the returned value of te, the
 * routine then calculates densities for various ions in the cell.  The densities
 * in xplasma are updated.
 *
 * ### Notes ###
 *
 *
 * Special exceptions are made for Zeus; it is not clear why this is necessary
 *
 * @bug There are some numbered switches included here.  This is because these values
 * are not allowed anymore. But this needs to be cleaned up.  Deactivated modes should
 * be captured when the parameters are read in, not at this point. An else statement
 * should provide a sufficent check for any unknown mode.
 *
 **********************************************************/

int
one_shot (xplasma, mode)
     PlasmaPtr xplasma;
     int mode;

{
  double te_old, te_new;
  double gain;



  gain = xplasma->gain;

  te_old = xplasma->t_e;

  if (modes.zeus_connect == 1 || modes.fixed_temp == 1)
  {
    te_new = te_old;            //We don't want to change the temperature
    xxxplasma = xplasma;
    zero_emit (te_old);         //But we do still want to compute all heating and cooling rates
  }
  else                          //Do things to normal way - look for a new temperature
  {
    te_new = calc_te (xplasma, 0.7 * te_old, 1.3 * te_old);     //compute the new t_e - no limits on where it can go
    xplasma->t_e = (1 - gain) * te_old + gain * te_new; /*Allow the temperature to move by a fraction gain towards
                                                           the equilibrium temperature */

    /* NSH 130722 - NOTE - at this stage, the cooling terms are still those computed from
     * the 'ideal' t_e, not the new t_e - this may be worth investigatiing. */
    if (xplasma->t_e > TMAX)    //check to see if we have maxed out the temperature.
    {
      xplasma->t_e = TMAX;
    }
    zero_emit (xplasma->t_e);   //Get the heating and cooling rates correctly for the new temperature
  }



/* Modes in the driving routines are not identical to those in nebular concentrations.
The next lines are an attempt to mediate this problem.  It might be better internally
at least to define a flag for using one shot, and have the modes take on the
meaning in nebular concentrations.
*/

  if (mode == IONMODE_ML93)
    mode = NEBULARMODE_ML93;    // This is weird, why not continue
  else if (mode <= 1 || mode == 5 || mode > 9)
  {
    /* There is no mode 5 at present  - SIM + two new modes in Feb 2012  + mode 5 now removed */

    Error ("one_shot: Sorry, Charlie, don't know how to process mode %d\n", mode);
    exit (0);
  }

  if (xplasma->t_r > 10.)
  {                             /* Then modify to an on the spot approx */
    if (nebular_concentrations (xplasma, mode))
    {
      Error ("ionization_on_the_spot: nebular_concentrations failed to converge\n");
      Error ("ionization_on_the_spot: j %8.2e t_e %8.2e t_r %8.2e w %8.2e nphot %i\n", xplasma->j, xplasma->t_e, xplasma->w, xplasma->ntot);
    }
    if (xplasma->ne < 0 || VERY_BIG < xplasma->ne)
    {
      Error ("ionization_on_the_spot: ne = %8.2e out of range\n", xplasma->ne);
    }
  }
  else
  {
    Error ("ionization_on_the_spot: t_r exceptionally small %g\n", xplasma->t_r);
    exit (0);
  }


  return (0);
}





/**********************************************************/
/**
 * @brief
 * calc_te determines and returns the electron temperature in the wind such that the energy emitted
 * by the wind is equal to energy emitted (asssuming it is in the interval bracketed by
 * tmin and tmax.)
 *
 * @param [in out] PlasmaPtr  xplasma   A plasma cell in the wind
 * @param [in out] double  tmin   A bracketing minimum temperature
 * @param [in out] double  tmax   A bracketing mxximum temperature
 * @return     The temperature where heating and cooling match
 *
 * @details
 * The routine iterates to find the temperature in a cell, where heating and cooling are matched.
 *
 * calc_te does not modify any abundances.  It simply takes the current value of the heating in the
 * cell and attempts to find the value of the electron temperature which will result in cooling which
 * matches the heating.
 *
 *
 * ### Notes ###
 * Ion densities are NOT updated in this process.
 *
 * xxxplasma is just a way to tranmit information to zero_emit
 *
 **********************************************************/

double
calc_te (xplasma, tmin, tmax)
     PlasmaPtr xplasma;
     double tmin, tmax;
{
  double z1, z2;
  int macro_pops ();


  /* we assign a plasma pointer here to a fixed structure because
   * we need to call zbrent and we cannot pass the xplasma ptr directly
   */

  xxxplasma = xplasma;


  xplasma->t_e = tmin;
  z1 = zero_emit (tmin);
  xplasma->t_e = tmax;
  z2 = zero_emit (tmax);

  /* The way this works is that if we have a situation where the cooling
   * at tmax and tmin brackets the heating, then we use zbrent to improve
   * the estimated temperature, but if not we chose the best direction
   */

  if ((z1 * z2 < 0.0))
  {                             // Then the interval is bracketed
    xplasma->t_e = zbrent (zero_emit, tmin, tmax, 50.);
  }
  else if (fabs (z1) < fabs (z2))
  {
    xplasma->t_e = tmin;
  }
  else
  {
    xplasma->t_e = tmax;
  }
  /* With the new temperature in place for the cell, get the correct value of heat_tot.
     SS June  04 */

  /* ksl - XXX I basically don't undestand what is going on here.  If we start using
   * macro atoms a lot we need to understand them better ??? -
   * Look at zero emit as well 091611 */

  xplasma->heat_tot -= xplasma->heat_lines_macro;
  xplasma->heat_lines -= xplasma->heat_lines_macro;
  xplasma->heat_lines_macro = macro_bb_heating (xplasma, xplasma->t_e);
  xplasma->heat_tot += xplasma->heat_lines_macro;
  xplasma->heat_lines += xplasma->heat_lines_macro;

  xplasma->heat_tot -= xplasma->heat_photo_macro;
  xplasma->heat_photo -= xplasma->heat_photo_macro;
  xplasma->heat_photo_macro = macro_bf_heating (xplasma, xplasma->t_e);
  xplasma->heat_tot += xplasma->heat_photo_macro;
  xplasma->heat_photo += xplasma->heat_photo_macro;


  return (xplasma->t_e);

}



/* This is just a function which has a zero when total energy loss is equal to total energy gain */


/**********************************************************/
/** 
 * @brief      Compute the cooling for a cell given a temperature t, and compare it
 * to the heating seen in the cell in the previous ionization cycle
 *
 * @param [in] double  t   A trial temperature
 * @return     The difference between the recorded heating and the cooling
 *
 * @details
 * This routine is used to estiamate a new temperature for a cell given the
 * heating of the cell in the previous ionization cycle.
 *
 * ### Notes ###
 * The abundances of ions in the cell are not modified.  Results are stored
 * in the cell of interest.  This routine is used in connection with a zero
 * finding routine
 *
 **********************************************************/

double
zero_emit (t)
     double t;
{
  double difference;
  double total_emission ();
  int macro_pops ();
  double macro_bb_heating (), macro_bf_heating ();

  /*Original method */
  xxxplasma->t_e = t;


  /* Correct heat_tot for the change in temperature. SS June 04. */
  xxxplasma->heat_tot -= xxxplasma->heat_lines_macro;
  xxxplasma->heat_lines -= xxxplasma->heat_lines_macro;
  xxxplasma->heat_lines_macro = macro_bb_heating (xxxplasma, t);
  xxxplasma->heat_tot += xxxplasma->heat_lines_macro;
  xxxplasma->heat_lines += xxxplasma->heat_lines_macro;

  xxxplasma->heat_tot -= xxxplasma->heat_photo_macro;
  xxxplasma->heat_photo -= xxxplasma->heat_photo_macro;
  xxxplasma->heat_photo_macro = macro_bf_heating (xxxplasma, t);
  xxxplasma->heat_tot += xxxplasma->heat_photo_macro;
  xxxplasma->heat_photo += xxxplasma->heat_photo_macro;


  cooling (xxxplasma, t);

  difference = xxxplasma->heat_tot - xxxplasma->cool_tot;



  return (difference);
}
