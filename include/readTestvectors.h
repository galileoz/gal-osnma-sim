
/** 
 * @file   readTestvectors.h
 * @author fdc
 * 
 *
 * Created on 16.02.2023
 *  
 */

#ifndef READTESTVECTORS_H
#define READTESTVECTORS_H

/*****************************************************************************
   Include
 *****************************************************************************/
#include "timeType.h"
#include "ephemgal.h"
#include "type.h"
#include "gal-sdr-sim.h"
#include <stdio.h>

/*****************************************************************************
   Typedefs and define
 *****************************************************************************/

#define EPH_SCALE_FACTOR_CRS       pow(2,-5)
#define EPH_SCALE_FACTOR_DELTAN    pow(2,-43)
#define EPH_SCALE_FACTOR_M0        pow(2,-31)
#define EPH_SCALE_FACTOR_CUC       pow(2,-29)
#define EPH_SCALE_FACTOR_E         pow(2,-33)
#define EPH_SCALE_FACTOR_CUS       pow(2,-29)
#define EPH_SCALE_FACTOR_ROOT_A    pow(2,-19)
#define EPH_SCALE_FACTOR_CIC       pow(2,-29)
#define EPH_SCALE_FACTOR_OMEGA0    pow(2,-31)
#define EPH_SCALE_FACTOR_CIS       pow(2,-29)
#define EPH_SCALE_FACTOR_I0        pow(2,-31)
#define EPH_SCALE_FACTOR_CRC       pow(2,-5)
#define EPH_SCALE_FACTOR_PERIGEE   pow(2,-31)
#define EPH_SCALE_FACTOR_OMEGA_DOT pow(2,-43)

#define GAL_EPH_SCALE_FACTOR_TOE       60
#define GAL_EPH_SCALE_FACTOR_AF0       pow(2,-34)
#define GAL_EPH_SCALE_FACTOR_AF1       pow(2,-46)
#define GAL_EPH_SCALE_FACTOR_AF2       pow(2,-59)
#define GAL_EPH_SCALE_FACTOR_TOC       60
#define GAL_EPH_SCALE_FACTOR_BDGE1E5   pow(2,-32)

#define GPS_EPH_SCALE_FACTOR_IDOT      pow(2,-43)

/*****************************************************************************
   Global variable
 *****************************************************************************/

/*****************************************************************************
   Function prototypes
 *****************************************************************************/
int readTestvectToEph(ephemgal_t * eph, FILE *fp, galileotime_t * gal, int * tmax);
void testvect_update_navm( tU8 navm_testvect[MAX_SAT][SIZEOF_NAV_MSG_TU8], FILE* fp, unsigned long tow);

#endif /* READTESTVECTORS_H */
