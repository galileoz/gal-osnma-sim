
/**
 * @file   outputConf.h
 * @author: FDC
 * @brief sample outuput configuration
 * Created on 14 juin 2018, 18:08
 */

#ifndef OUTPUTCONF_H
#define OUTPUTCONF_H
#include "constants.h"
#include <stdio.h>

#ifdef HACKRFLINKED
#include "hackrf-tr.h"
#endif
/** @brief type of data (complexe or reel) */
typedef enum dataType_t {
    I_dataType, ///< reel data type
    IQ_dataType ///< complexe data type
} dataType_t;

/** @brief format of data (8bits or 12 Bits)*/
typedef enum dataFormat_t {
	SC16 = 12,  ///<16 bits format for BladeRF
	SC08 = 8,   ///< 8 bits format for HackRF
	SC06 = 6	///< 6 bits format for Labsat
} dataFormat_t;
/**
 * @brief output configuration strucutre
 * @ingroup main_structure
 * @todo renam outputConf
 */
typedef struct outputConf_s {
    dataType_t dataType; ///<@brief I or I/Q
    double samp_freq; ///<@brief sample frequency
    int maxSize;      ///< size of iq_buff @note link to samp_freq & delt
    dataFormat_t format; ///<@brief 8 bit or 12bit
    double delt; ///< time between pseaudo range @ref DELTA_T
    short *iq_buff; ///<@brief temporary buffer

#ifdef HACKRFLINKED
    hackrfConf_t hackrf; ///<@brief hack RF configuration
#endif
    FILE *fp;   ///<@brief sample output file 
}outputConf_t;

#ifdef HACKRFLINKED
	#define INIT_OUTPUT_CONF {IQ_dataType,0,0,SC08,0,NULL,HACKRFCONF_DEFAULT,NULL}
#else
	#define INIT_OUTPUT_CONF {IQ_dataType,0,0,SC08,0,NULL,NULL}
#endif
#endif /* OUTPUTCONF_H */
