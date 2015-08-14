#include "structure.h"
#include "readdata.h"
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char const *argv[])
{
	t_mesh mesh = {
		.nb_points = 0,
		.nb_faces = 0,
		.nb_cells = 0,
		.points = NULL,
		.faces = NULL,
		.cells = NULL,
	};
	read_from_vtk(&mesh);
	//printf("hello\n");
	printf("%u %u %u\n", 
		mesh.faces[0].p1,
		mesh.faces[0].p2,
		mesh.faces[0].p3);
	printf("%u %u %u %u\n", 
		mesh.cells[0].p1,
		mesh.cells[0].p2,
		mesh.cells[0].p3,
		mesh.cells[0].p4);
	printf("points in domain:%d\n",mesh.nb_boundary_points);
	write_vtk(&mesh, "./out.vtk");
	destroy_mesh(&mesh);
	// double x=1.123456789;
	// scanf("%lf",&x);
	// printf("%d -- %.16g\n",sizeof(long double),x);
	return 0;
}
