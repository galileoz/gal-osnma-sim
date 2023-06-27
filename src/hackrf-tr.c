/* 
 * File:   hackrf-tr.c
 * @author: FDC
 * 
 * Created on 15 février 2020, 12:13
 */
 
#include "outputConf.h"

#ifdef HACKRFLINKED

#include "hackrf-tr.h"
#include <stdio.h>
#include <stdlib.h>
#include <signalControl.h>
#include "conversion.h"


/** @brief hackrf device
 *
 *  global var in hackrf-tr.c
 */
static hackrf_device* _device = NULL; 
// overide fonction
int tx_callback(hackrf_transfer* transfer);

typedef enum {
    HW_SYNC_MODE_OFF = 0,
    HW_SYNC_MODE_ON = 1,
} hw_sync_mode_t;


#ifdef HACKRFLINKED
t_u64toa ascii_u64_data1; // global variable
#endif


int hackrfIsStream(){
    return hackrf_is_streaming(_device);
}

int initHackrf(hackrfConf_t conf){
    int result;
    const char* serial_number = NULL;
    result = hackrf_init();
    if (result != HACKRF_SUCCESS) {
        fprintf(stdout, "hackrf_init() failed: %s (%d)\n", hackrf_error_name(result), result);
        return EXIT_FAILURE;
    }

    result = hackrf_open_by_serial(serial_number, &_device);
    if (result != HACKRF_SUCCESS) {
        fprintf(stdout, "hackrf_open() failed: %s (%d)\n", hackrf_error_name(result), result);
        return EXIT_FAILURE;
    }
    
    fprintf(stdout, "call hackrf_set_sample_rate(%u Hz/%.03f MHz)\n", conf.sample_rate_hz, ((float) conf.sample_rate_hz / (float) FREQ_ONE_MHZ));
    result = hackrf_set_sample_rate(_device, conf.sample_rate_hz);
    if (result != HACKRF_SUCCESS) {
        fprintf(stdout, "hackrf_set_sample_rate() failed: %s (%d)\n", hackrf_error_name(result), result);
        return EXIT_FAILURE;
    }

    if (result != HACKRF_SUCCESS) {
        fprintf(stdout, "hackrf_start_?x() failed: %s (%d)\n", hackrf_error_name(result), result);
        return EXIT_FAILURE;
    }

    // fprintf(stdout, "call hackrf_set_hw_sync_mode(%d)\n", conf.hw_sync_enable);
    result = hackrf_set_hw_sync_mode(_device, conf.hw_sync_enable ? HW_SYNC_MODE_ON : HW_SYNC_MODE_OFF);
    if (result != HACKRF_SUCCESS) {
        fprintf(stdout, "hackrf_set_hw_sync_mode() failed: %s (%d)\n", hackrf_error_name(result), result);
        return EXIT_FAILURE;
    }


    result = hackrf_set_txvga_gain(_device, conf.txvga_gain);
    result |= hackrf_start_tx(_device, &tx_callback, NULL);

    if (result != HACKRF_SUCCESS) {
        fprintf(stdout, "hackrf_start_?x() failed: %s (%d)\n", hackrf_error_name(result), result);
        return EXIT_FAILURE;
    }


    fprintf(stdout, "call hackrf_set_freq(%s Hz/%.03f MHz)\n",
            u64toa(conf.freq_hz, &ascii_u64_data1), ((double) conf.freq_hz / (double) FREQ_ONE_MHZ));

    result = hackrf_set_freq(_device, conf.freq_hz);
    if (result != HACKRF_SUCCESS) {
        fprintf(stdout, "hackrf_set_freq() failed: %s (%d)\n", hackrf_error_name(result), result);
        return EXIT_FAILURE;
    }

    fprintf(stdout, "call hackrf_set_amp_enable(%u)\n", conf.amp_enable);
    result = hackrf_set_amp_enable(_device, (uint8_t) conf.amp_enable);
    if (result != HACKRF_SUCCESS) {
        fprintf(stdout, "hackrf_set_amp_enable() failed: %s (%d)\n", hackrf_error_name(result), result);
        return EXIT_FAILURE;
    }


    fprintf(stdout, "call hackrf_set_antenna_enable(%u)\n", conf.antenna_enable);
    result = hackrf_set_antenna_enable(_device, (uint8_t) conf.antenna_enable);
    if (result != HACKRF_SUCCESS) {
        fprintf(stdout, "hackrf_set_antenna_enable() failed: %s (%d)\n", hackrf_error_name(result), result);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

void closeHackrf (){
        if (_device != NULL) {
        int result;
        result = hackrf_stop_tx(_device);
        if (result != HACKRF_SUCCESS) {
            fprintf(stdout, "hackrf_stop_tx() failed: %s (%d)\n", hackrf_error_name(result), result);
        } else {
            fprintf(stdout, "hackrf_stop_tx() done\n");
        }

        result = hackrf_close(_device);
        if (result != HACKRF_SUCCESS) {
            fprintf(stdout, "hackrf_close() failed: %s (%d)\n", hackrf_error_name(result), result);
        } else {
            printf( "hackrf_close() done\n");
            //hackrf_exit may segfault
            hackrf_exit();
        }
        fprintf(stdout, "hackrf_exit() done\n");
    }
}

void hackfrPush(int nsamp, fifoData_t inBuffer[]){
    while (!bufferHaveSpace(nsamp)){ // wait for space

        if (signalExit()) return ;
        enum hackrf_error re = hackrf_is_streaming(_device);
         if (re!=HACKRF_TRUE){
                ///@todo try to reactive hackRF
                printf(" fifo push HACKRF is not Stram  : %i\n", re);
                quit();
                return ;        
            }
        } 
    bufferPush(nsamp,inBuffer);
}

/**@brief hackrf transfer callback 
 * 
 * fonction call in libusb ansyncrone mod
 * @param transfer 
 * @return 
 * @todo: add fwirte
 */
int tx_callback(hackrf_transfer* transfer) {
        bufferPull(transfer->valid_length, transfer->buffer);

    return 0;
}

#endif /* HACKRFLINKED */
