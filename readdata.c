#include "readdata.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
void read_from_vtk(t_point *points,t_face *faces,t_cell *cells)
{
	FILE *fp;
	unsigned int i;
	static char buffer[1024];
	t_mesh meshdata;
	fp = fopen(VTK_MESH_PATH, "r");
	if(fp == NULL)
	{
		perror(VTK_MESH_PATH" open Error");
		exit(1);
	}
	//read points
	while(fgets(buffer, 1024, fp))
	{
		if(strncmp("POINTS",buffer, 6) == 0)
		{
			sscanf(buffer, "POINTS%u",&meshdata.nb_points);
			printf("POINTS LINE FOUND! nb_points:%u\n",meshdata.nb_points);
			meshdata.points = (t_point *)malloc(sizeof(t_point)*meshdata.nb_points);
			for(i=0;i<meshdata.nb_points;i++)
			{
				fgets(buffer, 1024, fp);
				sscanf(buffer, "%lf%lf%lf", &(meshdata.points[i].x), 
					&(meshdata.points[i].y),
					&(meshdata.points[i].z));
#ifdef DEBUG
				 printf("%lf %lf %lf\n",meshdata.points[i].x,
				 	meshdata.points[i].y,
					meshdata.points[i].z);
#endif
			}
		}
		else if(strncmp("CELLS", buffer, 5) == 0)
		{
			sscanf(buffer,"CELLS%u%*u",&meshdata.nb_cells);
			printf("CELLS LINE FOUND! nb_cells:%u\n",meshdata.nb_cells);
			//meshdata.faces = (t_face *)malloc(sizeof(t_face)*meshdata.nb_faces);
			meshdata.cells = (t_cell *)malloc(sizeof(t_cell)*meshdata.nb_cells);
			for(i=0; i<meshdata.nb_cells; i++)
			{
				fgets(buffer, 1024, fp);
				sscanf(buffer,"%u",&meshdata.cells[i].type);
				// 3 means triangle
				if(meshdata.cells[i].type == 3)
				{
					sscanf(buffer, "%*u%u%u%u", &(meshdata.cells[i].p1),
						&(meshdata.cells[i].p2),
						&(meshdata.cells[i].p3));
#ifdef DEBUG
					printf("%u %u %u %u\n",meshdata.cells[i].type, meshdata.cells[i].p1,meshdata.cells[i].p2,meshdata.cells[i].p3);
#endif
				}
				// 4 means tetra
				else if(meshdata.cells[i].type == 4)
				{
					sscanf(buffer, "%*u%u%u%u%u", &meshdata.cells[i].p1,
						&(meshdata.cells[i].p2),
						&(meshdata.cells[i].p3),
						&(meshdata.cells[i].p4));
#ifdef DEBUG					
					printf("%u %u %u %u %u\n",meshdata.cells[i].type,meshdata.cells[i].p1,meshdata.cells[i].p2,meshdata.cells[i].p3, meshdata.cells[i].p4);
#endif					
				}
			}
		}
	}
}