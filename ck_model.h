#ifndef _H_CK_MODEL_
#define _H_CK_MODEL_
void read_param_CK();
void interpolation_coef_CK(double T, double xh2o, int *iTCK, int *ixCK, double *ciT1, double *ciT2, double *cix1, double *cix2);
double fQC(double T);
double fQH(double T);
void compute_index(double t, double h2o, int *i_t_le, int *i_t_kappa, int *i_t_fQ, int *i_h2o);
int get_nu(int point);
int get_nu_face(int point);
#endif
