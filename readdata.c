#include "readdata.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
void read_from_vtk(t_mesh *meshdata)
{
	FILE *fp;
	unsigned int i;
	static char buffer[1024];
	t_cell *cells;
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
			sscanf(buffer, "POINTS%u",&meshdata->nb_points);
			printf("POINTS LINE FOUND! nb_points:%u\n",meshdata->nb_points);
			meshdata->points = (t_point *)malloc(sizeof(t_point)*meshdata->nb_points);
			memset(meshdata->points, 0, sizeof(t_point)*meshdata->nb_points);
			for(i=0;i<meshdata->nb_points;i++)
			{
				fgets(buffer, 1024, fp);
				sscanf(buffer, "%lf%lf%lf", &(meshdata->points[i].x), 
					&(meshdata->points[i].y),
					&(meshdata->points[i].z));
#ifdef DEBUG
				 printf("%lf %lf %lf\n",meshdata->points[i].x,
				 	meshdata->points[i].y,
					meshdata->points[i].z);
#endif
			}
		}
		else if(strncmp("CELLS", buffer, 5) == 0)
		{
			int nb_cells;
			sscanf(buffer,"CELLS%u%*u",&nb_cells);
			printf("CELLS LINE FOUND! nb_cells:%u\n",nb_cells);
			cells = (t_cell *)malloc(sizeof(t_cell)*nb_cells);
			memset(cells, 0, sizeof(t_cell) * nb_cells);
			meshdata->nb_faces = 0;
			meshdata->nb_cells = 0;
			unsigned int type;
			for(i = 0; i < nb_cells; i++)
			{
				fgets(buffer, 1024, fp);
				sscanf(buffer,"%u",&type);
				// 3 means triangle
				if(type == 3)
				{
					meshdata->nb_faces++;
					sscanf(buffer, "%*u%u%u%u", &(cells[i].p1),
						&(cells[i].p2),
						&(cells[i].p3));
				}
				// 4 means tetra
				else if(type == 4)
				{
					meshdata->nb_cells++;
					sscanf(buffer, "%*u%u%u%u%u", &cells[i].p1,
						&(cells[i].p2),
						&(cells[i].p3),
						&(cells[i].p4));				
				}
			}
		}
	}
	// split cells data to faces and cells
	meshdata->faces = (t_face *)malloc(sizeof(t_face)*meshdata->nb_faces);
	meshdata->cells = (t_cell *)malloc(sizeof(t_cell)*meshdata->nb_cells);
	memset(meshdata->faces, 0, sizeof(t_face)*meshdata->nb_faces);
	memset(meshdata->cells, 0, sizeof(t_cell)*meshdata->nb_cells);
	printf("boundary nb_faces:%u\n",meshdata->nb_faces);
	printf("cells:%u\n",meshdata->nb_cells);
	for(i=0; i < meshdata->nb_faces; i++)
	{
		meshdata->faces[i].p1 = cells[i].p1;
		meshdata->faces[i].p2 = cells[i].p2;
		meshdata->faces[i].p3 = cells[i].p3;
#ifdef DEBUG
		printf("%u %u %u\n",meshdata->faces[i].p1, meshdata->faces[i].p2,meshdata->faces[i].p3);
#endif
	}
	for(i=0; i < meshdata->nb_cells; i++)
	{
		meshdata->cells[i] = cells[i+meshdata->nb_faces];
#ifdef DEBUG					
		printf("%u %u %u %u\n",meshdata->cells[i].p1,meshdata->cells[i].p2,meshdata->cells[i].p3, meshdata->cells[i].p4);
#endif	
	}
	free(cells);
}

void destroy_mesh(t_mesh *mesh)
{
	if(mesh->points != NULL)
		free(mesh->points);
	if(mesh->faces != NULL)
		free(mesh->faces);
	if(mesh->cells != NULL)
		free(mesh->cells);
}