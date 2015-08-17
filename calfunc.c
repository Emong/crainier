#include <stdio.h>
#include "calfunc.h"
#include <math.h>
#include <stdlib.h>
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
	//printf("vx:%f\n",*v_x);
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

void compute_dir(int face, double *dir_x, double *dir_y, double *dir_z, double *epsilon, int first)
{
	t_mesh *mesh = get_mesh();
	double u_second_1, u_second_2, u_second_3;
	double n_x_1 , n_y_1 , n_z_1;
	double n_x_2 , n_y_2 , n_z_2;
	double norm1, norm2;
	double M_1_x, M_1_y, M_1_z;
	double scalaire2;
	double cos_theta, theta;
	double dx,dy,dz;
	int speculaire,cell;
	int table_theta[181];
	cell = mesh->faces[face].to_cell[0];
	dx = *dir_x;
	dy = *dir_y;
	dz = *dir_z;
	speculaire = 1;
	if(speculaire && !first)
	{
		//cos_theta = abs(dir_x * normal_face_x(face) + dir_y * normal_face_y(face) + dir_z * normal_face_z(face))
		cos_theta = fabs(dx * mesh->faces[face].normal_vector_x + dy * mesh->faces[face].normal_vector_y + dz * mesh->faces[face].normal_vector_z);
		*dir_x = dx + 2 * cos_theta * mesh->faces[face].normal_vector_x;
		*dir_y = dy + 2 * cos_theta * mesh->faces[face].normal_vector_y;
		*dir_z = dz + 2 * cos_theta * mesh->faces[face].normal_vector_z;

		*epsilon = *epsilon * 1.5 * cos_theta;

		cos_theta = min(1.0,fabs(cos_theta));

		theta = acos(cos_theta) * 45 + atan(1.0);
		if(theta > 90)
		{
			fprintf(stderr, "in compute_dir theta :%f\n bigger than 90",theta);
			exit(1);
		}
		// table_theta(INT(acos(cos_theta) * 180.0d0 / 3.14159d0 +1)) = table_theta(INT(acos(cos_theta) * 180.0d0 / 3.14159d0 +1)) + 1
		table_theta[(int)(acos(cos_theta) * 180.0 / PI)] = table_theta[(int)(acos(cos_theta) * 180.0 / PI)];
	}
	else
	{
		M_1_x  = mesh->faces[face].normal_vector_x + 1;
		M_1_y  = mesh->faces[face].normal_vector_y;
		M_1_z  = mesh->faces[face].normal_vector_z;
		scalaire2 = mesh->faces[face].normal_vector_x * M_1_x + mesh->faces[face].normal_vector_y  * M_1_y  + mesh->faces[face].normal_vector_z  * M_1_z;
		if(0 == mesh->faces[face].normal_vector_y && 0 == mesh->faces[face].normal_vector_z)
		{
			n_x_1 = 0;
			n_y_1 = 1;
			n_z_1 = 0;

			n_x_2 = 0;
			n_y_2 = 0;
			n_z_2 = mesh->faces[face].normal_vector_x;
		}
		else
		{
			n_x_1 = M_1_x - scalaire2 * mesh->faces[face].normal_vector_x;
			n_y_1 = M_1_y - scalaire2 * mesh->faces[face].normal_vector_y;
			n_z_1 = M_1_z - scalaire2 * mesh->faces[face].normal_vector_z;

			norm1 = sqrt(n_x_1*n_x_1 + n_y_1*n_y_1 + n_z_1*n_z_1);
			n_x_1 /= norm1;
			n_y_1 /= norm1;
			n_z_1 /= norm1;

			n_x_2 = mesh->faces[face].normal_vector_y*n_z_1 - mesh->faces[face].normal_vector_z*n_y_1;
			n_y_2 = mesh->faces[face].normal_vector_z*n_x_1 - mesh->faces[face].normal_vector_x*n_z_1;
			n_z_2 = mesh->faces[face].normal_vector_x*n_y_1 - mesh->faces[face].normal_vector_y*n_x_1;

			norm2 = sqrt(n_x_2*n_x_2 + n_y_2*n_y_2 + n_z_2*n_z_2);
			n_x_2 /= norm2;
			n_y_2 /= norm2;
			n_z_2 /= norm2;
		}
		local_coordinate_vector_diffusion(&u_second_1, &u_second_2, &u_second_3);
		*dir_x = u_second_1 * n_x_1 + u_second_2 * n_x_2 + u_second_3 * mesh->faces[face].normal_vector_x; 
		*dir_y = u_second_1 * n_y_1 + u_second_2 * n_y_2 + u_second_3 * mesh->faces[face].normal_vector_y;
		*dir_z = u_second_1 * n_z_1 + u_second_2 * n_z_2 + u_second_3 * mesh->faces[face].normal_vector_z;
	}

}
