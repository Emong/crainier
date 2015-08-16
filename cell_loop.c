#include "cell_loop.h"
#include "calfunc.h"
#include "ck_model.h"
void cell_loop(int point_begin, int point_end)
{
	t_mesh *mesh = get_mesh();
	unsigned int i,nb_simul,simul;
	unsigned int point_id;
	int tir,cell,face,nu;
	double energy_per_realisation;
	double t_inlet, co2_inlet, h2o_inlet, t_outlet, co2_outlet, h2o_outlet, t_oli, temperature;
	double v_x,v_y,v_z;
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
			// tir = 0
   //  		energy_inter(:) = 0.d0
			while(1)
			{
				energy_per_realisation = 0;
				cell = mesh->points[i].to_cell;
				if(1 == simul)
				{
					t_inlet = mesh->points[point_id].temperature;
					co2_inlet = mesh->points[point_id].co2;
					h2o_inlet = mesh->points[point_id].h2o;
					determine_direction(&v_x, &v_y, &v_z);
					nu = get_nu(point_id);

				}
				else
				{
					//need to be inmpliment
				}
			}

		}
	}
}