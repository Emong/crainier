#ifndef _H_CALFUNC_
#define _H_CALFUNC_
#include "structure.h"
t_mesh *get_mesh();
void debug(const char * format,...);
void local_coordinate_vector_diffusion(double *u_second_1, double *u_second_2, double *u_second_3);
#endif