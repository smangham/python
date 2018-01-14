#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "atomic.h"
#include "python.h"

/***********************************************************
             University of Southampton

Synopsis: 
  parse_command_line parses the command line and communicates stuff
  to the loggin routines (e.g. verbosity).
   
Arguments:   
  argc            command line arguments 

Returns:

  restart_start   1 if restarting
 
Description:  

Notes:

	The switches should be described in the introduction to
	python.c, instead of here

History:
  1502  JM  Moved here from main()
  1610	ksl	Added a new switch -dry-run which is functionally
  		equivalent to -i. Also dealt with the possibility
		that all of the command line would be consumed in
		switches with no parameter file specified.

**************************************************************/


int
parse_command_line (argc, argv)
     int argc;
     char *argv[];
{
  int restart_stat, verbosity, max_errors, i;
  int j = 0;
  char dummy[LINELENGTH];
  int mkdir ();
  double time_max;

  restart_stat = 0;

  if (argc == 1)
  {
    printf ("Parameter file name (e.g. my_model.pf, or just my_model):");
    fgets (dummy, LINELENGTH, stdin);
    get_root (files.root, dummy);
    strcpy (files.diag, files.root);
    strcat (files.diag, ".diag");
  }
  else
  {

    for (i = 1; i < argc; i++)
    {
      if (strcmp (argv[i], "-h") == 0)
      {
        help ();
      }
      else if (strcmp (argv[i], "-r") == 0)
      {
        Log ("Restarting %s\n", files.root);
        restart_stat = 1;
        j = i;
      }
      else if (strcmp (argv[i], "-t") == 0)
      {
        if (sscanf (argv[i + 1], "%lf", &time_max) != 1)
        {
          Error ("python: Expected time after -t switch\n");
          exit (0);
        }
        i++;
        j = i;

      }
      else if (strcmp (argv[i], "-v") == 0)
      {
        if (sscanf (argv[i + 1], "%d", &verbosity) != 1)
        {
          Error ("python: Expected verbosity after -v switch\n");
          exit (0);
        }
        Log_set_verbosity (verbosity);
        i++;
        j = i;

      }
      else if (strcmp (argv[i], "-e") == 0)
      {
        if (sscanf (argv[i + 1], "%d", &max_errors) != 1)
        {
          Error ("python: Expected max errors after -e switch\n");
          exit (0);
        }
        Log_quit_after_n_errors (max_errors);
        i++;
        j = i;

      }
      else if (strcmp (argv[i], "-d") == 0)
      {
        modes.iadvanced = 1;
        j = i;
      }
      else if (strcmp (argv[i], "-f") == 0)
      {
        modes.fixed_temp = 1;
        j = i;
      }

      /* JM 1503 -- Sometimes it is useful to vary the random number seed. Set a mode for that */
      else if (strcmp (argv[i], "--rseed") == 0)
      {
        modes.rand_seed_usetime = 1;
        j = i;
      }
      else if (strcmp (argv[i], "-z") == 0)
      {
        modes.zeus_connect = 1;
        Log ("setting zeus_connect to %i\n", modes.zeus_connect);
        j = i;
      }
      else if (strcmp (argv[i], "-i") == 0)
      {
        modes.quit_after_inputs = 1;
        j = i;
      }
      else if (strcmp (argv[i], "--dry-run") == 0)
      {
        modes.quit_after_inputs = 1;
        j = i;
      }

      else if (strcmp (argv[i], "--version") == 0)
      {
        /* give information about the pyhon version, such as commit hash */
        Log ("Python Version %s \n", VERSION);  //54f -- ksl -- Now read from version.h
        Log ("Built from git commit hash %s\n", GIT_COMMIT_HASH);
        /* warn the user if there are uncommited changes */
        int git_diff_status = GIT_DIFF_STATUS;
        if (git_diff_status > 0)
          Log ("This version was compiled with %i files with uncommitted changes.\n", git_diff_status);
        exit (0);
      }

      else if (strncmp (argv[i], "-", 1) == 0)
      {
        Error ("python: Unknown switch %s\n", argv[i]);
        help ();
      }
    }

    /* The last command line variable is always the .pf file */

    if (j + 1 == argc)
    {
      Error ("All of the command line has been consumed without specifying a parameter file name, so exiting\n");
      exit (0);
    }


    strcpy (dummy, argv[argc - 1]);
    get_root (files.root, dummy);

    /* This completes the parsing of the command line */

    /* Create a subdirectory to store diaganostic files */

    sprintf (files.diagfolder, "diag_%s/", files.root);
    mkdir (files.diagfolder, 0777);
    strcpy (files.diag, files.diagfolder);
    sprintf (dummy, "_%d.diag", rank_global);
    strcat (files.diag, files.root);
    strcat (files.diag, dummy);


  }

  return (restart_stat);
}


/***********************************************************
                                       Space Telescope Science Institute

 Synopsis:
	print out some basic help on how to run the program
Arguments:		

Returns:
 
Description:	
		
Notes:

The easiest way to create the message, at least initially, is simply to to type
out what you want to appear on the screen and then as \n\ to all of the lines, including
the ones with nothing in them

History:
	081217	ksl	67c - Added so that ksl could remember all of the options
	09feb	ksl	68b - Added info on -v switch

**************************************************************/

int
help ()
{
  char *some_help;

  some_help = "\
\n\
This program simulates radiative transfer in a (biconical) CV, YSO, quasar or (spherical) stellar wind \n\
\n\
	Usage:  py [-h] [-r] [-t time_max] xxx  or simply py \n\
\n\
	where xxx is the rootname or full name of a parameter file, e. g. test.pf \n\
\n\
	and the switches have the following meanings \n\
\n\
	-h 	to ge this help message \n\
	-r 	restart a run of the progarm reading the file xxx.windsave \n\
	-e change the maximum number of errors before quit- don't do this unless you understand\
	the consequences! \n\
\n\
	-t time_max	limit the total time to approximately time_max seconds.  Note that the program checks \n\
		for this limit somewhat infrequently, usually at the ends of cycles, because it \n\
		is attempting to save the program outputs so that the program can be restarted with \n\
		-r if that is desired. \n\
\n\
	-v n	controls the amount of print out.  The default is 4.  Larger numbers increase  \n\
		the amount printed; smaller numbers decrease it.   \n\
   --dry-run	Create a new .pf file and stop \n\
   --version	print out python version, commit hash and if there were files with uncommitted \n\
                changes \n\
      --rseed   set the random number seed to be time based, rather than fixed. \n\
\n\
(Certain other switches exist but these are largely diagnostic, or for special cases) \n\
\n\
If one simply types py or pyZZ where ZZ is the version number, one is queried for a name \n\
	of the parameter file. \n\
\n\
\n\
";                              // End of string to provide one with help

  printf ("%s\n", some_help);

  exit (0);
}
