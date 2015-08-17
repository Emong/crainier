#ifndef _H_NEXT_CELL_
#define _H_NEXT_CELL_
#include "calfunc.h"

void find_next_cell(int *cell, int *face_inlet, t_vector *inlet, t_vector *dir, 
		int *next_cell, int *next_face, t_vector *outlet, double *length, double *t_outlet, double *co2_outlet, double *h2o_outlet,
		int *ierr);

#endif
