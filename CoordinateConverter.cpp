#include"CoordinateConverter.h"
#include <QtDebug>
#define KMS_PER_GEO_DEGREE 108.0

CoordinateConverter::CoordinateConverter(double lat, double lon, double alt )
{
	refpt_lat = lat;
	refpt_lon = lon;
	refpt_alt = alt;

	degorradians = 0;

	esq = 0.00669437999013;     re  = 6378137.0;
}


void CoordinateConverter::setlp(double lat, double lon, double alt)
{
  	refpt_lat = lat;
	refpt_lon = lon;
	refpt_alt = alt;

	degorradians = 0;
}

int CoordinateConverter::getlp(double* lat, double* lon, double* alt)
{
	*lat = refpt_lat;
	*lon = refpt_lon;
	*alt = refpt_alt;

	return 0;
}





double CoordinateConverter::dms2degrees(double deg, double m , double s)
{
double d=0.0;

d = deg + (m/60.0) + (s/3600.0);
//d = round(d , 12);
return d;
}

double CoordinateConverter::degrees2rad(double deg)
{
double r = 0.0;
//long long l=0;
r = (deg*(PI/180.0));
//printf("\nvalue before rounding off = %20.10lf \n" , r );
//r = round(r , 12);
//printf("\nvalue after rounding off = %20.10lf \n" , r );
return r;
}

double CoordinateConverter::rad2degrees(double rad)
{
double d = 0.0;
d = (rad*(180.0/PI));
//printf("\nvalue before rounding off = %20.10lf \n" , d );
//d = round(d , 12);
//printf("\nvalue after rounding off = %20.10lf \n" , d );
return d;
}


#define ZERO 1.0e-15
#define ONE  (1.0-ZERO)

//      #define SIGN(x)  ((x>0) ? 1.0 : -1.0)
#define NEARLY_ONE(x)  (((fabs(x)>ONE)&&(fabs(x)<=1.0)) ?  \
                               ONE*((x>0) ? 1.0 : -1.0)  : x)


bool CoordinateConverter::getEarthLocalRadius(double lat_in_rad , double &r)
{
	double a=6378137.0 , sintheta,costheta , nr, dr;
	double b  = 6356752.3;
	sintheta = sin(lat_in_rad);		costheta = cos(lat_in_rad);
	nr = (  (a*a* costheta) * (a*a* costheta)  )  + (  (b*b*sintheta)*(b*b*sintheta)  );
	dr = (  (a*costheta)*(a*costheta) + (b*sintheta)*(b*sintheta)  );
	r = sqrt(nr/dr);

	return true;
}	// getEarthLocalRadius



//-----------------------------------------------------------------------------
/* The following function resolves a vector defined, in ENV (X(EAST),
   Y(NORTH) AND Z(VERTICAL))  in ECEF */
void CoordinateConverter::env_ecef(double x_env,double y_env,double z_env,double *x_ecef,double *y_ecef,double *z_ecef,double lat_env,double lon_env, int angletype)
{

	if ( angletype == 0 )  // input lat , lon are in degrees. convert it to radians before proceeding
	{
		lat_env = degrees2rad(lat_env);
		lon_env = degrees2rad(lon_env);
	}

	*x_ecef= cos(lat_env)*cos(lon_env)*z_env-sin(lon_env)*x_env-sin(lat_env)*cos(lon_env)*y_env;
        *y_ecef= cos(lat_env)*sin(lon_env)*z_env+cos(lon_env)*x_env-sin(lat_env)*sin(lon_env)*y_env;
	*z_ecef= sin(lat_env)*z_env+cos(lat_env)*y_env;

    return;
}/* end of env_ecef */


/* The following function resolves a vector defined in ECEF, in ENV (X(EAST),
   Y(NORTH) AND Z(VERTICAL)) */

void CoordinateConverter::ecef_env(double *x_env,double *y_env,double *z_env,double x_ecef,double y_ecef,double z_ecef,double lat_env,double lon_env, int angletype)
{

	if ( angletype == 0 )  // input lat , lon are in degrees. convert it to radians before proceeding
	{
		lat_env = degrees2rad(lat_env);
		lon_env = degrees2rad(lon_env);
	}

	*x_env= -sin(lon_env)*x_ecef+y_ecef*cos(lon_env);
    *y_env= -sin(lat_env)*cos(lon_env)*x_ecef-sin(lat_env)*sin(lon_env)*y_ecef+z_ecef*cos(lat_env);
	*z_env=  cos(lat_env)*cos(lon_env)*x_ecef+cos(lat_env)*sin(lon_env)*y_ecef+sin(lat_env)*z_ecef;

    return;
}/* end of ecef_env */

//-----------------------------------------------------------------------------
/* convertion Geodetic cartesian Coordinate System to ENV cartesian */

 /* Given the coordinates of a point in ECEF,the following function finds
    the coordinates of the same point in ENV (X(EAST),Y(NORTH) AND
	Z(VERTICAL)) centered at (lat_env,lon_env) ) */

/*  NOTE(s):1.(x_ecef,y_ecef,z_ecef) must be in meters as earth radius is
            taken in meters in the following function.
            2. angles (i.e.,(lat_env,lon_env)) must be in radians.
		    3. output (i.e., (x_env,y_env,z_env)) will be in meters.
 */


int CoordinateConverter::ecef2env(double *x_env,double *y_env,double *z_env,double x_ecef,double y_ecef,double z_ecef,double lat_env,double lon_env,double h , int angletype)
{


/*

	if ( angletype == 1 )  // input lat , lon are in degrees. convert it to radians before proceeding
	{
		lat_env = rad2degrees(lat_env);
		lon_env = rad2degrees(lon_env);
	}

 ecef2envspherical(x_ecef, y_ecef , z_ecef, lat_env,  lon_env,  x_env,  y_env,  z_env);
 *z_env = *z_env + h;
*/


//printf("inside function ecef2env     %lf  %lf   %lf   %lf   %lf   %lf  " , x_ecef , y_ecef , z_ecef , lat_env , lon_env , h);
	double x,y,z,x_lp,y_lp,z_lp, r_lamda;

//	double major_axis = 6378135.00 , minor_axis = 6356750.00 , f , ecc;

	if ( angletype == 0 )  // input lat , lon are in degrees. convert it to radians before proceeding
	{
		lat_env = degrees2rad(lat_env);
		lon_env = degrees2rad(lon_env);
	}

//printf("\ninside ecef to env function X = %20.10lf, Y=%20.10lf, Z = %20.10lf , \n lat = %20.10lf , lon = %20.10lf, alt = %lf \n" , x_ecef , y_ecef , z_ecef , lat_env , lon_env , h);
	r_lamda=re/sqrt((1.0-esq*sin(lat_env)*sin(lat_env)));

	//  convertion of lat long (in radians) to ecef(in meters)

	x_lp=(r_lamda+h)*cos(lat_env)*cos(lon_env);
	y_lp=(r_lamda+h)*cos(lat_env)*sin(lon_env);
	z_lp=((1.0-esq)*r_lamda+h)*sin(lat_env);

	x=x_ecef-x_lp;
	y=y_ecef-y_lp;
	z=z_ecef-z_lp;

//printf("\nSending params : x=%lf , y=%lf , z = %lf , lat = %lf , lon = %lf " , x,y,z,lat_env,lon_env );
	ecef_env(x_env,y_env,z_env,x,y,z,lat_env,lon_env , 1);


//printf("\ninside fn: x = %lf, y = %lf, z = %lf " , *x_env , *y_env , *z_env );
	return 1;
}// end of ecef2env




//-----------------------------------------------------------------------------

/* ENV cartesian to convertion Geodetic cartesian Coordinate System   */
/* Given the coordinates of a point in ENV (X(EAST),Y(NORTH) AND
   Z(VERTICAL)) centered at (lat_env,lon_env)),this function finds
   the coordinates of the same point in ECEF */

/*  NOTE(s):1.(x_env,y_env,z_env) must be in meters as earth radius is
              taken in meters in the following function.
            2. angles (i.e.,(lat_env,lon_env)) must be in radians.
			3. output (i.e., (x_ecef,y_ecef,z_ecef)) will be in meters.
 */


/*
void ecef_env(double *x_env,double *y_env,double *z_env,double x_ecef,double y_ecef,double z_ecef,double lat_env,double lon_env)
{

        *x_env= -sin(lon_env)*x_ecef+y_ecef*cos(lon_env);
        *y_env= -sin(lat_env)*cos(lon_env)*x_ecef-sin(lat_env)*sin(lon_env)*y_ecef+z_ecef*cos(lat_env);
        *z_env=  cos(lat_env)*cos(lon_env)*x_ecef+cos(lat_env)*sin(lon_env)*y_ecef+sin(lat_env)*z_ecef;

		//printf("INS: %lf %lf %lf\n",*x_env,*y_env,*z_env);

    return;
}  // end of ecef_env



void ecef2env(double *x_env,double *y_env,double *z_env,double x_ecef,double y_ecef,double z_ecef,double lat_env,double lon_env,double h)
{
        double x,y,z,x_lp,y_lp,z_lp;
        double r_lamda,esq=0.00669437999013;//re=6380970.81804;

        r_lamda=re/sqrt((1.0-esq*sin(lat_env)*sin(lat_env)));


        //  convertion of lat long (in radians) to ecef(in meters)

        x_lp=(r_lamda+h)*cos(lat_env)*cos(lon_env);
		y_lp=(r_lamda+h)*cos(lat_env)*sin(lon_env);
		z_lp=((1.0-esq)*r_lamda + h)*sin(lat_env);

		//printf("rlamba = %lf h= %lf\n",r_lamda,h);
		//printf("%lf %lf %lf \n",cos(lat_env)*cos(lon_env),(r_lamda+h),(r_lamda+h)*cos(lat_env)*cos(lon_env));
		//printf("x y z -lp %lf %lf %lf\n",x_lp,y_lp,z_lp);

        x=x_ecef-x_lp;
        y=y_ecef-y_lp;
        z=z_ecef-z_lp;
		//printf("x y z : %lf %lf %lf\n",x,y,z);
        ecef_env(x_env,y_env,z_env,x,y,z,lat_env,lon_env);

        return;
}// end of ecef2env

*/


int CoordinateConverter::env2ecef(double x_env,double y_env,double z_env,double *x_ecef,double *y_ecef,double *z_ecef,double lat_env,double lon_env,double h , int angletype)
{
	double x,y,z,x_lp,y_lp,z_lp;
	double r_lamda;


	if ( angletype == 0 )  // input lat , lon are in degrees. convert it to radians before proceeding
	{
		lat_env = degrees2rad(lat_env);
		lon_env = degrees2rad(lon_env);
	}

//printf("\nx = %20.10lf , y = %20.10lf , z = %20.10lf , lat = %20.10lf , lon = %20.10lf \n", x_env,y_env,z_env ,  lat_env , lon_env );


	/*   convertion of lat long in radians to x y z in ecef */
	r_lamda=re/sqrt((1.0-esq*sin(lat_env)*sin(lat_env)));
	x_lp=(r_lamda+h)*cos(lat_env)*cos(lon_env);y_lp=(r_lamda+h)*cos(lat_env)*sin(lon_env);z_lp=((1.0-esq)*r_lamda+h)*sin(lat_env);

	env_ecef(x_env,y_env,z_env,&x,&y,&z,lat_env,lon_env , 1);

//printf("\n Intermediate values x = %lf , y = %lf , z = %lf \n " , x , y , z );


	*x_ecef= x+x_lp;
	*y_ecef= y+y_lp;
	*z_ecef= z+z_lp;


	return 1;
}  // end of env2ecef



//-----------------------------------------------------------------------------
/* The following function converts a vector defined in ENV from cartesion
   form to polar form
   IMPORTANT NOTE :azimuth angle is defined from local North ( not from
   x-axis i.e., east)
*/
int CoordinateConverter::env2polar(double *r,  double *azm, double *elv,  double x_env,double y_env,double z_env)
{

    *elv=0;     *azm = 0.0;   *r = 0.0;

    *r = sqrt(x_env*x_env+y_env*y_env+z_env*z_env);
    if (z_env!=0) *elv= atan2(z_env,sqrt(x_env*x_env+y_env*y_env));
    *azm= atan2(x_env,y_env);/* x_env/y_env .. because azimuth is defined
      from north */
*azm = rad2degrees( *azm );
if ( *azm < 0.0 ) *azm = *azm + 360.0;
*elv = rad2degrees(*elv);

    return 1;
}/* end of env2polar */

//-----------------------------------------------------------------------------
/* The following function converts a vector defined in ENV from polar
   form to cartesion form
   IMPORTANT NOTE :azimuth angle should be defined from local North ( not from
   x-axis i.e., east)
*/
int CoordinateConverter::polar2env(double r, double azm, double elv, double *x_env,double *y_env,double *z_env , int angletype)
{
//printf("\n R = %lf , theta = %lf , phi = %lf \n" , r , azm , elv );
	if ( angletype == 0 )  // input lat , lon are in degrees. convert it to radians before proceeding
	{
		azm = degrees2rad(azm);
		elv = degrees2rad(elv);
	}

	*x_env = r*cos(elv)*sin(azm);/* because azimuth is defined
	                                from north(y axis) */
	*y_env = r*cos(elv)*cos(azm);
	*z_env = r*sin(elv);

	return 1;
}/* end of polar2env */

//-----------------------------------------------------------------------------
int CoordinateConverter::geodetic2ecef(double lat, double lon, double alt, double* x,double* y, double* z, int angletype)
{

        double r_lamda;

	if ( angletype == 0 )  // input lat , lon are in degrees. convert it to radians before proceeding
	{
		lat = degrees2rad(lat);
		lon = degrees2rad(lon);
	}

        r_lamda=re/sqrt((1.0-esq*sin(lat)*sin(lat)));
        *x=(r_lamda+alt)*cos(lat)*cos(lon);
        *y=(r_lamda+alt)*cos(lat)*sin(lon);
	*z=(((1.0-esq)*r_lamda)+alt)*sin(lat);

return 1;
}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
int CoordinateConverter::geodetic2ecef(float lat, float lon, float alt, float* x,float* y, float* z, int angletype)
{

        float r_lamda;

        if ( angletype == 0 )  // input lat , lon are in degrees. convert it to radians before proceeding
        {
                lat = degrees2rad(lat);
                lon = degrees2rad(lon);
        }

        r_lamda=re/sqrt((1.0-esq*sin(lat)*sin(lat)));
        *x=(r_lamda+alt)*cos(lat)*cos(lon);
        *y=(r_lamda+alt)*cos(lat)*sin(lon);
        *z=(((1.0-esq)*r_lamda)+alt)*sin(lat);

return 1;
}

//-----------------------------------------------------------------------------

/*   GCS cartesian to geodetic (latitude, longitude) */
int CoordinateConverter::ecef2geodetic(double x, double y, double z, double* lat, double* lon, double* alt )
{
double a = re;
//double f = 298.257223563;
double b = 6356752.3142;   // Polar Radius of Earth
double e1 = esq;
double e2 = 0.00673949674226;     // ????????????
double rlamda,p,theta;
double tan1,tan2,tan3;



p = sqrt((x*x) + (y*y));
tan1 = (z*a)/(p*b);
tan2 = (y/x);
theta = atan(tan1);
tan3 = (z + e2*b*sin(theta)*sin(theta)*sin(theta))/(p - e1*a*cos(theta)*cos(theta)*cos(theta));


*lon = atan(tan2);
*lat = atan(tan3);

rlamda=a/sqrt((1.0-e1*sin(*lat)*sin(*lat)));
*alt = (p/cos(*lat)) - rlamda;


*lat = rad2degrees(*lat);
*lon = rad2degrees(*lon);

//if (*lat < 0.0)          *lat += 180.0;
//if (*lon < 0.0)          *lon += 180.0;

return 1;
}


// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int CoordinateConverter::env2geodetic(double x_env,double y_env,double z_env, double lat_env,double lon_env,double h, double *lat, double *lon, double *alt , int angletype)
{
double  a,b,c;

// First convert ENV to ECEF and then to Geodetic
env2ecef( x_env, y_env, z_env, &a, &b , &c , lat_env, lon_env, h , angletype);
//printf("\n Intermediate ECEF Values : x = %f , y = %f , z = %f " , a , b , c );
// Now, convert this ECEF to Geodetic

ecef2geodetic( a,  b, c , lat, lon, alt );

if ( (lon_env + metersToDegrees(x_env) ) >= 90.0 )
{
    if ( *lon < 0 )
    {
        *lon += 180.0;
    }
}
else if ( (lon_env + metersToDegrees(x_env) ) <= -90.0 )
{
    if ( *lon > 0 )
    {
        *lon = *lon - 180.0;
    }
}

return 1;

}

double CoordinateConverter::metersToDegrees( double meters )
{
    double degrees = meters / (KMS_PER_GEO_DEGREE * 1000.0); // 108 kms
    return degrees;
}


int CoordinateConverter::ecef2polar(double x_ecef,double y_ecef,double z_ecef, double lat_env,double lon_env,double h, double *r, double *azm , double *elv , int angletype )
{
double  x ,y,z;

ecef2env( &x, &y, &z,  x_ecef, y_ecef, z_ecef,  lat_env,  lon_env,  h , angletype);

env2polar( r, azm , elv,  x, y, z);


return 1;
}



int CoordinateConverter::polar2ecef(double r, double azm, double elv, double lat_env,double lon_env,double h , double *x_ecef, double *y_ecef, double *z_ecef , int angletype)
{
// First, convert Polar to ENV
double x,y,z;

polar2env( r, azm, elv,  &x, &y, &z , angletype );

// Now, convert the ENV to ECEF
env2ecef(x, y, z, x_ecef, y_ecef, z_ecef, lat_env, lon_env, h , angletype);


return 1;
}

int CoordinateConverter::polar2geodetic(double r, double azm, double elv, double lat_p,double lon_p,double h , double *lat, double *lon, double *alt , int angletype)
{
double x_ecef,  y_ecef,  z_ecef ;
// First convert to ECEF
polar2ecef( r, azm , elv,  lat_p, lon_p, h , &x_ecef, &y_ecef, &z_ecef , angletype );

// Now, converT this ECEF to Geodetic
ecef2geodetic( x_ecef,  y_ecef,  z_ecef ,  lat , lon, alt ); // output lat, lon will be in degrees

return 1;
}

int CoordinateConverter::geodetic2env(double lat, double lon, double alt , double lat_g, double lon_g, double alt_g, double *x_env, double *y_env,double *z_env , int angletype)
{
double  x,y,z;

// First, conver Getodetic to ECEF
geodetic2ecef( lat,  lon,  alt, &x,  &y ,  &z , angletype);
//printf("\nin the middle, ECEF X = %f , Y = %f , z = %f" , x , y , z );

// printf("\nlat = %lf , long = %lf   \n", lat_g, lon_g  );

// Now, convert this to ENV
ecef2env( x_env, y_env, z_env,  x, y, z,  lat_g,  lon_g,  alt_g , angletype);

return 1;
}


int CoordinateConverter::geodetic2polar(double lat, double lon, double alt , double lat_g, double lon_g, double alt_g,  double *r,  double *azm , double *elv , int angletype)
{
double x_ecef,  y_ecef,  z_ecef ;
// First convert Geodetic to ECEF
geodetic2ecef( lat,  lon,  alt, &x_ecef,  &y_ecef,  &z_ecef , angletype);

// Then convert this to Polar
ecef2polar(x_ecef,  y_ecef,  z_ecef, lat_g, lon_g, alt_g, r, azm , elv , angletype);

return 1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int CoordinateConverter::env2drcral(double x_env, double y_env, double z_env, double az, double* dr, double* cr, double* al , int angletype)
{

double erl= re;  // erl= 6380970.81804

	if(angletype == 0)   // angle was given in degrees. convert it to radians
    {
		az =  degrees2rad(az);
    }
	*dr=x_env*sin(az) + y_env*cos(az);
	*cr=-x_env*cos(az) + y_env*sin(az);
	*al=sqrt((*dr)*(*dr) + (*cr)*(*cr) + (z_env + erl)*(z_env +erl)) - erl;

    *al = z_env;
//	*dr=erl*atan((*dr)/(erl+z_env));



return 1;
}


void CoordinateConverter::alt_to_z(double x,double y,double* z,double lp_lat,double alt)
{
    double  erl;

//    cons= re;
//    sqecc=0.006694317778;   //   00669437999013

    //double si1=0;
    //si1=sin(lp_lat);
/*
	sqecc = esq;

    	r1=sqrt(1.0-sqecc*(si1*si1));   // 6378137
    	erl= re/r1;

*/

//	erl = re;	// commented on 6 dec 2012
	getEarthLocalRadius(lp_lat , erl);

//  printf("\n some variables, sqecc=%lf, si1=%lf, r1=%lf, erl=%lf " , sqecc, si1, r1, erl);
     	*z = sqrt( (alt+erl)*(alt+erl) - (x*x+y*y) )-erl;

//printf("\n insde alt to z fn, x2 + y2 = %lf ,           (alt+erl)*(alt+erl)=%lf  " , (x*x+y*y) , (alt+erl)*(alt+erl)   );
}




int CoordinateConverter::drcral2env(double dr,double cr,double al,double az,double lat,double lon , double* x_env, double* y_env, double* z_env , int angletype)
{
double z;
if (angletype == 0)  // angles are  in degrees. convert it to radians
	{
		az = degrees2rad(az);
		lat = degrees2rad(lat);
		lon = degrees2rad(lon);
	}
//printf("\nDrCrAl to ENV  Lat = %15.10lf , Lon = %15.10lf " , lat, lon );


*x_env=dr*sin(az)-cr*cos(az);
*y_env=dr*cos(az)+cr*sin(az);
alt_to_z( *x_env, *y_env, &z , lat , al);
*z_env = z;

//   printf("\n inside function, x = %lf, y=%lf, z=%lf " , *x_env, *y_env, *z_env );

return 1;
}


int CoordinateConverter::ecef2drcral(double x_ecef, double y_ecef, double z_ecef, double az, double lat_env,double lon_env,double h , double* dr, double* cr, double* al , int angletype)
{
double x_env , y_env, z_env ;

// convert the ECEF to ENV first
ecef2env( &x_env, &y_env, &z_env, x_ecef,  y_ecef,  z_ecef, lat_env, lon_env, h , angletype );

// Now, convert ENV to dr cr al
env2drcral(x_env , y_env, z_env,  az,  dr,  cr,  al , angletype);


return 1;
}

int CoordinateConverter::drcral2ecef(double dr, double cr, double al, double az, double lp_lat, double lp_lon, double lp_alt, double* x_ecef, double* y_ecef, double* z_ecef, int angletype)
{
double x_env, y_env, z_env ;
// First convert drcral to env
drcral2env(dr, cr, al, az, lp_lat, lp_lon , &x_env, &y_env, &z_env , angletype);

// Now, convert this to ecef
env2ecef( x_env, y_env, z_env, x_ecef, y_ecef, z_ecef,lp_lat , lp_lon ,lp_alt , angletype);

return 1;
}

int CoordinateConverter::drcral2geodetic(double dr, double cr, double al, double az, double lp_lat, double lp_lon, double lp_alt, double* g_lat, double *g_lon, double* g_alt, int angletype)
{
double x_ecef,  y_ecef, z_ecef;

// first convert to ecef
drcral2ecef(  dr,   cr,   al,   az,   lp_lat,   lp_lon,   lp_alt,  &x_ecef,  &y_ecef, &z_ecef, angletype);

//printf("\n intermediate ecef values are X=%lf, Y=%lf, Z=%lf " , x_ecef,  y_ecef, z_ecef );

// Convert to geodetic now
ecef2geodetic(x_ecef,  y_ecef, z_ecef, g_lat, g_lon, g_alt );
//printf("\n Geodetic lat=%lf, lon=%lf, alt=%lf " , *g_lat, *g_lon, *g_alt );

return 1;
}


int CoordinateConverter::drcral2polar(double dr,double cr,double al,double az,double lat,double lon , double* r, double* theta, double* phi, int angletype)
{
double x_env, y_env, z_env ;

// First convert to env
drcral2env(  dr,  cr,  al,  az,  lat,  lon , &x_env, &y_env, &z_env , angletype);

// then convert env to polar
env2polar(r,  theta, phi,  x_env, y_env, z_env);

return 1;
}


int CoordinateConverter::polar2drcral(double r, double azm, double elv, double az, double* dr, double* cr, double* al,  int angletype)
{
double x_env, y_env, z_env ;
// First  convert to env
polar2env(  r,   azm,   elv,   &x_env, &y_env, &z_env , angletype);

//then convert this env to drcral
env2drcral(  x_env,   y_env,   z_env,  az,  dr,  cr,  al , angletype);


return 1;
}


int CoordinateConverter::geodetic2drcral(double lat, double lon, double alt, double az, double lat_env,double lon_env,double h , double* dr, double* cr, double* al, int angletype)
{
double x,y,z;

// First convert to ecef
geodetic2ecef(  lat,   lon,   alt, &x, &y, &z, angletype);

// then convert ecef to drcral
ecef2drcral(x,y,z,   az,   lat_env,  lon_env,  h ,  dr,  cr,  al , angletype);


return 1;
}

void CoordinateConverter :: cov_pos_polar2env(double var_env[][3],double r,double ele,double azi,double var_plr[][3])
{
        Cmatrix mat;
        int Row,Col;
        double JT[MAXCOLS][MAXCOLS],temp[MAXCOLS][MAXCOLS];
        //IV&V Bug No - 535,537
        for(Row = 0;Row < MAXCOLS;Row++)
        {
                for(Col = 0;Col < MAXCOLS;Col++)
                {
                        JT[Row][Col]= 0.0;
                        temp[Row][Col]= 0.0;
                }
        }
        double J[MAXCOLS][MAXCOLS]={cos(ele)*sin(azi),-r*sin(ele)*sin(azi),r*cos(ele)*cos(azi),
                                        cos(ele)*cos(azi),-r*sin(ele)*cos(azi),-r*cos(ele)*sin(azi),
                                        sin(ele),r*cos(ele),0};
        mat.mat_transpose(*J,*JT,3,3);
        mat.mat_mul(*var_plr,*JT,*temp,3,3,3);
        mat.mat_mul(*J,*temp,*var_env,3,3,3);
}

void  CoordinateConverter :: cov_pos_env2ecef(double var_ecef[][3],double lat_env,double lon_env,double var_ENV[][3])
{
        Cmatrix mat;
        int Row,Col;/* ASP_IVV  01/12/16  12:00:00  */

        Row = 3;
        Col = 3;
        double JT[MAXCOLS][MAXCOLS],temp[MAXCOLS][MAXCOLS];
        //IV&V Bug No - 538 & 539
        for(Row = 0;Row < MAXCOLS;Row++)
        {
                for(Col = 0;Col < MAXCOLS;Col++)
                {
                        JT[Row][Col]= 0.0;
                        temp[Row][Col]= 0.0;
                }
        } 

        double J[MAXCOLS][MAXCOLS]= {-sin(lon_env),-sin(lat_env)*cos(lon_env),cos(lat_env)*cos(lon_env),
                                        cos(lon_env),-sin(lat_env)*sin(lon_env), cos(lat_env)*sin(lon_env),
                                        0,cos(lat_env),sin(lat_env)};
        mat.mat_transpose(*J,*JT,3,3);
        mat.mat_mul(*var_ENV,*JT,*temp,3,3,3);
        mat.mat_mul(*J,*temp,*var_ecef,3,3,3);
}

void CoordinateConverter :: cov_pos_ecef2env(double latdeg,double londeg,double *cov_ecef,double *cov_env)
{
        Cmatrix mat;
        double latrad,lonrad;
        latrad = latdeg *(PI/180.0);
        lonrad = londeg *(PI/180.0);

        double Jmatrix[3][3] = {-sin(lonrad),-sin(latrad)*cos(lonrad),cos(latrad)*cos(lonrad),
                                cos(lonrad),-sin(latrad)*sin(lonrad),cos(latrad)*sin(lonrad),
                                0,          cos(latrad),             sin(latrad)};


        double temp[3][3],Jtrans[3][3];
        mat.mat_transpose(*Jmatrix,*Jtrans,3,3);
        mat.mat_mul(*Jtrans,cov_ecef,*temp,3,3,3);      
        mat.mat_mul(*temp,*Jmatrix,cov_env,3,3,3);
}


/*

main()
{

	CoordinateConverter c;
	c.setlp(21, 86, 4.33);
	printf("  %15.10lf " , c.round(23.84578345734857485435 , 0) );
	printf(" %lf " , round(23.485034583405) );
}



*/



