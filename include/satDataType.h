

/**
 * 
 * data of Galileo satellite constelation. 
 * @file   satDataType.h
 * @author: FDC
 *
 * Created on 6 avril 2018, 18:17
 */

#ifndef SATDATATYPE_H
#define SATDATATYPE_H
#include "ephemgal.h"
#include "type.h"
#include <stdio.h>


/** @brief  ephemeris and almanac data
 *  @ingroup main_structure
 *  @see ephemgal.h almagal.h
 */
typedef struct satData_t{
        int neph;       /**<@brief number of ephemeris*/
        int iodCmpt; /**<@brief update iod when cmp=0*/
        ephemgal_t eph[MAX_SAT];    /**< list of ephemeris*/
        FILE * testvectFile;

}satData_t;

/** @def INTI_SATDATA_T @brief initialise satData_t structure */
#define INIT_SATDATA_T {0,UPDATE_EPH_TIME,{{0}}, NULL}

/** @brief comput almanac  from ephemeris
 *  @param[in,out] satData satData_t struct
 *  @note previoous almanac will delet
 */
void eph2alm(satData_t* satData);


#endif /* SATDATATYPE_H */
