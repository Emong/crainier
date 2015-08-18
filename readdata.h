#ifndef _H_READDATA_
#define _H_READDATA_
#include "structure.h"
#include "config.h"
void read_from_vtk(t_mesh *meshdata);
void write_vtk(t_mesh *mesh, const char filename[]);
void destroy_mesh();
#endif
