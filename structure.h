#ifndef _H_STRUCTURE_
#define _H_STRUCTURE_
//for CK MODEL temprary defined here
#define ck_c1 1.191062e-16
#define ck_c2 1.438786314e-2
#define ck_cst 5.670e-8
#define nb_gauss_CK 7
#define nb_temp_CK  16
#define nb_xh2o_CK  5
#define nb_nuh2o_CK 44
#define nb_nuco2_CK 17
#define full_nb_gauss 1100

#define PI 3.1415926535897932384626433832795028841971693993751
#define FOUR_PI PI*4
#define constant_stefan 5.670E-8

#define nb_t_le_store 2600
#define nb_t_le_boundary_store 2600
#define nb_t_kappa_store 575 
#define nb_xh2o_kappa_store 120
#define nb_fQ_store 2800

#define t_min_le_store 250.0
#define t_min_le_boundary_store 250.0
#define delta_t_le_store 1.0
#define delta_t_le_boundary_store 1.0
#define t_min_kappa_store 250.0
#define delta_t_kappa_store 5.0
#define delta_xh2o_kappa_store 0.0015
#define t_fQ_min_store 250.0
#define delta_fQ_store 1.0

#define NB_BAND 1022
//CK MODEL
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
	float le_store[NB_BAND][nb_t_le_store];
	float kappa_h2o_store[NB_BAND][nb_t_kappa_store][nb_xh2o_kappa_store+1];
	float kappa_co2_store[NB_BAND][nb_t_kappa_store];
	float fQH_store[nb_fQ_store],fQC_store[nb_fQ_store];

}t_ck_model;

typedef struct _t_point
{
	char is_boundary;
	char compute_able;
	double x,y,z;
	double temperature, co2, h2o, emission,emission_surface;
	int to_cell;
	int to_face;
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

typedef struct _t_function_hook
{
	void (*set_h2o_co2_hook)();
}t_func_hook;

typedef struct _t_mesh
{
	t_point *points;
	unsigned int *point_to_compute;
	unsigned int *boundary_point_to_compute;
	t_face *faces;
	t_cell *cells;
	t_ck_model ckmodel;
	double point_presure;
	double face_presure;
	double face_point_presure;
	double max_emission;
	double epsilon_paro;
	t_func_hook func_hooks;
	unsigned int nb_points,nb_faces,nb_cells;
	unsigned int nb_boundary_points;
	unsigned int nb_point_to_compute;
	unsigned int nb_boundary_point_to_compute;
	double max_x,max_y,max_z;
	double min_x,min_y,min_z;
	double max_length;
}t_mesh;
#endif
