/* get_atomicdata.c */
int get_atomic_data(char masterfile[]);
int index_lines(void);
int index_phot_top(void);
int index_inner_cross(void);
void indexx(int n, float arrin[], int indx[]);
int limit_lines(double freqmin, double freqmax);
int check_xsections(void);
double q21(struct lines *line_ptr, double t);
double q12(struct lines *line_ptr, double t);
double a21(struct lines *line_ptr);
double upsilon(int n_coll, double u0);
int fraction(double value, double array[], int npts, int *ival, double *f, int mode);
int linterp(double x, double xarray[], double yarray[], int xdim, double *y, int mode);
