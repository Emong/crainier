#include "readdata.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "safe_func.h"
#include "calfunc.h"
#include <math.h>
void after_read(t_mesh *);
void gen_h2o_co2();
void split_cells_to_faces();
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
				if(0 == i)
				{
					meshdata->max_x = meshdata->points[i].x;
					meshdata->max_y = meshdata->points[i].y;
					meshdata->max_z = meshdata->points[i].z;

					meshdata->min_x = meshdata->points[i].x;
					meshdata->min_y = meshdata->points[i].y;
					meshdata->min_z = meshdata->points[i].z;	
				}
				else
				{
					if(meshdata->points[i].x > meshdata->max_x)
						meshdata->max_x = meshdata->points[i].x;
					if(meshdata->points[i].y > meshdata->max_y)
						meshdata->max_y = meshdata->points[i].y;
					if(meshdata->points[i].z > meshdata->max_z)
						meshdata->max_z = meshdata->points[i].z;

					if(meshdata->points[i].x < meshdata->min_x)
						meshdata->min_x = meshdata->points[i].x;
					if(meshdata->points[i].y < meshdata->min_y)
						meshdata->min_y = meshdata->points[i].y;
					if(meshdata->points[i].z < meshdata->min_z)
						meshdata->min_z = meshdata->points[i].z;
				}
				meshdata->points[i].to_cell = -1;
				meshdata->points[i].to_face = -1;
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
					cells[nb_type_3_4].type = type;
					sscanf(buffer, "%*u%u%u%u", &(cells[nb_type_3_4].p1),
						&(cells[nb_type_3_4].p2),
						&(cells[nb_type_3_4].p3));
					nb_type_3_4++;
				}
				// 4 means tetra
				else if(4 == type)
				{
					meshdata->nb_cells++;
					cells[nb_type_3_4].type = type;
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

		meshdata->points[cells[i].p1].to_face = i;
		meshdata->points[cells[i].p2].to_face = i;
		meshdata->points[cells[i].p3].to_face = i;
#ifdef DEBUG
		printf("%u %u %u\n",meshdata->faces[i].p1, meshdata->faces[i].p2,meshdata->faces[i].p3);
#endif
	}
	for(i=0; i < meshdata->nb_cells; i++)
	{
		meshdata->cells[i] = cells[i+meshdata->nb_faces];
		meshdata->points[meshdata->cells[i].p1].to_cell = i;
		meshdata->points[meshdata->cells[i].p2].to_cell = i;
		meshdata->points[meshdata->cells[i].p3].to_cell = i;
		meshdata->points[meshdata->cells[i].p4].to_cell = i;
#ifdef DEBUG					
		printf("%u %u %u %u %u\n",meshdata->cells[i].type,meshdata->cells[i].p1,meshdata->cells[i].p2,meshdata->cells[i].p3, meshdata->cells[i].p4);
#endif	
	}
	safe_free(cells);
	gen_h2o_co2();
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
	fprintf(fp,"POINT_DATA %u\n",mesh->nb_points);
	fprintf(fp,"SCALARS temperature float\n");
	fprintf(fp, "LOOKUP_TABLE default\n");
	for(i=0;i < mesh->nb_points;i++)
	{
		fprintf(fp, "%.16f\n",mesh->points[i].temperature);
	}
	fprintf(fp,"\n");

	fclose(fp);
}

void after_read(t_mesh *mesh)
{
	int i;
	mesh->point_to_compute = safe_malloc(sizeof(unsigned int) * mesh->nb_points );
	mesh->boundary_point_to_compute = safe_malloc(sizeof(unsigned int) * mesh->nb_points);
	for(i=0;i<mesh->nb_points;i++)
	{
		if(!mesh->points[i].is_boundary)
		{
			if(mesh->points[i].h2o < 1e-3 || mesh->points[i].co2 < 1e-3 )
				continue;
			mesh->boundary_point_to_compute[mesh->nb_point_to_compute] = i;
		}
		else
		{
			mesh->nb_boundary_points++;
			mesh->nb_boundary_point_to_compute++;
		}
		mesh->point_to_compute[mesh->nb_point_to_compute] = i; 
		mesh->nb_point_to_compute++;
	}
	mesh->max_length = sqrt(pow(mesh->max_x-mesh->min_x,2) +
		pow(mesh->max_y-mesh->min_y,2) +
		pow(mesh->max_z-mesh->min_z,2));
	printf("begin split!\n");
	split_cells_to_faces();
	printf("end split!\n");
}
void gen_h2o_co2()
{
	t_mesh *mesh = get_mesh();
	if(mesh->func_hooks.set_h2o_co2_hook != NULL)
		return mesh->func_hooks.set_h2o_co2_hook();
	unsigned int i;
	for(i=0;i<mesh->nb_points;i++)
	{
		mesh->points[i].h2o = 0.5;
		mesh->points[i].co2 = 0.5;
		mesh->points[i].temperature = 300;
	}
}
void destroy_mesh()
{
	t_mesh *mesh = get_mesh();
	safe_free(mesh->points);
	safe_free(mesh->point_to_compute);
	safe_free(mesh->boundary_point_to_compute);
	safe_free(mesh->faces);
	safe_free(mesh->cells);
}
inline void swap(int *x,int*y)
{
	int temp;
	temp = *x;
	*x = *y;
	*y = temp;
}
void make_lite_to_big(int *x,int *y,int *z)
{
	if(*x > *y)
		swap(x,y);
	if(*x > *z)
		swap(x,z);
	if(*y > *z)
		swap(y,z);
}
int test_face(int p1, int p2, int p3, int point_to_face[][100],t_face *faces, int *face_id)
{
	int i,v1,v2,v3;
//	printf("%d\n",nb_point_to_face);
	for(i=1;i < point_to_face[p1][0] + 1;i++)
	{
		v1 = faces[point_to_face[p1][i]].p1;
		v2 = faces[point_to_face[p1][i]].p2;
		v3 = faces[point_to_face[p1][i]].p3;
		if( (v1 == p1 && v2 == p2 && v3 == p3) ||
			(v1 == p1 && v2 == p3 && v3 == p2) ||
			(v1 == p2 && v2 == p1 && v3 == p3) ||
			(v1 == p2 && v2 == p3 && v3 == p1) ||
			(v1 == p3 && v2 == p2 && v3 == p1) ||
			(v1 == p3 && v2 == p1 && v3 == p2) ) 
		{
			*face_id =point_to_face[p1][i];
			return 1;
		}
	}
	return 0;
}
void split_cells_to_faces()
{
	t_mesh *mesh = get_mesh();
	t_face *faces = safe_malloc(sizeof(t_face) * 4*mesh->nb_cells);
	int (*point_to_face)[100] = safe_malloc(sizeof(int)*100*mesh->nb_points);
	memset(point_to_face,0,sizeof(int)*100*mesh->nb_points);
	memset(faces,0,sizeof(t_face)*4*mesh->nb_cells);
	printf("nb_tetara:%u\n",mesh->nb_cells);
	int i,p1,p2,p3,nb_faces=0,face_id;
	for(i=0;i<4*mesh->nb_cells;i++)
	{
		faces[i].to_cell[0] = -1;
		faces[i].to_cell[1] = -1;
	}

	//face1
	for(i=0;i<mesh->nb_cells;i++)
	{
		p1 = mesh->cells[i].p1;
		p2 = mesh->cells[i].p2;
		p3 = mesh->cells[i].p3;

		if(test_face(p1, p2, p3, point_to_face, faces, &face_id))	//face exsist
		{
			faces[face_id].to_cell[1] = i;
			mesh->cells[i].to_faces[0] = face_id;
		}
		else
		{
			faces[nb_faces].p1 = p1;
			faces[nb_faces].p2 = p2;
			faces[nb_faces].p3 = p3;
			faces[nb_faces].to_cell[0] = i;
			mesh->cells[i].to_faces[0] = face_id;
			point_to_face[p1][0]++;
			point_to_face[p2][0]++;
			point_to_face[p3][0]++;
			point_to_face[p1][point_to_face[p1][0]] = nb_faces;
			point_to_face[p2][point_to_face[p2][0]] = nb_faces;
			point_to_face[p3][point_to_face[p3][0]] = nb_faces;
			nb_faces++;
		}

		//face2
		p1 = mesh->cells[i].p1;
		p2 = mesh->cells[i].p2;
		p3 = mesh->cells[i].p4;

		if(test_face(p1, p2, p3, point_to_face, faces, &face_id))	//face exsist
		{
			faces[face_id].to_cell[1] = i;
			mesh->cells[i].to_faces[1] = face_id;
		}
		else
		{
			faces[nb_faces].p1 = p1;
			faces[nb_faces].p2 = p2;
			faces[nb_faces].p3 = p3;
			faces[nb_faces].to_cell[0] = i;
			mesh->cells[i].to_faces[1] = face_id;
			point_to_face[p1][0]++;
			point_to_face[p2][0]++;
			point_to_face[p3][0]++;
			point_to_face[p1][point_to_face[p1][0]] = nb_faces;
			point_to_face[p2][point_to_face[p2][0]] = nb_faces;
			point_to_face[p3][point_to_face[p3][0]] = nb_faces;
			nb_faces++;
		}
		//face 3
		p1 = mesh->cells[i].p1;
		p2 = mesh->cells[i].p3;
		p3 = mesh->cells[i].p4;

		if(test_face(p1, p2, p3, point_to_face, faces, &face_id))	//face exsist
		{
			faces[face_id].to_cell[1] = i;
			mesh->cells[i].to_faces[2] = face_id;
		}
		else
		{
			faces[nb_faces].p1 = p1;
			faces[nb_faces].p2 = p2;
			faces[nb_faces].p3 = p3;
			faces[nb_faces].to_cell[0] = i;
			mesh->cells[i].to_faces[2] = face_id;
			point_to_face[p1][0]++;
			point_to_face[p2][0]++;
			point_to_face[p3][0]++;
			point_to_face[p1][point_to_face[p1][0]] = nb_faces;
			point_to_face[p2][point_to_face[p2][0]] = nb_faces;
			point_to_face[p3][point_to_face[p3][0]] = nb_faces;
			nb_faces++;
		}
		//face 4
		p1 = mesh->cells[i].p2;
		p2 = mesh->cells[i].p3;
		p3 = mesh->cells[i].p4;

		if(test_face(p1, p2, p3, point_to_face, faces, &face_id))	//face exsist
		{
			faces[face_id].to_cell[1] = i;
			mesh->cells[i].to_faces[3] = face_id;
		}
		else
		{
			faces[nb_faces].p1 = p1;
			faces[nb_faces].p2 = p2;
			faces[nb_faces].p3 = p3;
			faces[nb_faces].to_cell[0] = i;
			mesh->cells[i].to_faces[3] = face_id;
			point_to_face[p1][0]++;
			point_to_face[p2][0]++;
			point_to_face[p3][0]++;
			point_to_face[p1][point_to_face[p1][0]] = nb_faces;
			point_to_face[p2][point_to_face[p2][0]] = nb_faces;
			point_to_face[p3][point_to_face[p3][0]] = nb_faces;
			nb_faces++;
		}
		//printf("%d\n",nb_faces);
		//getchar();

	}
	int nb_d_faces=0;
	for(i=0;i<nb_faces;i++)
	{
		if(faces[i].to_cell[0] != -1 && faces[i].to_cell[1] != -1)
		{
#ifdef DEBUG
			printf("%d %d %d -> %d: %d %d %d %d | %d: %d %d %d %d",faces[i].p1,faces[i].p2,faces[i].p3,
				faces[i].to_cell[0],
				mesh->cells[faces[i].to_cell[0]].p1,
				mesh->cells[faces[i].to_cell[0]].p2,
				mesh->cells[faces[i].to_cell[0]].p3,
				mesh->cells[faces[i].to_cell[0]].p4,
				faces[i].to_cell[1],
				mesh->cells[faces[i].to_cell[1]].p1,
				mesh->cells[faces[i].to_cell[1]].p2,
				mesh->cells[faces[i].to_cell[1]].p3,
				mesh->cells[faces[i].to_cell[1]].p4);
			getchar();
#endif
			nb_d_faces++;
		}
		else
		{
			faces[i].is_boundary = 1;
		}
	}
	printf("nb_d_faces:%d\n",nb_d_faces);
	//safe_free(faces);
	safe_free(point_to_face);
	printf("split nb_faces:%d\n",nb_faces);


	safe_free(mesh->faces);
	mesh->faces = faces;
	mesh->nb_faces = nb_faces;

}
