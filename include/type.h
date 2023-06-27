/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   type.h
 * @author: FDC
 *
 * Created on 8 février 2018, 12:20
 */

#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
/**
 *  @brief type of coordone in Earth-centered Earth-fixed
 *  @note   llh_t[0]=X
 *          llh_t[1]=Y
 *          llh_t[2]=Z
 *  @todo use structure
 *      typedef struct ecef_t{
 *          double x;
 *          double y;
 *          double z;
 *          }ecef_t
 */
typedef double ecef_t[3];

static inline void ecefCp(ecef_t out, const ecef_t in){
    out[0]=in[0];
    out[1]=in[1];
    out[2]=in[2];
}

#define tU8           uint8_t             /**< 8-bit Unsigned Integer Type Macro                    */
#define tS8           int8_t              /**< 8-bit Signed Integer Type Macro                      */
#define tU16          uint16_t            /**< 16-bit Unsigned Integer Type Macro                   */
#define tS16          int16_t             /**< 16-bit Signed Integer Type Macro                     */
#define tU32          uint32_t            /**< 32-bit Unsigned Integer Type Macro                   */
#define tS32          int32_t             /**< 32-bit Signed Integer Type Macro                     */
#define tU64          uint64_t            /**< 32-bit Unsigned Integer Type Macro                   */
#define tS64          int64_t            /**< 32-bit Unsigned Integer Type Macro                   */

#endif	// TYPE_H

