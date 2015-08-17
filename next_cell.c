#include "next_cell.h"
#include <math.h>

// SUBROUTINE find_next_cell(cell, face_inlet, inlet, dir, next_cell, next_face, outlet, length, t_outlet, co2_outlet, h2o_outlet,&
//     ierr) 
// ! 
// USE geometrie, ONLY : xyz, cell_to_faces, cell_to_points &
// 					, face_to_points, face_to_cells, face_merged &
//                     , maximum_length, cell_volume &
//                     , face_patch_type, face_periodic_connection &
//                     , num_patch_periodic_1, num_patch_periodic_2 &
//                     , angle_patch_periodic_1, angle_patch_periodic_2 &
//                     , nb_faces
// USE  physical_values, ONLY : point_temperature, point_xco2, point_xh2o
// USE control_parameters, ONLY : distance_impact_min
// !

// INTEGER, INTENT(INOUT)  :: cell, face_inlet
// INTEGER, INTENT(OUT) :: next_cell, next_face, ierr
// DOUBLE PRECISION, INTENT(OUT)   :: length
// DOUBLE PRECISION, INTENT(OUT)   :: t_outlet, co2_outlet, h2o_outlet
// DOUBLE PRECISION, INTENT(INOUT),    DIMENSION(3) :: inlet
// DOUBLE PRECISION, INTENT(OUT),   DIMENSION(3) :: outlet
// DOUBLE PRECISION, INTENT(INOUT), DIMENSION(3) :: dir
// !
// INTEGER :: code, ioerror
// DOUBLE PRECISION, DIMENSION(3) :: a,b,c,d,f, inlet_first, new_dir, tempo1, tempo2
// DOUBLE PRECISION :: det, pa, pb, pc, pd, sum_p, norma, normb
// LOGICAL :: ok, merged
// !
// INTEGER :: n_face, face
inline double p_cal(t_vector a, t_vector b, t_vector c)
{
	return ( a.z * ( b.x*c.y - c.x*b.y ) 
		+ b.z * ( c.x*a.y - a.x*c.y ) 
		+ c.z * ( a.x*b.y - b.x*a.y ));
}

void find_next_cell(int *cell, int *face_inlet, t_vector *inlet, t_vector *dir, 
		int *next_cell, int *next_face, t_vector *outlet, double *length, double *t_outlet, double *co2_outlet, double *h2o_outlet,
		int *ierr)
{
	t_mesh *mesh = get_mesh();
	t_vector a,b,c,d,f, inlet_first;
	double det, pa, pb, pc, pd, sum_p;
	int ok,merged,i_face,face;
	*ierr = 1;
	ok = 0;
	*next_cell = 0;
	*next_face = -1;
	inlet_first = *inlet;
	while(1)
	{
		f.x = inlet->x + mesh->max_length /10 * dir->x;
		f.y = inlet->y + mesh->max_length /10 * dir->y;
		f.z = inlet->z + mesh->max_length /10 * dir->z;
		for(i_face = 0;i_face<4;i_face++)
		{
			face = mesh->cell_to_faces[*cell][i_face];
			if(face == *face_inlet) 
				continue;
			a.x = f.x - mesh->points[mesh->faces[face].p1].x;
			a.y = f.y - mesh->points[mesh->faces[face].p1].y;
			a.z = f.z - mesh->points[mesh->faces[face].p1].z;

			b.x = f.x - mesh->points[mesh->faces[face].p2].x;
			b.y = f.y - mesh->points[mesh->faces[face].p2].y;
			b.z = f.z - mesh->points[mesh->faces[face].p2].z;

			c.x = f.x - mesh->points[mesh->faces[face].p3].x;
			c.y = f.y - mesh->points[mesh->faces[face].p3].y;
			c.z = f.z - mesh->points[mesh->faces[face].p3].z;

			d.x = f.x - inlet->x;
			d.y = f.x - inlet->y;
			d.z = f.x - inlet->z;
			det = 1.0 / (                                    
				( c.x * ( - b.y + d.y )           
				+ b.x * ( - d.y + c.y )           
				+ d.x * ( - c.y + b.y ) ) * a.z  
				+                                    
				( c.x * ( - d.y + a.y )           
				+ a.x * ( - c.y + d.y )           
				+ d.x * ( - a.y + c.y ) ) * b.y  
				+                                     
				( b.x * ( - a.y + d.y )           
				+ a.x * ( - d.y + b.y )           
				+ d.x * ( - b.y + a.y ) ) * c.y
				+                                                      
				( a.x * ( - b.y + c.y )           
				+ b.x * ( - c.y + a.y )           
				+ c.x * ( - a.y + b.y ) ) * d.z  
				);
			pd = p_cal(a, b, c) * det;
			if(pd < 0)
			{
				pa = p_cal(b, c, d) * det;
				if(pa < 0) 
					continue;
				pb = p_cal(a, c, d) * det;
				if(pb < 0)
					continue;
				pc = p_cal(a, b, d) * det;
				if(pc < 0 )
					continue;
				sum_p = pa + pb +pc;
				pa = max(pa / sum_p, mesh->distance_impact_min);
				pb = max(pb / sum_p, mesh->distance_impact_min);
				pc = max(pc / sum_p, mesh->distance_impact_min);
				sum_p = 1.0 / (pa + pb + pc);

				outlet->x = pa * mesh->points[mesh->faces[face].p1].x + pb * mesh->points[mesh->faces[face].p2].x + pc * mesh->points[mesh->faces[face].p3].x;
				outlet->y = pa * mesh->points[mesh->faces[face].p1].y + pb * mesh->points[mesh->faces[face].p2].y + pc * mesh->points[mesh->faces[face].p3].y;
				outlet->z = pa * mesh->points[mesh->faces[face].p1].z + pb * mesh->points[mesh->faces[face].p2].z + pc * mesh->points[mesh->faces[face].p3].z;
				if(mesh->faces[face].to_cell[0] == *cell)
				{
					*next_cell = mesh->faces[face].to_cell[1];
				}
				else
				{
					*next_cell = mesh->faces[face].to_cell[0];
				}
				*next_face = face;
				if(*next_cell == 0)
					ok = 1;
				// need do something about face merge!				
			}        	
		} //end for
		//still not comlete
		if(ok || merged)
			break;
		*inlet = *outlet;
		*cell = *next_cell;
		*face_inlet = *next_face;
		merged = 0;
	} // end while
	*t_outlet = sum_p * ( pa * mesh->points[mesh->faces[*next_face].p1].temperature +  pb * mesh->points[mesh->faces[*next_face].p2].temperature + pc * mesh->points[mesh->faces[*next_face].p3].temperature);
	*co2_outlet = sum_p * ( pa * mesh->points[mesh->faces[*next_face].p1].co2 +  pb * mesh->points[mesh->faces[*next_face].p2].co2 + pc * mesh->points[mesh->faces[*next_face].p3].co2);
	*h2o_outlet = sum_p * ( pa * mesh->points[mesh->faces[*next_face].p1].h2o +  pb * mesh->points[mesh->faces[*next_face].p2].h2o + pc * mesh->points[mesh->faces[*next_face].p3].h2o);
	*ierr = 1;
	*length = sqrt(pow(inlet_first.x - inlet->x,2) + pow(inlet_first.y - inlet->y,2) + pow(inlet_first.z - inlet->z,2));
	if(*next_cell != 0 )
		return ;
}