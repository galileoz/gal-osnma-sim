
/**
 * @brief receiver structure definition
 * 
 * 
 * @file   receiver.h
 * @author: FDC
 * @section receiverUsage usage
 * 
 * ```
 * receiver_t receiver = INIT_receiver;
 * receiver*=receiverINIT(receiver*);
 * receiver.mt= motionType // see motionType.h
 * 
 * while(1){ // every DELTA_T
 * 
 * t= getTime(receiver); 
 * p= receiver.currentPosition;
 * 
 * receiverUpdate(*receiver)
 * }
 * ```
 * Created on 6 avril 2018, 18:08
 */

#ifndef RECEIVER_H
#define RECEIVER_H
#include <pthread.h>
#include <stdio.h>
#include "timeType.h"
#include "motionType.h"

/** @brief value return by receiverUpdate()
 */
enum receiverUpdateReturn {
             TIME_OVER = 1, ///<@brief tx time \> receiver_t.tmax
             LAST_POS = 2,  ///<@brief receiver is in the last position @warning not used
             RECEIVER_UPDATED = 0 ///<@brief receiver update normaly
}; 


/** @brief receiver structure definition
 * 
 * receiver position, time 
 * @ingroup main_structure
 * @see receiver.h
 */
typedef struct receiver_s{
    pthread_mutex_t lock; 
    int numd;   /**< numbre of postition*/
    int tmax;   /**< duration of transmition*/
    ecef_t currentPosition; /**<@brief current receiver position. 
                             * updated in receiverUpdate()
                             * */
    motionType_t type;  ///< motion type 
    galileotime_t gal0;  ///< galileo time of transmission begenig
    long txtime;        ///<@brief time of transmission (in DELTA_T) 
    FILE* log;          ///<@brief gal-sdr-sim debug output file @todo move
    motionStr_t* mt;    ///<@brief receiver motion definition
    ionoutc_t ionoutc;
}receiver_t;


/** @brief initalise receiver_t to 0*/
#define INIT_receiver {{0},0,0,{0,0,0},STATIC}

galileotime_t getTime(receiver_t);
void getTimePrint(char* msg, receiver_t receiver);

receiver_t* receiverINIT(receiver_t* receiver);

enum receiverUpdateReturn receiverUpdate(receiver_t* receiver);

#endif /* RECEIVER_H */
