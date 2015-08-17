#include "cell_loop.h"
#include "calfunc.h"
#include "ck_model.h"
#include <stdio.h>
#include <math.h>
#include "next_cell.h"
void cell_loop(int point_begin, int point_end)
{
	t_mesh *mesh = get_mesh();
	unsigned int i,nb_simul,simul;
	int next_cell;
	unsigned int point_id;
	int tir,cell,face,nu;
	int face_impact, face_impact_old;
	double transmission, absorption;
	double energy_per_realisation;
	double t_inlet, co2_inlet, h2o_inlet, t_outlet, co2_outlet, h2o_outlet, t_oli, temperature, co2, h2o, temperature_wall;
	double v_x,v_y,v_z;
	double distance, kappa, le, le_at_point, kappa_wall, le_wall, epsilon;
	t_vector point_in, point_out,dir;
	int first,ierr;
	int point_to_check = -25201;
	for(i=point_begin;i < point_end; i++)
	{
		nb_simul = 1;
		point_id = mesh->point_to_compute[i];
		//if a point is boundary ,should compute twice
		// one for the domain 
		// one for the wall type
		if(mesh->points[point_id].is_boundary)
			nb_simul = 2;
		for(simul = 0; simul < nb_simul;simul++)
		{
			if(1 == simul && mesh->points[point_id].emission < 0.5)
				break;
			if(2 == simul && mesh->points[point_id].emission_surface < 0.5)
				break;
			tir = 0;
			double energy_inter[NB_BAND] = {0};
			// tir = 0
   //  		energy_inter(:) = 0.d0
			while(1)
			{
				energy_per_realisation = 0;
				cell = mesh->points[point_id].to_cell;
				if(0 == simul)
				{
					t_inlet = mesh->points[point_id].temperature;
					co2_inlet = mesh->points[point_id].co2;
					h2o_inlet = mesh->points[point_id].h2o;
					determine_direction(&v_x, &v_y, &v_z);
					nu = get_nu(point_id);
					printf("v_x:%f ,v_y:%f, v_z:%f,nu:%d\n",v_x,v_y,v_z,nu);
					//printf("%f\n",sqrt(pow(v_x,2)+pow(v_y,2)+pow(v_z,2)));

				}
				else
				{
					first = 1;
					t_inlet = mesh->points[point_id].temperature;
					co2_inlet = mesh->points[point_id].co2;
					h2o_inlet = mesh->points[point_id].h2o;
					first = 1;
					//point_to_face_boundary
					face = mesh->points[point_id].to_face;
					nu = get_nu_face(point_id);
					epsilon = mesh->ckmodel.epsilon;
					compute_dir(face, &v_x, &v_y, &v_z, &epsilon, first);
					first = 0;
				}
				// 0.99D0 * xyz(:, point) &
				// 	+ 0.01D0/4.0D0 * xyz(:,cell_to_points(1,cell)) &
				// 	+ 0.01D0/4.0D0 * xyz(:,cell_to_points(2,cell)) &
				// 	+ 0.01D0/4.0D0 * xyz(:,cell_to_points(3,cell)) &
				// 	+ 0.01D0/4.0D0 * xyz(:,cell_to_points(4,cell))

				point_in.x = 0.99 * mesh->points[point_id].x + 0.01 / 4 * mesh->points[mesh->cells[cell].p1].x +
					0.01 / 4 * mesh->points[mesh->cells[cell].p2].x +
					0.01 / 4 * mesh->points[mesh->cells[cell].p3].x +
					0.01 / 4 * mesh->points[mesh->cells[cell].p4].x;
				point_in.y = 0.99 * mesh->points[point_id].y + 0.01 / 4 * mesh->points[mesh->cells[cell].p1].y +
					0.01 / 4 * mesh->points[mesh->cells[cell].p2].y +
					0.01 / 4 * mesh->points[mesh->cells[cell].p3].y +
					0.01 / 4 * mesh->points[mesh->cells[cell].p4].y;
				point_in.z = 0.99 * mesh->points[point_id].z + 0.01 / 4 * mesh->points[mesh->cells[cell].p1].z +
					0.01 / 4 * mesh->points[mesh->cells[cell].p2].z +
					0.01 / 4 * mesh->points[mesh->cells[cell].p3].z +
					0.01 / 4 * mesh->points[mesh->cells[cell].p4].z;
				tir = tir + 1;
				face_impact = -1;
				face_impact_old = -1;
				transmission = 0;

				compute_kappa_le_simple(nu, t_inlet, co2_inlet, h2o_inlet, &kappa, &le_at_point);
				dir.x = v_x;
				dir.y = v_y;
				dir.z = v_z;
				while(1)
				{

					find_next_cell(&cell, &face_impact_old, &point_in, &dir, &next_cell, &face_impact, &point_out, &distance, &t_outlet, &co2_outlet, &h2o_outlet, &ierr);
					temperature = (t_inlet + t_outlet) / 2.0;
					co2 = (co2_inlet + co2_outlet) / 2.0;
					h2o = (h2o_inlet + h2o_outlet) / 2.0;
					t_inlet = t_outlet;
					co2_inlet = co2_outlet;
					h2o_inlet = h2o_outlet;
					if(next_cell != 0 )
					{
						if(i == point_to_check) fprintf(stderr, "point standard \n");
						compute_kappa_le_simple(nu, temperature, co2, h2o, &kappa, &le);
						absorption =  1.0 - exp(-kappa * distance);
						energy_per_realisation += transmission + absorption * (le  / le_at_point - 1);
						transmission = transmission * (1.0 - absorption);
						if(transmission <= mesh->transmission_stop)
							break;
						cell = next_cell;
					}
					else
					{
						if(i == point_to_check) fprintf(stderr, "point standard \n");
						compute_kappa_le_simple(nu, temperature, co2, h2o, &kappa, &le);
						absorption =  1.0 - exp(-kappa * distance);
						energy_per_realisation += transmission + absorption * (le  / le_at_point - 1);
						transmission = transmission * (1.0 - absorption);
						if(transmission <= mesh->transmission_stop)
							break;
						epsilon = mesh->ckmodel.epsilon;
						compute_dir(face_impact, &v_x, &v_y, &v_z, &epsilon, first);
						dir.x = v_x;
						dir.y = v_y;
						dir.z = v_z;
						//temperature_wall = face_boundary_temperature(face_impact);
						temperature_wall = mesh->faces[face_impact].p1;  //have problem
						compute_kappa_le_simple(nu, temperature_wall, co2, h2o, &kappa, &le_wall);
						absorption = epsilon;
						energy_per_realisation += transmission * absorption * ( le_wall / le_at_point - 1);
						transmission = transmission * (1.0 - epsilon);
						if(transmission <= mesh->transmission_stop)
							break;
					}
					point_in = point_out;
					face_impact_old = face_impact;
				}
			}

		}
	}
}