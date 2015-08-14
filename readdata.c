#include "readdata.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "safe_func.h"
void after_read(t_mesh *);
void read_from_vtk(t_mesh *meshdata)
{
	FILE *fp;
	unsigned int i;
	static char buffer[1024];
	t_cell *cells;
	fp = fopen(VTK_MESH_PATH, "r");
	if(NULL == fp)
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
			meshdata->points = (t_point *)safe_malloc(sizeof(t_point)*meshdata->nb_points);
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
			cells = (t_cell *)safe_malloc(sizeof(t_cell)*nb_cells);
			memset(cells, 0, sizeof(t_cell) * nb_cells);
			meshdata->nb_faces = 0;
			meshdata->nb_cells = 0;
			unsigned int type;
			int nb_type_3_4 = 0;
			for(i = 0; i < nb_cells; i++)
			{
				fgets(buffer, 1024, fp);
				sscanf(buffer,"%u",&type);
				// 3 means triangle
				if(3 == type)
				{
					meshdata->nb_faces++;
					sscanf(buffer, "%*u%u%u%u", &(cells[nb_type_3_4].p1),
						&(cells[nb_type_3_4].p2),
						&(cells[nb_type_3_4].p3));
					nb_type_3_4++;
				}
				// 4 means tetra
				else if(4 == type)
				{
					meshdata->nb_cells++;
					sscanf(buffer, "%*u%u%u%u%u", &cells[nb_type_3_4].p1,
						&(cells[nb_type_3_4].p2),
						&(cells[nb_type_3_4].p3),
						&(cells[nb_type_3_4].p4));
					nb_type_3_4++;			
				}
			}
		}
	}
	fclose(fp);
	// split cells data to faces and cells
	meshdata->faces = (t_face *)safe_malloc(sizeof(t_face)*meshdata->nb_faces);
	meshdata->cells = (t_cell *)safe_malloc(sizeof(t_cell)*meshdata->nb_cells);
	memset(meshdata->faces, 0, sizeof(t_face)*meshdata->nb_faces);
	memset(meshdata->cells, 0, sizeof(t_cell)*meshdata->nb_cells);
	printf("boundary nb_faces:%u\n",meshdata->nb_faces);
	printf("cells:%u\n",meshdata->nb_cells);
	for(i=0; i < meshdata->nb_faces; i++)
	{
		meshdata->faces[i].p1 = cells[i].p1;
		meshdata->faces[i].p2 = cells[i].p2;
		meshdata->faces[i].p3 = cells[i].p3;
		meshdata->points[cells[i].p1].is_boundary = 1;
		meshdata->points[cells[i].p2].is_boundary = 1;
		meshdata->points[cells[i].p3].is_boundary = 1;
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
	safe_free(cells);
	after_read(meshdata);
}

void write_vtk(t_mesh *mesh, const char filename[])
{
	FILE *fp;
	fp = fopen(filename, "w");
	if(NULL == fp)
	{
		perror(VTK_MESH_PATH" open Error");
		exit(1);
	}
	fprintf(fp, "# vtk DataFile Version 2.0\n");
	fprintf(fp, "rainier, Created by rainier\n");
	fprintf(fp, "ASCII\n");
	fprintf(fp, "DATASET UNSTRUCTURED_GRID\n");
	fprintf(fp, "POINTS %u double\n",mesh->nb_points);
	unsigned int i;
	for(i = 0; i<mesh->nb_points;i++)
	{
		fprintf(fp, "%.16g %.16g %.16g\n",mesh->points[i].x,
			mesh->points[i].y,
			mesh->points[i].z);
	}
	fprintf(fp, "\n");
	fprintf(fp, "CELLS %u %u\n",mesh->nb_cells,mesh->nb_cells*5);
	for(i=0;i<mesh->nb_cells;i++)
	{
		fprintf(fp, "4 %u %u %u %u\n",mesh->cells[i].p1,
			mesh->cells[i].p2,
			mesh->cells[i].p3,
			mesh->cells[i].p4);
	}
	fprintf(fp, "\n");
	fprintf(fp, "CELL_TYPES %u\n",mesh->nb_cells);
	for(i=0;i<mesh->nb_cells;i++)
	{
		fprintf(fp, "%d\n",10);
	}

	fclose(fp);
}

void after_read(t_mesh *mesh)
{
	int i;
	for(i=0;i<mesh->nb_points;i++)
	{
		if(mesh->points[i].is_boundary)
			mesh->nb_boundary_points++;
	}
}
void destroy_mesh(t_mesh *mesh)
{
		safe_free(mesh->points);
		safe_free(mesh->faces);
		safe_free(mesh->cells);
}
