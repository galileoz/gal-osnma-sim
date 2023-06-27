/** @brief  main fonciton definition
 * @file   gal-sdr-sim.h
 * @author: FDC
 */
#ifndef GAL_SDR_SIM_H
#define GAL_SDR_SIM_H

/*****************************************************************************
   Include
 *****************************************************************************/
#include "satListType.h"
#include "receiver.h"
#include "satDataType.h"
#include "outputConf.h"

/*****************************************************************************
   Global variable
 *****************************************************************************/
extern int verbose;

/*****************************************************************************
   Function prototypes
 *****************************************************************************/
/**
 * @brief read arg and init main programe structure
 * @ingroup run_commande
 * @see main_structure
 * @param[in] argc number of argument
 * @param[in] argv argument list
 * @param[out] satData main satData_t struct
 * @param[out] receiver  main receiver_t strcut
 * @param[out] outputConf main bufferConf_t struct
 */
void readOption(int argc, char** argv, satData_t* satData, receiver_t* receiver, outputConf_t* outputConf);

/**
 * @ingroup run_commande
 * @brief gal-sdr-sim uage
 * 
 * print cmd line option 
 * 
 */
void usage(void);
    
/** @brief main signal generation fonction
 * 
 * creat sample for @ref DELTA_T sec. and send it
 * 
 * @param[in] outputConf  output configuration option
 * @param[in] sat channel liste
 */
void genSampl(outputConf_t outputConf, satList_t* sat);


#endif 
