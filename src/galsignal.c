/* 
 * File:   signal.c
 * @author: FDC
 * 
 * Created on 8 février 2020, 13:59
 */

#include <conversion.h>
#include "galsignal.h"
#include "CRC_CODE.h"
#include "PRN_CODE.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "galWord.h"

#define K_CNV_ENCD 7

void cnv_encd(long input_len, int *in_array, int *out_array) {

    int m = K_CNV_ENCD - 1;
    int *unencoded_data; /* pointer to data array */
    int shift_reg[K_CNV_ENCD]={0}; /* the encoder shift register */


    // Allocate space for the zero-padded input data array
    unencoded_data = calloc((input_len + m),  sizeof (int));
    if (unencoded_data == NULL) {
        printf("\ncnv_encd.c: Can't allocate enough memory for unencoded data!  Aborting...");
        exit(1);
    }

    // Read in the data and store it in the array
    for (int t = 0; t < input_len; t++)
        unencoded_data[t] = in_array[t];

    // Zero-pad the end of the data 
    for (int t = 0; t < m; t++) {
        unencoded_data[input_len + t] = 0;
    }

    // Initialize the shift register 
    for (int j = 0; j < K_CNV_ENCD; j++) {
        shift_reg[j] = 0;
    }

    /* To try to speed things up a little, the shift register will be operated
       as a circular buffer, so it needs at least a head pointer. It doesn't
       need a tail pointer, though, since we won't be taking anything out of
       it--we'll just be overwriting the oldest entry with the new data. */

    int sr_head = 0; /* index to the first elt in the Shift register */

    // Initialize the channel symbol output index 

    /*---- Now start the encoding process -----*/

    // Compute the upper and lower mod-two adder outputs, one bit at a time 
    for (int t = 0; t < input_len ; t++) {
        shift_reg[sr_head] = unencoded_data[t];
        int G1, nG2;/* the upper and lower xor gate outputs */
        G1 = 0;
        nG2 = 0; /// @note ICD fig.13
        for (int j = 0; j < K_CNV_ENCD; j++) {
            int iSr = (j + sr_head) % K_CNV_ENCD;
            G1 ^= shift_reg[iSr] & ccGenMatrix[0][j];
            nG2 ^= shift_reg[iSr] & ccGenMatrix[1][j];
        }

        // Write the upper and lower xor gate outputs as channel symbols 
        out_array[2*t] = G1;
        out_array[2*t+1] = 1-nG2; 


        sr_head -= 1; // Equivalent to shifting everything right one place
        if (sr_head < 0) // But make sure we adjust pointer moduloK_CNV_ENCDK
            sr_head = m;

    }

    // Free the dynamically allocated array
    free(unencoded_data);

}
 
/*! block interleave
 * \brief compute block interleave of input vector
 * @param [in] in input bits
 * @param [in] row rows (where data is read)
 * @param [in] col columns (where data is written)
 * @param [out] out interleaved bits
 * @note  maxbin > row*col 
 */
void interleave(const int *in, int row, int col, int *out) {
    int r, c;
    int tmp[240];

    memcpy(tmp, in, sizeof (int)*row * col);

    for (r = 0; r < row; r++) {
        for (c = 0; c < col; c++) {
            out[r * col + c] = tmp[c * row + r];
        }
    }
}

/**
 *  \brief Add the synchronisation pattern to the I/NAV page part
 *  @param[in] in input bits
 *  @param[out] out  output bits (symbols)
 */
void add_sync(int *in, int *out) {
    const int sync_pattern[10] = {0, 1, 0, 1, 1, 0, 0, 0, 0, 0};
    int i;

    for (i = 0; i < 10; i++)
        out[i] = sync_pattern[i];
    for (i = 0; i < 240; i++)
        out[i + 10] = in[i];
}

/**
 * \brief BOC modulation function
 * 
 * binary offset carrier (BOC)
 * @param boccode BOC modulated code
 * @param code GNSS code (-1 or 1)
 * @param len code length
 * @param m sub-carrier frequency/1.023MHz
 * @param n chip frequency/1.023MHz
 */
void boc(short *boccode, short *code, int len, int m, int n) {
    int i, j, N = 2 * m / n;
    for (i = 0; i < len; i++)
        for (j = 0; j < N; j++)
            boccode[N * i + j] = code[i];

    //Mix sub carrier
    for (i = 0; i < N * len / 2; i++) {
        boccode[2 * i] = -boccode[2 * i];
    }
    return;
}



/*! \brief generate the  primaryCode sequence for a given Space Vehicle PRN, for E1-B signal
 *  \param[in] svId svId number of the Space Vehicle
 *  \param[out] primaryCode_E1B char array of 4092 bytes (-1 or 1 with -1<=>1 and 1<=>0)
 */
void galcodegen_E1B(short *primaryCode_E1B, int svId) {
    if (svId < 0 || svId >= N_PRN) {
        printf("\nE1B Memory code not available for PRN %d! \n", svId);
        return;
    }

    //Converts each hexadecimal character to binary number encoded on 4 bits
    for (int k = 0; k < GAL_SEQ_LEN / 4; k++)
    {
        for (int i = 0; i < 4; i++)
        {
            primaryCode_E1B[4 * k + 3 - i] = ((PRN_E1B[svId][k] & (1 << i)) > 0 ? -1 : 1);
        }
    }
    return;
}


/*! \brief generate the  primaryCode sequence for a given Space Vehicle PRN, for E1-C signal
 *  \param[in] svID PRN number of the Space Vehicle
 *  \param[out] primaryCode_E1C char array of 4092 bytes (-1 or 1 with -1<=>1 and 1<=>0)
 */
void galcodegen_E1C(short *primaryCode_E1C, int svID) {
    if (svID < 0 || svID >= N_PRN) {
        printf("\nE1C Memory code not available for PRN %d! \n", svID+1);
        return;
    }

    //Converts each hexadecimal character to binary number encoded on 4 bits
    for (int k = 0; k < GAL_SEQ_LEN / 4; k++)
    {
        for (int i = 0; i < 4; i++)
        {
            primaryCode_E1C[4 * k + 3 - i] = (((PRN_E1C[svID][k] & (1 << i)) > 0)? -1 : 1);
        }
    }
}
