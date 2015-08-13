#ifndef _H_STRUCTURE_
#define _H_STRUCTURE_
typedef struct _t_point
{
	int x,y,z;
}t_point;

typedef struct _t_triangle
{
	int p1,p2,p3;
}t_triangle;

typedef struct _t_tetrahedron
{
	int p1,p2,p3,p4;
}t_tetrahedron;
#endif