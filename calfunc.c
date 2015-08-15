#include <stdio.h>
#include "calfunc.h"
#include <math.h>
#include "nrand.h"
#include "structure.h"
t_mesh *get_mesh()
{
	//static vars only initialize on the compling time.
	static t_mesh only_mesh = {
	.nb_points = 0,
	.nb_faces = 0,
	.nb_cells = 0,
	.nb_boundary_points = 0,
	.points = NULL,
	.faces = NULL,
	.cells = NULL,
	};
	return(&only_mesh);
}

inline void debug(const char * format,...)
{
#ifdef DEBUG
	va_list args;
 
 	va_start(args,format);
 	vprintf(format,args);
 	va_end(args);
#endif
}

void local_coordinate_vector_diffusion(double *u_second_1, double *u_second_2, double *u_second_3)
{
	double theta_d, phi_d;
	phi_d = 2.0 * PI * nrand();
	theta_d = acos(sqrt(nrand()));
	*u_second_1 = sin(theta_d) * cos(phi_d);
	*u_second_2 = sin(theta_d) * sin(phi_d);
	*u_second_3 = cos(theta_d);
}
