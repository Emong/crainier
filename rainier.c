#include "structure.h"
#include "readdata.h"
int main(int argc, char const *argv[])
{
	t_face *faces;
	t_cell *cells;
	t_point *points;
	read_from_vtk(points, faces, cells);
	//printf("hello\n");
	return 0;
}
