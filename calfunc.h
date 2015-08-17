#ifndef _H_CALFUNC_
#define _H_CALFUNC_

#define max(x,y) (__extension__({typeof(x) __X=(x);typeof(y) __Y=(y);__X>__Y?__X:__Y;}))
#define min(x,y) (__extension__({typeof(x) __X=(x);typeof(y) __Y=(y);__X<__Y?__X:__Y;}))

#include "structure.h"
t_mesh *get_mesh();
void debug(const char * format,...);
void local_coordinate_vector_diffusion(double *u_second_1, double *u_second_2, double *u_second_3);
void compute_point_emission();
void compute_point_emission_surface();
void determine_direction(double *v_x, double *v_y, double *v_z);
void compute_dir(int face, double *dir_x, double *dir_y, double *dir_z, double *epsilon, int first);
#endif