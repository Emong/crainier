#include "ck_model.h"
#include "calfunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

void generation_spectrum_parameter();

void read_param_CK()
{
	t_ck_model *ckmodel = &get_mesh()->ckmodel;
	FILE *fp;
	char buffer[1024];
	unsigned int i,j,k,l;
	fp = fopen("./param_CK.don","r");
	if(NULL == fp)
	{
		perror("param_CK.don");
		exit(1);
	}
	fgets(buffer, 1024, fp);
	for(i=0;i<nb_gauss_CK;i++)
	{
		fscanf(fp, "%lf",&ckmodel->weight_CK[i]);
	}
	fgets(buffer, 1024, fp);
	for(i=0;i<nb_gauss_CK;i++)
	{
		fscanf(fp, "%lf",&ckmodel->temp_pmg[i]);
	}
	fgets(buffer, 1024, fp);
	for(i=0;i<nb_gauss_CK;i++)
	{
		fscanf(fp, "%lf",&ckmodel->xh2o_CK[i]);
	}
	fclose(fp);

	fp = fopen("./CKH2O.don","r");
	if(NULL == fp)
	{
		perror("CKH2O.don");
		exit(1);
	}
	for(i=0;i<nb_nuh2o_CK;i++)
	{
		fscanf(fp, "%lf%lf\n",&ckmodel->nuc[i],&ckmodel->deltanu[i]);
		ckmodel->nuc[i] *= 100;
		ckmodel->deltanu[i] *= 100;
//		printf("%.16g %.16g\n",ckmodel->nuc[i],ckmodel->deltanu[i]);
		for(j=0;j<nb_gauss_CK;j++)
		{
			fgets(buffer, 1024, fp);
//			puts(buffer);
			for(k=0;k<nb_xh2o_CK;k++)
			{
				for(l=0;l<nb_temp_CK;l++)
				{
					fscanf(fp,"%lf",&ckmodel->kstarh[l][k][i][j]);
					ckmodel->kstarh[l][k][i][j] *= 100;
//					printf("%.16g ",ckmodel->kstarh[l][k][i][j]);
				}
				fgetc(fp);
			}
		}
	}
	fclose(fp);
	fp = fopen("./CKCO2.don","r");
	if(NULL == fp)
	{
		perror("CKH2O.don");
		exit(1);
	}
	for(i=0;i<nb_nuco2_CK;i++)
	{
		fgets(buffer,1024,fp);
		for(j=0;j<nb_gauss_CK;j++)
		{
			fgets(buffer, 1024, fp);
			for(k=0;k<nb_temp_CK;k++)
			{
				fscanf(fp,"%lf",&ckmodel->kstarc[k][i][j]);
				ckmodel->kstarc[k][i][j] *= 100;
//				printf("%.16g ",ckmodel->kstarc[k][i][j]);
			}
			fgetc(fp);
//			puts("");
		}
	}
	fclose(fp);
	generation_spectrum_parameter();
}

void generation_spectrum_parameter()
{
	t_ck_model * ckmodel = &get_mesh()->ckmodel;
	unsigned int i,j,k;
	memset(ckmodel->h2o_to_co2, 0,	sizeof(int)*nb_nuh2o_CK);
	ckmodel->h2o_to_co2[5] = 1;
	ckmodel->h2o_to_co2[6] = 2;
	ckmodel->h2o_to_co2[7] = 3;
	ckmodel->h2o_to_co2[8] = 4;
	ckmodel->h2o_to_co2[9]= 5;
	ckmodel->h2o_to_co2[10]= 6;
	ckmodel->h2o_to_co2[11]= 7;
	ckmodel->h2o_to_co2[12]= 8;
	ckmodel->h2o_to_co2[18]= 9;
	ckmodel->h2o_to_co2[19]=10;
	ckmodel->h2o_to_co2[20]=11;
	ckmodel->h2o_to_co2[25]=12;
	ckmodel->h2o_to_co2[26]=13;
	ckmodel->h2o_to_co2[27]=14;
	ckmodel->h2o_to_co2[31]=15;
	ckmodel->h2o_to_co2[32]=16;
	ckmodel->h2o_to_co2[33]=17;
	ckmodel->nb_band = 0;
// DO i=1,nbnuh2o_CK
// 	IF (h2o_to_co2(i) > 0) THEN
// 		DO j=1, nbgauss_CK
// 			DO k=1, nbgauss_CK
// 				nb_band = nb_band + 1
// 				small_to_big(nb_band) = i
// 				weight_gauss(nb_band) = weight_CK(j) * weight_CK(k)
// 				num_gauss(nb_band) = j
// 				num_gaus2(nb_band) = k
// 			ENDDO
// 		ENDDO
// 	ELSE
// 		DO k=1, nbgauss_CK
// 			nb_band = nb_band + 1
// 			small_to_big(nb_band) = i
// 			weight_gauss(nb_band) = weight_CK(k)
// 			num_gauss(nb_band) = k
// 			num_gaus2(nb_band) = 0
// 		ENDDO
// 	ENDIF
// ENDDO
	for(i=0;i<nb_nuh2o_CK;i++)
	{
		if(ckmodel->h2o_to_co2[i] > 0)
		{
			for(j=0;j<nb_gauss_CK;j++)
			{
				for(k=0;k<nb_gauss_CK;k++)
				{
					ckmodel->nb_band++;
					ckmodel->small_to_big[ckmodel->nb_band] = i;
					ckmodel->weight_guass[ckmodel->nb_band] = ckmodel->weight_CK[j] + ckmodel->weight_CK[k];
					ckmodel->num_gauss[ckmodel->nb_band] = j;
					ckmodel->num_gauss2[ckmodel->nb_band] = k;
				}
			}
		}
		else
		{
			for(k=0;k<nb_gauss_CK;k++)
			{
				ckmodel->nb_band++;
				ckmodel->small_to_big[ckmodel->nb_band] = i;
				ckmodel->weight_guass[ckmodel->nb_band] = ckmodel->weight_CK[k];
				ckmodel->num_gauss[ckmodel->nb_band] = k;
				ckmodel->num_gauss2[ckmodel->nb_band] = 0;
			}
		}
	}
}

// !###############################################################################################################
// !
// SUBROUTINE interpolation_coef_CK(T, xh2o, iTCK, ixCK, ciT1, ciT2, cix1, cix2)
// !
// !###############################################################################################################
// !
// IMPLICIT NONE
// !
// INTEGER						:: j
// DOUBLE PRECISION, INTENT(IN)			:: T, xh2o 
// INTEGER, INTENT(OUT)		:: iTCK, ixCK
// DOUBLE PRECISION, INTENT(OUT)			:: ciT1, ciT2, cix1, cix2

void interpolation_coef_CK(double T, double xh2o, int iTCK, int ixCK, double ciT1, double ciT2, double cix1, double cix2)
{
	t_ck_model *ckmodel = &get_mesh()->ckmodel;
	int j;
	if(T <= ckmodel->temp_pmg[0])
	{
		iTCK = 1;
		ciT1 = 1;
		ciT2 = 0;
	}
	else if(T >= ckmodel->temp_pmg[nb_temp_CK-1])
	{
		iTCK = nb_temp_CK - 1;
		ciT2 = 0;
		ciT2 = 1;
	}
	else
	{
		for(j = 1; T > ckmodel->temp_pmg[j]; j++)
		{

		}
		iTCK = j;
		ciT2 = (T-ckmodel->temp_pmg[j] / (ckmodel->temp_pmg[j]-ckmodel->temp_pmg[j-1]) );
		ciT1 = 1 - ciT2;
	}
	for(j=0;xh2o > ckmodel->xh2o_CK[j];j++)
	{

	}
	ixCK = j;
	cix2 = (xh2o-ckmodel->xh2o_CK[j]/(ckmodel->xh2o_CK[j]-ckmodel->xh2o_CK[j]));
	cix1 = 1 - cix2;
}
