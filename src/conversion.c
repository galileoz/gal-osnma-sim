
/**
 * @file   convertion.c
 * @author: FDC
 * 
 * Created on 8 février 2020, 11:52
 * 
 * @brief @copybrief conversion.h
 */

#include "conversion.h"
#include "constants.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>



/*! \brief Subtract two vectors of double
 *  \param[out] y Result of subtraction
 *  \param[in] x1 Minuend of subtraction
 *  \param[in] x2 Subtrahend of subtracion
 *  @note y=x1-x2
 */
void subVect(double *y, const double *x1, const double *x2) {
    y[0] = x1[0] - x2[0];
    y[1] = x1[1] - x2[1];
    y[2] = x1[2] - x2[2];

    return;
}

/*! \brief Compute Norm of Vector
 *  \param[in] x Input vector
 *  \returns Length (Norm) of the input vector
 */
double normVect(const double *x) {
    double norm = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
#if TEST_NAN
    if (norm!=norm){
        printf("error NAN in normVect()%f\n %f,%f,%f",norm,x[0],x[1],x[2]);
        exit(5);
    }
#endif
    return norm;
}

/*! \brief Compute dot-product of two vectors
 *  \param[in] x1 First multiplicand
 *  \param[in] x2 Second multiplicand
 *  \returns Dot-product of both multiplicands
 */
double dotProd(const double *x1, const double *x2) {
    return (x1[0] * x2[0] + x1[1] * x2[1] + x1[2] * x2[2]);
}

/*! \brief Convert Earth-centered Earth-Fixed to North-East-Up
 *  \param[in] xyz Input position as vector in ECEF format
 *  \param[in] t Intermediate matrix computed by \ref ltcmat
 *  \param[out] neu Output position as North-East-Up format
 */
void ecef2neu(const double *xyz, double t[3][3], double *neu) {
    neu[0] = t[0][0] * xyz[0] + t[0][1] * xyz[1] + t[0][2] * xyz[2];
    neu[1] = t[1][0] * xyz[0] + t[1][1] * xyz[1] + t[1][2] * xyz[2];
    neu[2] = t[2][0] * xyz[0] + t[2][1] * xyz[1] + t[2][2] * xyz[2];

    return;
}

/*! \brief Convert North-Eeast-Up to Azimuth + Elevation
 *  \param[in] neu Input position in North-East-Up format
 *  \param[out] azel Output array of azimuth + elevation as double
 */
void neu2azel(double *azel, const double *neu) {
    double ne;

    azel[0] = atan2(neu[1], neu[0]);
    if (azel[0] < 0.0)
        azel[0] += (2.0 * PI);

    ne = sqrt(neu[0] * neu[0] + neu[1] * neu[1]);
    azel[1] = atan2(neu[2], ne);

    return;
}

/*! \brief Convert Earth-centered Earth-fixed (ECEF) into Latitude/Longitude/Height
 *  \param[in] xyz Input Array of X, Y and Z ECEF coordinates
 *  \param[out] llh Output Array of Latitude, Longitude and Height
 */
void xyz2llh(const double *xyz, double *llh) {
    double a, eps, e, e2;
    double x, y, z;
    double rho2, dz, zdz, nh, slat, n, dz_new;

    //Get WGS84 ellipsoid radius and eccentricity
    a = WGS84_RADIUS;
    e = WGS84_ECCENTRICITY;

    eps = 1.0e-3;
    e2 = e*e;

    x = xyz[0];
    y = xyz[1];
    z = xyz[2];

    rho2 = x * x + y*y;
    dz = e2*z;

    //Iterate to obtain a good approximation of zdz. 
    //Starting hypothesis is that geodetic and geocentric latitude are the same, given that altitude is not 0.
    while (1) {
        zdz = z + dz;
        nh = sqrt(rho2 + zdz * zdz);
        slat = zdz / nh;
        n = a / sqrt(1.0 - e2 * slat * slat);
        dz_new = n * e2*slat;

        if (fabs(dz - dz_new) < eps)
            break;

        dz = dz_new;
    }

    //Finish the conversion
    llh[0] = atan2(zdz, sqrt(rho2));
    llh[1] = atan2(y, x);
    llh[2] = nh - n;

    return;
}

/*! \brief Convert Latitude/Longitude/Height into Earth-centered Earth-fixed (ECEF)
 *  \param[in] llh Input Array of Latitude, Longitude and Height
 *  \param[out] xyz Output Array of X, Y and Z ECEF coordinates
 */
void llh2xyz(const double *llh, double *xyz) {
    double n;
    double a;
    double e;
    double e2;
    double clat;
    double slat;
    double clon;
    double slon;
    double d, nph;
    double tmp;

    //Get WGS84 ellipsoid radius and eccentricity
    a = WGS84_RADIUS;
    e = WGS84_ECCENTRICITY;
    e2 = e*e;

    clat = cos(llh[0]);
    slat = sin(llh[0]);
    clon = cos(llh[1]);
    slon = sin(llh[1]);
    d = e*slat;

    n = a / sqrt(1.0 - d * d);
    nph = n + llh[2];

    tmp = nph*clat;
    xyz[0] = tmp*clon;
    xyz[1] = tmp*slon;
    xyz[2] = ((1.0 - e2) * n + llh[2]) * slat;

    return;
}

static char *stringrev(char *str) {
    char *p1, *p2;

    if (!str || ! *str)
        return str;

    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}

char* u64toa(uint64_t val, t_u64toa* str) {
#define BASE (10ull) // Base10 by default 
    uint64_t sum;
    int pos;
    int digit;
    int max_len;
    char* res;

    sum = val;
    max_len = U64TOA_MAX_DIGIT;
    pos = 0;

    do {
        digit = (sum % BASE);
        str->data[pos] = digit + '0';
        pos++;

        sum /= BASE;
    } while ((sum > 0) && (pos < max_len));

    if ((pos == max_len) && (sum > 0))
        return NULL;

    str->data[pos] = '\0';
    res = stringrev(str->data);

    return res;
}



/*! \brief Compute the intermediate matrix for conversion from LLH to NEU
 *  \param[in] llh Input position in Latitude-Longitude-Height format
 *  \param[out] t Three-by-Three output matrix
 *  */
void ltcmat(const double *llh, double t[3][3]) {
    double slat, clat;
    double slon, clon;
    
    slat = sin(llh[0]);
    clat = cos(llh[0]);
    slon = sin(llh[1]);
    clon = cos(llh[1]);

    t[0][0] = -slat*clon;
    t[0][1] = -slat*slon;
    t[0][2] = clat;
    t[1][0] = -slon;
    t[1][1] = clon;
    t[1][2] = 0.0;
    t[2][0] = clat*clon;
    t[2][1] = clat*slon;
    t[2][2] = slat;

    return;
}

/* Two complement on data with MSB not equal to tU16 or tU32 */
int two_complements(int len, int data)
{
	int i;
	tU32 mask = 0;

	/* Test value of MSB, If 1 -> sign is - */
	if ( BIT_ISSET(data, len-1))
	{
		/* Generat mask of data len */
		for(i=0; i<len; i++)
			mask |= (1 << i);

		// printf("mask: %X\n",mask);
		/* Return two complement with negative sign */
		return ( ( ~(data - 1) & mask ) * -1);
	}
	else
	{
		return data;
	}
}

/* Extract data in tU8 format in bits*/
tU32 data_extract_tU32 ( tU8 * str_in, int offset_in, int len )
{
	tU8 str_buff_out[4] = {0};
	tU8 str_buff[4] = {0};
	tU8 mask1=0,mask2=0,mask3=0;
	int i,j;
	int offset_index_in;
	int offset_index_out;
	int nbit_offset_in, r_nbit_offset_in;
	int nbit_offset_out,r_nbit_offset_out;
	int nbit_len;
	int len_byte;

	tU32 out = 0;
	int offset_out = 32-len;

	if (len > 32 )
	{
		return 0;
	}

	offset_index_in = floor(offset_in/8);
	nbit_offset_in = offset_in%8;
	r_nbit_offset_in = 8 - nbit_offset_in;

	offset_index_out = floor(offset_out/8);
	nbit_offset_out = offset_out%8;
	r_nbit_offset_out = 8 - nbit_offset_out;
	nbit_len = len%8;
	len_byte = floor(len/8);

	for(j=0; j<r_nbit_offset_in; j++)
		mask1 |= (tU8)(1 << j);

	for(j=7; j>7-nbit_offset_in; j--)
		mask2 |= (tU8)(1 << j);

	for (i=0 ; i<len_byte; i++)
	{
		str_buff[i] |= (tU8)(str_in[offset_index_in+i] & mask1) << nbit_offset_in;

		str_buff[i] |= (tU8)(str_in[offset_index_in+i+1] & mask2) >> (r_nbit_offset_in);
	}


	if ( r_nbit_offset_in < nbit_len )
	{
		str_buff[i] |= (tU8)(str_in[offset_index_in+i] & mask1) << nbit_offset_in;

		for(j=7; j>7 - (nbit_len - r_nbit_offset_in) ; j--)
			mask3 |= (tU8)(1 << j);

		str_buff[i] |= (tU8)(str_in[offset_index_in+i+1] & mask3) >> r_nbit_offset_in;
	}
	else
	{
		for(j=r_nbit_offset_in-1; j>r_nbit_offset_in-1 - nbit_len; j--)
			mask3 |= (tU8)(1 << j);

		str_buff[i] |= (tU8)(str_in[offset_index_in+i] & mask3) << nbit_offset_in;
	}

	if (nbit_len != 0) len_byte++;

	if ( nbit_offset_out == 0 )
	{
		for ( i=0 ; i<len_byte; i++)
			str_buff_out[offset_index_out+i] = str_buff[i];
	}
	else
	{
		mask1=0;
		mask2=0;

		for(j=0; j<nbit_offset_out; j++)
			mask1 |= (tU8)(1 << j);

		for(j=7; j>7-r_nbit_offset_out; j--)
			mask2 |= (tU8)(1 << j);

		for ( i=0 ; i<len_byte; i++)
		{
			str_buff_out[offset_index_out+i] |= (tU8)(str_buff[i] & mask2) >> nbit_offset_out;
			str_buff_out[offset_index_out+i+1] = (tU8)(str_buff[i] & mask1) << r_nbit_offset_out;
		}
	}

	out = str_buff_out[3] | str_buff_out[2] << 8 | str_buff_out[1] << 16 | str_buff_out[0] << 24;

	return out;
}


/* Extract data in tU8 format in bits*/
void data_extract ( tU8 * str_out, const tU8 * str_in,int offset_out, int offset_in, int len )
{
	tU8 str_buff[MAX_CHAR] = {0};
	tU8 mask1=0,mask2=0,mask3=0;
	int i,j;
	int offset_index_in;
	int offset_index_out;
	int nbit_offset_in, r_nbit_offset_in;
	int nbit_offset_out,r_nbit_offset_out;
	int nbit_len;
	int len_byte;

	offset_index_in = floor(offset_in/8);
	nbit_offset_in = offset_in%8;
	r_nbit_offset_in = 8 - nbit_offset_in;

	offset_index_out = floor(offset_out/8);
	nbit_offset_out = offset_out%8;
	r_nbit_offset_out = 8 - nbit_offset_out;
	nbit_len = len%8;
	len_byte = floor(len/8);

	for(j=0; j<r_nbit_offset_in; j++)
		mask1 |= (tU8)(1 << j);

	for(j=7; j>7-nbit_offset_in; j--)
		mask2 |= (tU8)(1 << j);

	for (i=0 ; i<len_byte; i++)
	{
		str_buff[i] |= (tU8)(str_in[offset_index_in+i] & mask1) << nbit_offset_in;

		str_buff[i] |= (tU8)(str_in[offset_index_in+i+1] & mask2) >> (r_nbit_offset_in);
	}


	if ( r_nbit_offset_in < nbit_len )
	{
		str_buff[i] |= (tU8)(str_in[offset_index_in+i] & mask1) << nbit_offset_in;

		for(j=7; j>7 - (nbit_len - r_nbit_offset_in) ; j--)
			mask3 |= (tU8)(1 << j);
		str_buff[i] |= (tU8)(str_in[offset_index_in+i+1] & mask3) >> r_nbit_offset_in;
	}
	else
	{
		for(j=r_nbit_offset_in-1; j>r_nbit_offset_in-1 - nbit_len; j--)
			mask3 |= (tU8)(1 << j);
		str_buff[i] |= (tU8)(str_in[offset_index_in+i] & mask3) << nbit_offset_in;
	}

	if (nbit_len != 0) len_byte++;

	if ( nbit_offset_out == 0 )
	{
		for ( i=0 ; i<len_byte; i++)
			str_out[offset_index_out+i] = str_buff[i];
	}
	else
	{
		mask1=0;
		mask2=0;

		for(j=0; j<nbit_offset_out; j++)
			mask1 |= (tU8)(1 << j);

		for(j=7; j>7-r_nbit_offset_out; j--)
			mask2 |= (tU8)(1 << j);

		for ( i=0 ; i<len_byte; i++)
		{
			str_out[offset_index_out+i] |= (tU8)(str_buff[i] & mask2) >> nbit_offset_out;
			str_out[offset_index_out+i+1] = (tU8)(str_buff[i] & mask1) << r_nbit_offset_out;
		}
	}
}

/* Convert Char read in the file in tU8 */
void chartotU8( tU8 *out, char *in, int len){

  unsigned char tmp[2];
  int i,j;
  
  for(i=0;i<len;i++)
  {
    for(j=0;j<2;j++)
    {
      switch ( in[i*2+j] )
      {
        case 'A':
          tmp[j] = 10;
          break;
        case 'B':
          tmp[j] = 11;
          break;
        case 'C':
          tmp[j] = 12;
          break;
        case 'D':
          tmp[j] = 13;
          break;
        case 'E':
          tmp[j] = 14;
          break;
        case 'F':
          tmp[j] = 15;
          break;
        default:
          tmp[j]=in[i*2+j]-48;
          break;
      }
    }
    out[i] = (16*tmp[0]) + tmp[1];
  }
}

