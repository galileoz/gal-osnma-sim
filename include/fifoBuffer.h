
/**
 * @file   fifoBuffer.h
 * @author: FDC
 * @brief first in first out Buffer definition
 * 
 * this buffer is used for syncrhonisation and transmission between samp generation and hackrf Transfer fonciton
 * 
 * 
 * usage
 * =====
 * 
 * generation thread
 * -------------
 * @code
 * initBuffer(length);
 * 
 * while(1){
 *      fifoData_t data[maxlength]//maxlength may be variable
 *      
 *      while(!bufferHaveSpace(len)) { /// optional
 *          // do somthing
 *      }
 * 
 *      bufferPush(len, data); //len \< maxlength .
 *  
 *  }
 * 
 * @endcode
 * 
 * transmission thread
 * -------------------
 * 
 * @code 
 *  while(1){
 *      fifoData_t data[maxlength];
 *      bufferpush(data, len); // len\< maxlength
 *      // bufferpush only return if buffer have enough data
 * }
 * @endcode
 * 
 * Created on 19 février 2018, 15:34
 */

#ifndef FIFOBUFFER_H
#define FIFOBUFFER_H

#include <stdint.h>

///@brief data type in fifo buffer
typedef short fifoData_t;

/**
 * @brief initalize fifo buffer 
 * @param length max size of buffer
 * @note fifo buffer use global variable define in fifoBuffer.c
 *       only one buffer can be used in a programe
 */
void initBuffer(int length);

/**
 * @brief check if fifo buffer have space for nsamp new samp
 * @param nsamp number of fifoData_t samp
 * @return 1 if buffer have space
 * @note if buffer is full, fonction return olny after bufferPull() call or 1second
 */
int bufferHaveSpace(int nsamp);

/**
 * @brief push data in fifo buffer
 * @param nsamp number of data
 * @param inBuffer input data
 * @note fonction block if buffer have not space
 */
void bufferPush(int nsamp, fifoData_t inBuffer[nsamp]);
/**
 * @brief read data in fifo buffer
 * @param buffer output buffer
 * @param nBytes number of element to read
 * @note fonction block if buffer do not have data
 */
void bufferPull(int nBytes, uint8_t buffer[nBytes]);



#endif /* FIFOBUFFER_H */
