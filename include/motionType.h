
/**
 * @file   motionType.h

 * 
 * @brief receiver motion description 
 * 
 *  @section usage
 * 
 *  ```
 *  motionStr_t* mt = motionTypeInit();
 *  if (type==STATIC) {
 *      setStatic(mt, p0)
 *  }else if (type==STRAIGHT){
 *      setStraight(mt, p0,v0)
 *  }else if(type==USERMOTION) {
 *      setUserMotion(mt, filename)
 *  }else if (type==GGA){
 *      setGGA(mt, filname)
 *  }
 *  while(1){
 *      getPos(pos,t,mt)
 *      t+=DELAT_T
 *  }    
 *    
 *  ```
 * @author: FDC
 * 
 * Created on 25 juin 2018, 14:32
 */

#ifndef MOTIONTYPE_H
#define MOTIONTYPE_H

#include "type.h"

/** @brief receiver motion type
 */
typedef enum motionType_t{
    STATIC, ///<@brief static possition 
}motionType_t;

/** @brief structur contain list of receiver position.
 *  @see motionType.h
 */
typedef struct motionStr_s{
    int numd;       ///<@brief numbre of element 
    ecef_t* pList;  /**<@brief list ofposition
                     * 
                     * numd+1 position are store. 2 last position are egal to
                     * alow static motion ate the end of list 
                     */
    double* tList;  /**<@brief list of time
                     * 
                     * numd+1 time recorde 
                     * @note time in second relative gal0
                     */
    
    motionType_t type;
    
}motionStr_t;
#define INIT_MOTIONsTR {0,NULL,NULL,STATIC}

/**
 * @brief initialize motionStr_t structur
 * @return motionStr_t
 * @warning default value not correct.
 *          motionStr_t should be updated with setStatic(), setStraight(), setUserMotion() or setGGA()
 */
motionStr_t* motionTypeInit();

/**@brief return position for time = tx
 * 
 * @param[out] pos return possition
 * @param[in] tx  time 
 * @param[in] mt motionStr_t
 */

void getPos(ecef_t pos, const double tx, const motionStr_t* mt);

/**
 * @brief configure motionStr_t as Static location
 * @param[out] mt motionStr_t
 * @param[in] p0 receiver position
 * @note previous configuration while be remove
 */
void setStatic(motionStr_t* mt,const ecef_t p0);

#endif /* MOTIONTYPE_H */
