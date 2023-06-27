/**
 * @file  receiver.c
 * @author: FDC
 * 
 * Created on 6 avril 2020, 18:08
 */

#include <conversion.h>
#include "receiver.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "timeType.h"

receiver_t* receiverINIT(receiver_t* receiver){
    pthread_mutex_init(&receiver->lock, NULL);
    receiver->gal0 = INIT_GALILEOTIME_T;
    receiver->numd=0;
    receiver->tmax=0;
    receiver->type=STATIC;
    receiver->txtime=0;        
         receiver->mt=motionTypeInit();
    
    return receiver;
}

enum receiverUpdateReturn receiverUpdate(receiver_t* receiver){
    receiver->txtime++;

    if(receiver->tmax>0&&receiver->txtime*DELTA_T>=receiver->tmax){
        return TIME_OVER;
    }


    double tx =receiver->txtime*DELTA_T;
    getPos(receiver->currentPosition, tx, receiver->mt);
#if PRINT_POS
    printf("x:%f,Y: %f, z:%f tx %lf\n",receiver->currentPosition[0], receiver->currentPosition[1], receiver->currentPosition[2],tx);
#endif
    return RECEIVER_UPDATED;
}


galileotime_t getTime(receiver_t receiver){
    double dt = receiver.txtime*DELTA_T;
    double t =receiver.gal0.wn*SECONDS_IN_WEEK+ receiver.gal0.tow + dt;
    galileotime_t time={0,0};
    time.wn=t/SECONDS_IN_WEEK;
    time.tow=t-time.wn*SECONDS_IN_WEEK;
    return time;
}
