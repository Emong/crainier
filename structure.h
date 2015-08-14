#ifndef _H_STRUCTURE_
#define _H_STRUCTURE_

enum cell_type 
{
	VERTEX=1,
	POLY_VERTEX,
	LINE,
	POLY_LINE,
	TRIANGLE,
	TRIANGLE_STRIP,
	POLYGON,
	PIXEL,
	QUAD,
	TETRA
};
typedef struct _t_point
{
	char is_boundary;
	double x,y,z;
}t_point;

typedef struct _t_face
{
	unsigned int p1,p2,p3;
}t_face;

typedef struct _t_cell
{
//	unsigned int type;
	enum cell_type type;
	unsigned int p1,p2,p3,p4;
}t_cell;

typedef struct _t_mesh
{
	t_point *points;
	t_face *faces;
	t_cell *cells;
	unsigned int nb_points,nb_faces,nb_cells;
	unsigned int nb_boundary_points;
}t_mesh;
#endif
