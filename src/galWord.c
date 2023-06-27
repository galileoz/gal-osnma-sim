
/*
 * @file   galWord.c
 * @author: FDC
 *
 * Created on 29 mars 2020, 11:14
 */

#include <conversion.h>
#include "galWord.h"
#include "galsignal.h"
#include <stdbool.h>
#include <stdio.h>



void genTrame(hPage_t page, int* trame){

    int fec_sbf_tmp[2 * N_BIT_PAGE] = {0};
    cnv_encd(N_BIT_PAGE, page, fec_sbf_tmp); //N_BIT_PAGE = 120


    ////////////////////////////////////
    // interleave
    /////////////////////////////////////
    int interleave_sbf[2 * N_BIT_PAGE] = {0}; //Subframe to store the interleaved data
    //Interleave
    interleave(fec_sbf_tmp, 8, 30, interleave_sbf);


    //Add synchronization pattern
    int final_sbf[2 * N_BIT_PAGE + 10] = {0};
    //Final subframe containing all the required data for subframe 1
    //unsigned long tow = ((unsigned long) gal0.tow);
    add_sync(interleave_sbf, final_sbf);



    /*Store the final pages of the subframe
     into the structure representing a Galileo channel */
    for (int j = 0; j < 250; j++) {
        trame[j] = final_sbf[j];
    }
}

