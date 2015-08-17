#include "structure.h"
#include "readdata.h"
#include <stdlib.h>
#include <stdio.h>
#include "calfunc.h"
#include "ck_model.h"
#include "cell_loop.h"

void set_co2_h2o()
{
	t_mesh *mesh = get_mesh();
	unsigned int i;
	for(i=0;i<mesh->nb_points;i++)
	{
		mesh->points[i].h2o = 0.1;
		mesh->points[i].co2 = 0.1;
		if(mesh->points[i].z < mesh->max_z/2)
			mesh->points[i].temperature = 300;
		else 
			mesh->points[i].temperature = 600;

	}
}
void init()
{
	get_mesh()->func_hooks.set_h2o_co2_hook = set_co2_h2o;
	get_mesh()->epsilon_paro = 0.8;
	get_mesh()->point_presure = 1e5;
	get_mesh()->distance_impact_min = 1e-5;
	get_mesh()->ckmodel.epsilon = 0.8;
	get_mesh()->transmission_stop = 1e-3;
	// get_mesh()->nb_points = 0;
	// get_mesh()->nb_faces = 0;
	// get_mesh()->nb_cells = 0;
	// get_mesh()->nb_boundary_points = 0;
	// get_mesh()->points = NULL;
	// get_mesh()->faces = NULL;
	// get_mesh()->cells = NULL;

}
int main(int argc, char const *argv[])
{
	init();
	read_from_vtk(get_mesh());
	read_param_CK();
	//printf("hello\n");
	printf("%u %u %u\n", 
		get_mesh()->faces[0].p1,
		get_mesh()->faces[0].p2,
		get_mesh()->faces[0].p3);
	printf("%u %u %u %u\n", 
		get_mesh()->cells[0].p1,
		get_mesh()->cells[0].p2,
		get_mesh()->cells[0].p3,
		get_mesh()->cells[0].p4);
	printf("points in domain:%u\n",get_mesh()->nb_boundary_points);
	printf("x_min: %f x_max:%f\n",get_mesh()->min_x,get_mesh()->max_x);
	printf("y_min: %f y_max:%f\n",get_mesh()->min_y,get_mesh()->max_y);
	printf("z_min: %f z_max:%f\n",get_mesh()->min_z,get_mesh()->max_z);
	printf("max length in mesh:%f\n",get_mesh()->max_length);
	printf("points to compute:%u\n",get_mesh()->nb_point_to_compute);
	printf("ck_model:%.16g\n",get_mesh()->ckmodel.kstarh[0][0][0][0]);
	printf("cell type:%d\n",get_mesh()->cells[0].type);
	printf("points 0 to cells:%d\n",get_mesh()->points[0].to_face);
	//cell_loop(0,200);
	write_vtk(get_mesh(), "./out.vtk");
	destroy_mesh(get_mesh());
	// double x=1.123456789;
	// scanf("%lf",&x);
	// printf("%d -- %.16g\n",sizeof(long double),x);


	return 0;
}
