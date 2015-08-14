#include "structure.h"
#include "readdata.h"
#include <stdlib.h>
#include <stdio.h>
#include "calfunc.h"
#include "ck_model.h"
void init()
{
	get_mesh()->nb_points = 0;
	get_mesh()->nb_faces = 0;
	get_mesh()->nb_cells = 0;
	get_mesh()->nb_boundary_points = 0;
	get_mesh()->points = NULL;
	get_mesh()->faces = NULL;
	get_mesh()->cells = NULL;

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
	printf("ck_model:%.16g\n",get_mesh()->ckmodel.kstarh[0][0][0][0]);
	write_vtk(get_mesh(), "./out.vtk");
	destroy_mesh(get_mesh());
	// double x=1.123456789;
	// scanf("%lf",&x);
	// printf("%d -- %.16g\n",sizeof(long double),x);
	return 0;
}
