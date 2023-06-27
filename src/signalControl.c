/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   signalControle.c
 * @author: FDC
 * 
 * Created on 16 février 2020, 10:26
 */

#include "signalControl.h"
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>

static volatile sig_atomic_t do_exit= false;

#ifdef _MSC_VER

BOOL WINAPI
sighandler(int signum) {
    if (CTRL_C_EVENT == signum) {
        fprintf(stdout, "Caught signal %d\n", signum);
        do_exit = true;
        return TRUE;
    }
    return FALSE;
}
#else
void quit(){
    do_exit = 1;
}

void sigint_callback_handler(int signum) {
    fprintf(stdout, "Caught signal %d, %i\n", signum,do_exit);
    do_exit = 1;
}
#endif

void initInteruptionSignal(){
#ifdef _MSC_VER
    SetConsoleCtrlHandler((PHANDLER_ROUTINE) sighandler, TRUE);
#else
    signal(SIGINT, &sigint_callback_handler);
#endif
}
    



int signalExit (){
    return do_exit;
}


