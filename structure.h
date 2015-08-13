#ifndef _H_STRUCTURE_
#define _H_STRUCTURE_

enum cell_type 
{
	VERTEX,
	POLY_VERTEX,
	LINE,
	POLY_LINE,
	TRIANGLE=5,
	TRIANGLE_STRIP,
	POLYGON,
	PIXEL,
	QUAD,
	TETRA=10
};
typedef struct _t_point
{
	double x,y,z;
}t_point;

typedef struct _t_face
{
	unsigned int p1,p2,p3;
}t_face;

typedef struct _t_cell
{
	unsigned int type;
	unsigned int p1,p2,p3,p4;
}t_cell;

typedef struct _t_mesh
{
	t_point *points;
	t_face *faces;
	t_cell *cells;
	unsigned int nb_points,nb_faces,nb_cells;
}t_mesh;
#endif