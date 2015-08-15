#include "ck_model.h"
#include "calfunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

void generation_spectrum_parameter();
void store_kappa_le();

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
					ckmodel->small_to_big[ckmodel->nb_band] = i;
					ckmodel->weight_guass[ckmodel->nb_band] = ckmodel->weight_CK[j] * ckmodel->weight_CK[k];
					ckmodel->num_gauss[ckmodel->nb_band] = j;
					ckmodel->num_gauss2[ckmodel->nb_band] = k;
					ckmodel->nb_band++;
				}
			}
		}
		else
		{
			for(k=0;k<nb_gauss_CK;k++)
			{
				ckmodel->small_to_big[ckmodel->nb_band] = i;
				ckmodel->weight_guass[ckmodel->nb_band] = ckmodel->weight_CK[k];
				ckmodel->num_gauss[ckmodel->nb_band] = k;
				ckmodel->num_gauss2[ckmodel->nb_band] = 0;
				ckmodel->nb_band++;
			}
		}
	}
	store_kappa_le();
//#ifdef DEBUG
	printf("nb_band:%d\n",ckmodel->nb_band );
//#endif
}

void interpolation_coef_CK(double T, double xh2o, int *iTCK, int *ixCK, double *ciT1, double *ciT2, double *cix1, double *cix2)
{
	t_ck_model *ckmodel = &get_mesh()->ckmodel;
	int j;
	if(T <= ckmodel->temp_pmg[0])
	{
		*iTCK = 1;
		*ciT1 = 1;
		*ciT2 = 0;
	}
	else if(T >= ckmodel->temp_pmg[nb_temp_CK-1])
	{
		*iTCK = nb_temp_CK - 1;
		*ciT2 = 0;
		*ciT2 = 1;
	}
	else
	{
		for(j = 1; T > ckmodel->temp_pmg[j]; j++)
		{

		}
		*iTCK = j;
		*ciT2 = (T-ckmodel->temp_pmg[j] / (ckmodel->temp_pmg[j]-ckmodel->temp_pmg[j-1]) );
		*ciT1 = 1 - *ciT2;
	}
	for(j=0;xh2o > ckmodel->xh2o_CK[j];j++)
	{

	}
	*ixCK = j;
	*cix2 = (xh2o-ckmodel->xh2o_CK[j]/(ckmodel->xh2o_CK[j]-ckmodel->xh2o_CK[j]));
	*cix1 = 1 - *cix2;
}

double fQC(double T)
{
	double cco2a[4]={-2.154173478, 0.9670123486, -0.8082674825e-3, 0.2806562032e-5};
	double cco2b[4]={-351.7866975, 2.779333185, -0.3673680835e-2, 0.4090051841e-5};
	if(T < 405)
	{
		return cco2a[0] + cco2a[1]*T + cco2a[2]*pow(T,2) + cco2a[3]*pow(T,3);
	}
	else
	{
		return cco2b[0] + cco2b[1]*T + cco2b[2]*pow(T,2) + cco2b[3]*pow(T,3);
	}
}
double fQH(double T)
{
	double ch2oa[4]={-3.697248813, 0.2601622207, 0.1358654145e-2, -0.6749557714e-6};
	double ch2ob[4]={-49.01925676, 0.6271879739, 0.3293104256e-3, 0.3447216719e-6};
	if(T < 405)
	{
		return ch2oa[0] + ch2oa[1]*T + ch2oa[2]*pow(T,2) + ch2oa[3]*pow(T,3);
	}
	else
	{
		return ch2ob[0] + ch2ob[1]*T + ch2ob[2]*pow(T,2) + ch2ob[3]*pow(T,3);
	}
}

void store_kappa_le()
{
	t_ck_model *ckmodel = &get_mesh()->ckmodel;
	double  nu;
	double ciT1x1, ciT2x1, ciT1x2, ciT2x2, ciT1, ciT2, cix1, cix2;
	double kco2, kh2o;
	double t,xh2o;
	int ig, ig2, i_t, i_nu, i_xh2o;
	int iT, ix;
//le_store
	for(i_t=0;i_t<nb_t_le_store;i_t++)
	{
		t = t_min_le_store + (i_t) * delta_t_le_store;
		for(i_nu=0;i_nu<NB_BAND;i_nu++)
		{
			nu = ckmodel->nuc[ckmodel->small_to_big[i_nu]];
			ckmodel->le_store[i_nu][i_t] = ck_c1 * nu * nu * nu / (exp(ck_c2 / t * nu) - 1.0) * 
			ckmodel->deltanu[ckmodel->small_to_big[i_nu]] * ckmodel->weight_guass[i_nu];
		}
	}
//FQ store
	for(i_t=0;i_t<nb_fQ_store;i_t++)
	{
		t = t_fQ_min_store + (i_t) * delta_fQ_store;
		ckmodel->fQH_store[i_t] = 1.0 /t / fQH(t);
		ckmodel->fQC_store[i_t] = 1.0 /t /fQC(t);
	}
//kappa store
	for(i_t=0;i_t<nb_t_kappa_store;i_t++)
	{
		t = t_min_kappa_store + (i_t-1) * delta_t_kappa_store;
		for(i_xh2o = 0;i_xh2o < nb_xh2o_kappa_store+1;i_xh2o++)
		{
			xh2o = (i_xh2o) * delta_xh2o_kappa_store;
			interpolation_coef_CK(t, xh2o, &iT, &ix, &ciT1, &ciT2, &cix1, &cix2);
			ciT1x1 = ciT1*cix1;
	 		ciT2x1 = ciT2*cix1;
	 		ciT1x2 = ciT1*cix2;
	 		ciT2x2 = ciT2*cix2;
	 		for(i_nu = 0;i_nu<NB_BAND;i_nu++)
	 		{
	 			if(ckmodel->h2o_to_co2[i_nu] > 0)
	 			{
	 				ig = ckmodel->num_gauss[i_nu];
	 				ig2 = ckmodel->num_gauss2[i_nu];
	 				kh2o = (  ciT1x1 * ckmodel->kstarh[iT][ix][ckmodel->small_to_big[i_nu]][ig]  
						+ ciT2x1 * ckmodel->kstarh[iT+1][ix][ckmodel->small_to_big[i_nu]][ig]  
						+ ciT1x2 * ckmodel->kstarh[iT][ix+1][ckmodel->small_to_big[i_nu]][ig]  
						+ ciT2x2 * ckmodel->kstarh[iT+1][ix+1][ckmodel->small_to_big[i_nu]][ig]);
	 				kco2 = (  ciT1 * ckmodel->kstarc[iT][ckmodel->h2o_to_co2[ckmodel->small_to_big[i_nu]]][ig2]
						+ ciT2 * ckmodel->kstarc[iT+1][ckmodel->h2o_to_co2[ckmodel->small_to_big[i_nu]]][ig2]);
	 				ckmodel->kappa_h2o_store[i_nu][i_t][i_xh2o] = kh2o;
	 				ckmodel->kappa_co2_store[i_nu][i_t] = kco2;
	 			}
	 			else
	 			{
	 				ig = ckmodel->num_gauss[i_nu];
	 				kh2o = (  ciT1x1 * ckmodel->kstarh[iT][ix][ckmodel->small_to_big[i_nu]][ig]  
						+ ciT2x1 * ckmodel->kstarh[iT+1][ix][ckmodel->small_to_big[i_nu]][ig]  
						+ ciT1x2 * ckmodel->kstarh[iT][ix+1][ckmodel->small_to_big[i_nu]][ig]  
						+ ciT2x2 * ckmodel->kstarh[iT+1][ix+1][ckmodel->small_to_big[i_nu]][ig]);
	 				ckmodel->kappa_h2o_store[i_nu][i_t][i_xh2o] = kh2o;
	 				ckmodel->kappa_co2_store[i_nu][i_t] = 0;
	 			}
	 		}
		}
	}
}

void compute_index(double t, double h2o, int *i_t_le, int *i_t_kappa, int *i_t_fQ, int *i_h2o)
{
	*i_t_le = (int)((t - t_min_kappa_store)/delta_t_le_store + 0.5) +1;
	*i_t_kappa = (int)( (t-t_min_kappa_store)/delta_t_kappa_store +0.5) + 1;
	*i_t_fQ = (int)( (t-t_fQ_min_store)/delta_fQ_store + 0.5) + 1;
	*i_h2o  = (int)( abs(h2o)/delta_xh2o_kappa_store + 0.5);
}