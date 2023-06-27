
/**
 * @file   signalControle.h
 * @brief manage interuption signal
 * 
 * catch système sygnal and synchonized staop of all thread
 * 
 * usage
 * =====
 * 
 * @code
 * 
 *  // in main thread before thread start 
 *  initInteruptionSignal();
 * 
 *  //in all thread
 *  while (!signalExit()){
 *      if(exit_condition) quit();
 *      
 *      // main thread code
 *  }
 *  quit(); // use quit() at the loop end for stop all thread in case of unexpected loop breack
 * 
 * @endcode
 * 
 * @author: FDC
 *  
 * Created on 16 février 2018, 10:26
 */

#ifndef signalCONTROLE_H
#define signalCONTROLE_H


/**
 * @brief test if exit signal are catch
 * @return true if exit signal
 */
int signalExit();

/**
 * init signal interution catch
 */
void initInteruptionSignal();

/**
 * @brief send exit signal
 */
void quit();

#endif /* signalCONTROLE_H */


