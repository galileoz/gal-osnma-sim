
/**
 *  @brief type conversion fonction
 * @file:   conversion.h
 * @author: FDC
 *
 * Created on 8 février 2018, 11:52
 */

#ifndef CONVERSION_H
#define CONVERSION_H
#include "type.h"
#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>

/*****************************************************************************
   Typedefs and define
 *****************************************************************************/

#define U64TOA_MAX_DIGIT (31)

typedef struct {
    char data[U64TOA_MAX_DIGIT + 1];
} t_u64toa;

#define BIT_ISSET(var, n) !!((tU32)(var) & (1 << (n)))


/*****************************************************************************
   Function prototypes
 *****************************************************************************/

void ltcmat(const double *llh, double t[3][3]);


void subVect(double *y, const double *x1, const double *x2);

double normVect(const double *x);

double dotProd(const double *x1, const double *x2);


int replaceExpDesignator(char *str, int len);

void ecef2neu(const double *xyz, double t[3][3], double *neu);

void neu2azel(double *azel, const double *neu); /// refactor

void xyz2llh(const double *xyz, double *llh);

void llh2xyz(const double *llh, double *xyz);

//int parse_u64(char* s, uint64_t * const value);
//static float TimevalDiff(const struct timeval *a, const struct timeval *b);
int parse_u32(char* s, uint32_t * const value);
//static char *stringrev(char *str) ;
char* u64toa(uint64_t val, t_u64toa* str);

int two_complements(int len, int data);
tU32 data_extract_tU32 (tU8 * str_in, int offset_in, int len );
void data_extract( tU8 * str_out, const tU8 * str_in,int offset_out, int offset_in, int len );
void chartotU8( tU8 *out, char *in, int len);

#endif /* CONVERSION_H */
