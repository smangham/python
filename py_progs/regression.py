#!/usr/bin/env python 

'''
                    Space Telescope Science Institute

Synopsis:  

Execute a series of short python runs to test whether a version 
of python is working.


Command line usage (if any):

    usage: regression.py [-np 3 -pf_dir test -out_dir foo] version 

    where 

        version         the executable of python
        -np 3           the number of processors with which to run (default 3)
        -pf_dir test    the directory containing all of the .pf files which will be run
                        The defaults is $PYTHON/examples/regress
        -out_dir foo    The directory (below the current working directory) where the 
                        tests will run.  The defauld is constructed for the version
                        and the data

Description:  

    The basic process is as follows

    1.  Create a directory in which to work and intialize it
    2.  Copy all of the relevant .pf files to this directory
    3.  Switch to the working directory and run all the models, performing
        some basic checks 

Primary routines:

    doit:       Internal routine which runs python on all of the pf files of interest.  Use
                this if working in a python shell
    steer:      A routine to parse the command lineh
    check_one:  A routine which oversees checking of the runs

Notes:

    Regression here means to run a series of models. These routines do not compare the
    models to earlier runs
                                       
History:

170903 ksl Coding begun

'''

import sys
from astropy.io import ascii
import numpy
from glob import glob
import time
import subprocess
import os
import shutil




def sum_errors(root='1d_sn'):
    '''
    This sums the errors from various threads

    Note that this effectively duplicates a routine
    py_error which James wrote, but in Python 3


    '''
    diag_files=glob('diag_%s/%s_*.diag' % (root,root))

    errors=[]
    for one in diag_files:
        x=open(one)
        lines=x.readlines()
        lines=lines[len(lines)-100:]
        test=False
        for one_line in lines:
            if test and one_line.count(' -- '):
                errors.append(one_line)
            if one_line.count('Recurrences --  Description'):
                test=True

    # At this point we have the error but we need to parse them etc

    i=0
    error_names=[]
    error_counts=[]
    while i<len(errors):
        errors[i]=errors[i].strip()
        words=errors[i].split(' -- ')
        error_names.append(words[1])
        error_counts.append(int(words[0]))
        i+=1

    print(errors)

    # Now get the unique names

    unique_errors=set(error_names)

    print(unique_errors)

    # finally add up the errors from the various threads

    records=[]
    for one in unique_errors:
        name=one
        counts=0
        i=0
        while i<len(error_names):
            if name==error_names[i]:
                counts+=error_counts[i]
            i+=1
        records.append([name,counts])
    return(records)


def check_one(xfile,root):
    '''
    Perform some checks on what happened in a run
    '''

    g=open(root+'.sig')
    lines=g.readlines()
    last_line=lines[len(lines)-1]
    words=last_line.split()
    if last_line.count('COMPLETE'):
        xfile.write('The model ran to completion in %s s\n' % (words[5]))
    else:
        xfile.write('The models stopped before completing\n')

    errors=sum_errors(root)

    xfile.write('\n The reported errors in all threads was as follows: \n')
    for one in errors:
        xfile.write('%10d -- %s\n' % (one[1],one[0]))

    return
    
    


def doit(version='py',pf_dir='',out_dir='',np=3,outputfile='Summary.txt'):
    '''
    Test a specific version of python against a series of models

    Description:

    Notes:

    History:

        170903 ksl Bagan work

    '''

    date=time.strftime("%y%m%d", time.gmtime())

    if out_dir=='':
        out_dir='%s_%s' % (version,date)

    if os.path.exists(out_dir)==False:
        os.mkdir(out_dir)

    print(date)

    # Get the PYTHON environment variable

    PYTHON=os.environ['PYTHON']
    print(PYTHON)

    if pf_dir=='':
        pf_files=glob(PYTHON+'/examples/regress/*pf')
    else:
        pf_files=glob(pf_dir+'/*pf')

    print(pf_files)


    commands=[]
    root_names=[]
    for one in pf_files:
        shutil.copy(one,out_dir)
        words=one.split('/')
        pf=(words[len(words)-1])
        root_name=pf.replace('.pf','')
        if np<=1:
            command='%s %s' % (version,pf)
        else:
            command='mpirun -np %d %s %s >%s.stdout.txt' % (np,version,pf,root_name)
        commands.append(command)

        root_names.append(root_name)

    print(commands)

    # Switch to the work directory
    cwd=os.getcwd()
    os.chdir(out_dir)

    proc=subprocess.Popen('Setup_Py_Dir',shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)

    f=open(outputfile,'w')

    i=0
    while i<len(commands):
        one=commands[i]
        string='Running %s' % one
        print(string)
        f.write('%s\n'% string)

        proc=subprocess.Popen(one,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        stdout,stderr=proc.communicate()
        if len(stderr):
            string='Stderrs were reported!!'
            f.write('%s\n'% string)
            print(string)
            print(stderr)
            stderr=str(stderr)
            stderr.replace('\\n','\n')
            g=open(root_names[i]+'.stderr.txt','w')
            g.write(stderr)
            g.close()
        else:
            string='No stderrs were reported'
            print(string)
            f.write('%s\n' % string)
            check_one(f,root_names[i])

        string='Finished %s' % one
        print(string)
        f.write('%s\n\n'% string)
        i+=1


    f.close()
    # Return to the place where the code was made from
    os.chdir(cwd)





    return


def steer(argv):
    '''
    This is just a steering routine so that switches can be processed 
    from the command line
    '''
    pf_dir=''
    out_dir=''
    np=3

    i=1
    words=[]
    while i<len(argv):
        if argv[i]=='-h':
            print(__doc__)
            return
        elif argv[i]=='-np':
            i=i+1
            np=int(argv[i])
        elif argv[i]=='-pf_dir':
            i=i+1
            pf_dir=(argv[i])
        elif argv[i]=='-out_dir':
            i=i+1
            out_dir=(argv[i])
        elif argv[i][0]=='-':
            print('Error: Unknown switch ---  %s' % argv[i])
            return
        else:
            words.append(argv[i])
        i+=1

    if(len(words)==0):
        print('Error: Consumed of command line without a python executable')
        return

    for one in words:
        doit(version=one,pf_dir=pf_dir,out_dir=out_dir,np=np,outputfile='Summary.txt')







# Next lines permit one to run the routine from the command line
if __name__ == "__main__":
    import sys
    if len(sys.argv)>1:
        steer(sys.argv)
    else:
        print(__doc__)
