#include <stdio.h>
#include "calfunc.h"
#include <math.h>
#include "nrand.h"
#include "structure.h"
#include "ck_model.h"
#include "readdata.h"
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
void determine_direction(double *v_x, double *v_y, double *v_z)
{

	double cos_theta,sin_theta,phi;
	phi = 2 * PI * nrand();
	cos_theta = 1 - 2 * nrand();
	sin_theta = sqrt(1 - pow(cos_theta,2));
	*v_x = sin_theta * cos(phi);
	*v_y = sin_theta * sin(phi);
	*v_z = cos_theta;
}

void compute_point_emission()
{
	t_mesh *mesh = get_mesh();
	unsigned int i,j,point_id;
	int i_t_le,i_t_kappa,i_t_fQ,i_h2o;
	double kappa,emission=0;
	mesh->max_emission = 0;
	for(i=0;i<mesh->nb_points;i++)
	{
		mesh->points[i].emission = 0;
	}
	for(i=0;i<mesh->nb_point_to_compute;i++)
	{
		point_id = mesh->point_to_compute[i];
		compute_index(mesh->points[point_id].temperature, mesh->points[point_id].h2o, &i_t_le, &i_t_kappa, &i_t_fQ, &i_h2o);
		for(j=0;j<NB_BAND;j++)
		{
			kappa = mesh->point_presure * mesh->points[i].h2o * mesh->ckmodel.fQH_store[i] * mesh->ckmodel.kappa_h2o_store[j][i_t_kappa][i_h2o] + 
				mesh->point_presure * mesh->points[i].co2 * mesh->ckmodel.fQC_store[i] * mesh->ckmodel.kappa_co2_store[j][i_t_kappa];
			emission = emission + kappa*mesh->ckmodel.le_store[j][i_t_le];
		}
		mesh->points[i].emission = FOUR_PI * emission;
		if(mesh->points[i].emission > mesh->max_emission)
			mesh->max_emission = mesh->points[i].emission;
	}
}
void compute_point_emission_surface()
{
	t_mesh *mesh = get_mesh();
	unsigned int i,point_id;
	printf("nb_bouudary_point_to_compute:%d\n",mesh->nb_boundary_point_to_compute);
	for(i=0;i<mesh->nb_boundary_point_to_compute;i++)
	{
		point_id = mesh->boundary_point_to_compute[i];
		mesh->points[point_id].emission_surface = mesh->epsilon_paro * constant_stefan * pow(mesh->points[point_id].temperature,4);
	}
}
