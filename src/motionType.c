
/** 
 * @file   motionType.c
 * @author: FDC
 * 
 * Created on 25 juin 2020, 14:32
 */

#include <conversion.h>
#include "motionType.h"
#include "constants.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


motionStr_t* motionTypeInit(){
    motionStr_t* mt =(motionStr_t*)malloc(sizeof(motionStr_t));
    mt->numd=0;
    mt->pList=(ecef_t*) malloc(sizeof(ecef_t));
    mt->tList=(double*) malloc(sizeof(double));
    mt->type=STATIC;
    
    return mt;
}

/**
 * @brief comptPos receiver position
 * @param[out] pos receiver position
 * @param[in] tx time
 * @param[in] p0 last crossing point position
 * @param[in] v0 speed between tow crossing point
 * @param[in] t0 time of crossing point
 * @note P(tx)=P(t0)+V*(tx-t0)
 */
static inline void comptPos(ecef_t pos, double tx, const ecef_t p0, const ecef_t v0, double t0){
        pos[0]=p0[0]+v0[0]*(tx-t0);
        pos[1]=p0[1]+v0[1]*(tx-t0);
        pos[2]=p0[2]+v0[2]*(tx-t0);
}

void getPos(ecef_t pos, double tx, const motionStr_t* mt) {
    ecef_t p0={0};
    ecef_t v0={0};
    double t0=0;
    
	ecefCp(p0, mt->pList[0]);
            
    comptPos(pos, tx, p0, v0, t0);
}

void setStatic(motionStr_t* mt, const ecef_t p0){
    mt->type=STATIC;
    mt->numd = 1;
    free(mt->pList);
    free(mt->tList);
    mt->tList=NULL;

    mt->pList=(ecef_t*) malloc(sizeof(ecef_t));
    ecefCp(mt->pList[0], p0);
}

