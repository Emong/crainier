#include "structure.h"
#include "readdata.h"
#include <stdlib.h>
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
	destroy_mesh(&mesh);
	return 0;
}
