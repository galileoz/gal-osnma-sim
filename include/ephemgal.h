
/**
 * @file  ephemgal.h
 * @author: FDC
 * @brief Galileo epemeris structure 
 * Created on 27 juin 2018, 17:05
 */

#ifndef EPHEMGAL_H
#define EPHEMGAL_H
#include "constants.h"
#include "timeType.h"
#include <stdint.h>

/*! \brief Structure representing ephemeris of a single Galileo satellite */
typedef struct ephemgal_t {
    int svId;
    int vflg; /*!< Valid Flag */
    galileotime_t toc; /*!< Time of Clock */
    galileotime_t toe; /*!< Time of Ephemeris */
    int iodnav; /*!< Isuse of Data of the nav batch */
    double deltan; /*!< Delta-N (radians/sec) */
    double cuc; /*!< Cuc (radians) */
    double cus; /*!< Cus (radians) */
    double cic; /*!< Correction to inclination cos (radians) */
    double cis; /*!< Correction to inclination sin (radians) */
    double crc; /*!< Correction to radius cos (meters) */
    double crs; /*!< Correction to radius sin (meters) */
    double ecc; /*!< e Eccentricity */
    double sqrta; /*!< sqrt(A) (sqrt(m)) */
    double m0; /*!< Mean anamoly (radians) */
    double omg0; /*!< Longitude of the ascending node (radians) */
    double inc0; /*!< Inclination (radians) */
    double aop; /*!< Omega (radians) */
    double omgdot; /*!< Omega dot (radians/s) */
    double idot; /*!< IDOT (radians/s) */
    double af0; /*!< Clock offset (seconds) */
    double af1; /*!< rate (sec/sec) */
    double af2; /*!< acceleration (sec/sec^2) */
    double bgde5a;
    double bgde5b;

    // Working variables follow
    double n; /*!< Mean motion (Average angular velocity) */
    double sq1e2; /*!< sqrt(1-e^2) */
    double A; /*!< Semi-major axis */
    double omgkdot; /*!< OmegaDot-OmegaEdot */
    datetime_t t;
} ephemgal_t;



void printEph(ephemgal_t * eph);

/*! \brief update Ephemeris list
 *  \param[in] eph list of Ephemeris data of the satellite
 *  \param[in] g Galileo time at which position is to be computed
 *  @note eph should be a tab of MAX_SAT ephemgal_t 
 *  @todo => update one eph
 */
void UpdateEphemeris(ephemgal_t eph[MAX_SAT], galileotime_t g);





void satpos(ephemgal_t eph, galileotime_t g, double *pos, double *vel, double *clk);

#endif /* EPHEMGAL_H */
