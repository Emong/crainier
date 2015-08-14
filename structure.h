#ifndef _H_STRUCTURE_
#define _H_STRUCTURE_
#define nb_gauss_CK 7
#define nb_temp_CK  16
#define nb_xh2o_CK  5
#define nb_nuh2o_CK 44
#define nb_nuco2_CK 17
#define full_nb_gauss 1100

#define PI 3.1415926535897932384626433832795028841971693993751
#define FOUR_PI PI*4
#define constant_stefan 5.670E-8

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
typedef struct _t_ck_model
{
	int nb_band;
	double weight_CK[nb_gauss_CK];
	double weight_guass[full_nb_gauss];
	int num_gauss[full_nb_gauss],num_gauss2[full_nb_gauss],small_to_big[full_nb_gauss];
	int h2o_to_co2[nb_nuh2o_CK];
	double temp_pmg[nb_temp_CK];
	double xh2o_CK[nb_xh2o_CK];	//**
	double nuc[nb_nuh2o_CK],deltanu[nb_nuh2o_CK];
	double kstarh[nb_temp_CK][nb_xh2o_CK][nb_nuh2o_CK][nb_gauss_CK];	//**
	double kstarc[nb_temp_CK][nb_nuco2_CK][nb_gauss_CK];
	double epsilon; //no need

}t_ck_model;

typedef struct _t_point
{
	char is_boundary;
	double x,y,z;
	double temperature, co2, h2o, emission;
}t_point;

typedef struct _t_face
{
	unsigned int p1,p2,p3;
	double normal_vector_x;
	double normal_vector_y;
	double normal_vector_z;
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
	t_ck_model ckmodel;
	unsigned int nb_points,nb_faces,nb_cells;
	unsigned int nb_boundary_points;
}t_mesh;
#endif
