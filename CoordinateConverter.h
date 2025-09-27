#ifndef COORD_CONV_H_INCLUDED
#define COORD_CONV_H_INCLUDED


#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include "matrix.h"
#define PI (4.0*atan(1.0))
#define MAXCOLS 3

class CoordinateConverter
{

    private:
 	double esq ,re ;
	double refpt_lat, refpt_lon, refpt_alt;
	int degorradians;


	  void alt_to_z(double x,double y,double* z,double lp_lat,double alt);


  	 //void Comp_Rearth(double latgc_lp, double *Rearth_lp);
  	 //double issose(double *sidec,double *anga,double *sidea,double *angb);
     //double obliq(double *sidea,double *sidec,double *angb,double *anga,double *angc,double *sideb);
     //void gc_gd(double xe,double ye,double ze,double *h,double *gdlat,double *gdlong);
     //void sph_dist_azi( double xlatlp,double xlonlp, double xlattg, double xlontg, double *dwnran,double *fltaz,double *tght);


public:

	inline CoordinateConverter()
	{
		refpt_lat =  refpt_lon = refpt_alt = 0.0;
		degorradians = 0;
		esq = 0.00669437999013;     re  = 6378137.0;
    }

	CoordinateConverter(double, double, double )    ;

	void setlp(double, double , double );
	int getlp(double* , double*, double* );
	double round(double , int  );
	double dms2degrees(double , double  , double );
	double degrees2rad(double );
	double rad2degrees(double );
	bool getEarthLocalRadius(double lat_in_rad , double &r);



int ecef2env(double *x_env,double *y_env,double *z_env,double x_ecef,double y_ecef,double z_ecef,double lat_env,double lon_env,double h , int angletype=0);
void ecef2env(float *x_env, float *y_env, float *z_env, float x_ecef, float y_ecef, float z_ecef, float lat_env, float lon_env, float h );

int env2ecef(double x_env,double y_env,double z_env,double *x_ecef,double *y_ecef,double *z_ecef,double lat_env,double lon_env,double h , int angletype=0);
void env2ecef(float x_env,float y_env,float z_env,float *x_ecef,float *y_ecef,float *z_ecef,float lat_env,float lon_env,float h );

int env2polar(double *r,  double *azm, double *elv,  double x_env,double y_env,double z_env);
int polar2env(double r, double azm, double elv, double *x_env,double *y_env,double *z_env , int angletype=0);
 int geodetic2ecef(double lat, double lon, double alt, double* x,double* y, double* z, int angletype=0);
 int geodetic2ecef(float lat, float lon, float alt, float* x,float* y, float* z, int angletype=0);

int ecef2geodetic(double x, double y, double z, double* lat, double* lon, double* alt );
int env2geodetic(double x_env,double y_env,double z_env, double lat_env,double lon_env,double h, double *lat, double *lon, double *alt , int angletype=0);
int ecef2polar(double x_ecef,double y_ecef,double z_ecef, double lat_env,double lon_env,double h, double *r, double *azm , double *elv , int angletype=0 );
int polar2ecef(double r, double azm, double elv, double lat_env,double lon_env,double h , double *x_ecef, double *y_ecef, double *z_ecef , int angletype=0);
int polar2geodetic(double r, double azm, double elv, double lat_p,double lon_p,double h , double *lat, double *lon, double *alt , int angletype=0);
 int geodetic2env(double lat, double lon, double alt , double lat_g, double lon_g, double alt_g, double *x_env, double *y_env,double *z_env , int angletype=0);
 int geodetic2polar(double lat, double lon, double alt , double lat_g, double lon_g, double alt_g,  double *r,  double *azm , double *elv , int angletype=0);



int env2drcral(double x_env, double y_env, double z_env, double az, double* dr, double* cr, double* al , int angletype=0);
int drcral2env(double dr,double cr,double al,double az,double lat,double lon, double* x_env, double* y_env, double* z_env , int angletype=0);
int ecef2drcral(double x_ecef, double y_ecef, double z_ecef, double az, double lat_env,double lon_env,double h , double* dr, double* cr, double* al , int angletype=0);
int drcral2ecef(double dr, double cr, double al, double az, double lp_lat, double lp_lon, double lp_alt, double* x_ecef, double* y_ecef, double* z_ecef, int angletype=0);
int drcral2geodetic(double dr, double cr, double al, double az, double lp_lat, double lp_lon, double lp_alt, double* g_lat, double *g_lon, double* g_alt, int angletype=0);
int drcral2polar(double dr,double cr,double al,double az,double lat,double lon , double* r, double* theta, double* phi, int angletype=0);
int polar2drcral(double r, double azm, double elv, double az, double* dr, double* cr, double* al,  int angletype=0);
int geodetic2drcral(double lat, double lon, double alt, double az, double lat_env,double lon_env,double h , double* dr, double* cr, double* al, int angletype=0);



void ecef_env(double *x_env,double *y_env,double *z_env,double x_ecef,double y_ecef,double z_ecef,double lat_env,double lon_env, int angletype);
void env_ecef(double x_env,double y_env,double z_env,double *x_ecef,double *y_ecef,double *z_ecef,double lat_env,double lon_env, int angletype);

        double atod(char* a);
   void substr(char* str, int start, int end , char *msg);
//Added by Kanan - 28/03/2017
void cov_pos_polar2env(double var_env[][3],double r,double ele,double azi,double var_plr[][3]);
void cov_pos_env2ecef(double var_ecef[][3],double lat_env,double lon_env,double var_ENV[][3]);
void cov_pos_ecef2env(double latdeg,double londeg,double *cov_ecef,double *cov_env);

        ~CoordinateConverter() {   }


    double metersToDegrees( double meters );



};

//double CoordinateConverter::esq = 0.00669437999013;
//double CoordinateConverter::re  = 6378137.0;


#endif
