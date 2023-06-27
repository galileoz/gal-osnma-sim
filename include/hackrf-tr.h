
#include "outputConf.h"

#ifdef HACKRFLINKED
/** @brief hackRF direct transfer fonction
 * @file:   hackrf-tr.h
 * @author: FDC
 *
 * Created on 15 février 2018, 12:13
 */

#ifndef HACKRF_TR_H
#define HACKRF_TR_H

#include <libhackrf/hackrf.h>
//#include "type.h"
#include <stdbool.h>
#include "fifoBuffer.h"

#include <sys/time.h>



///@brief 1e6
#define FREQ_ONE_MHZ (1000000ll)

/** @brief hackRF configuraiton structur
 */
struct hackrfConf_t{
    bool hackrf_transfer;   ///<@brief set when hackRF transfer si enable
    int64_t freq_hz;        /**<@brief hackRF center frequency*/
    uint32_t sample_rate_hz;    
    uint32_t antenna_enable ;
    uint32_t amp_enable ;
    unsigned int txvga_gain ;
    uint32_t hw_sync_enable ; 
};

typedef struct hackrfConf_t hackrfConf_t;

#define HACKRFCONF_DEFAULT    { \
false,      /*hackRF_transfer*/ \
1575420000, /*freq_hz*/         \
2600000, /*sample_rate_hz*/     \
0,       /*antenna_enable*/     \
1,         /*amp_enable*/       \
2,          /*txvga_gain*/      \
0}       /*hw_sync_enable*/  


#if defined _WIN32
#define sleep(a) Sleep( (a*1000) )
#endif





int initHackrf(hackrfConf_t conf);
void hackfrPush(int nsamp, fifoData_t inBuffer[]);
void closeHackrf ();

// private fonciton
float TimevalDiff(const struct timeval *a, const struct timeval *b);
int hackrfIsStream();






#endif /* HACKRF_TR_H */
#endif /* HACKRFLINKED */
