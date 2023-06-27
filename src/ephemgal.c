
/**
 * @file:   ephemgal.c
 * @author: FDC
 * @brief @copybrief ephemgal.h
 * 
 * Created on 27 juin 2020, 17:05
 */

#include <conversion.h>
#include "ephemgal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void printEph(ephemgal_t * eph)
{
	int i;

	printf("Ephemeris:\n");

	for (i = 0; i < MAX_SAT; i++)
	{
		if ( eph[i].vflg == 1)
		{
			printf("\tSat[%d]: A\t%e\n", eph[i].svId, eph[i].A);
			printf("\tSat[%d]: af0\t%e\n", eph[i].svId, eph[i].af0);
			printf("\tSat[%d]: af1\t%e\n", eph[i].svId, eph[i].af1);
			printf("\tSat[%d]: af2\t%e\n", eph[i].svId, eph[i].af2);
			printf("\tSat[%d]: aop\t%e\n", eph[i].svId, eph[i].aop);
			printf("\tSat[%d]: bgde5a\t%e\n", eph[i].svId, eph[i].bgde5a);
			printf("\tSat[%d]: bgde5b\t%e\n", eph[i].svId, eph[i].bgde5b);
			printf("\tSat[%d]: cic\t%e\n", eph[i].svId, eph[i].cic);
			printf("\tSat[%d]: cis\t%e\n", eph[i].svId, eph[i].cis);
			printf("\tSat[%d]: crc\t%e\n", eph[i].svId, eph[i].crc);
			printf("\tSat[%d]: crs\t%e\n", eph[i].svId, eph[i].crs);
			printf("\tSat[%d]: cuc\t%e\n ", eph[i].svId, eph[i].cuc);
			printf("\tSat[%d]: cus\t%e\n", eph[i].svId, eph[i].cus);
			printf("\tSat[%d]: deltan\t%e\n", eph[i].svId, eph[i].deltan);
			printf("\tSat[%d]: ecc\t%e\n", eph[i].svId, eph[i].ecc);
			printf("\tSat[%d]: idot\t%e\n", eph[i].svId, eph[i].idot);
			printf("\tSat[%d]: inc0\t%e\n", eph[i].svId, eph[i].inc0);
			printf("\tSat[%d]: iodnav\t%d\n", eph[i].svId, eph[i].iodnav);
			printf("\tSat[%d]: m0\t%e\n", eph[i].svId, eph[i].m0);
			printf("\tSat[%d]: n\t%e\n", eph[i].svId, eph[i].n);
			printf("\tSat[%d]: omg0\t%e\n", eph[i].svId, eph[i].omg0);
			printf("\tSat[%d]: omgdot\t%e\n", eph[i].svId, eph[i].omgdot);
			printf("\tSat[%d]: omgkdot\t%e\n", eph[i].svId, eph[i].omgkdot);
			printf("\tSat[%d]: sq1e2\t%e\n", eph[i].svId, eph[i].sq1e2);
			printf("\tSat[%d]: sqrta\t%e\n", eph[i].svId, eph[i].sqrta);
			printf("\tSat[%d]: toc\t%0.f\n", eph[i].svId, eph[i].toc.tow);
			printf("\tSat[%d]: toe\t%0.f\n", eph[i].svId, eph[i].toe.tow);
		}

	}
}
void UpdateEphemeris(ephemgal_t eph[MAX_SAT], galileotime_t g) {

    double tk;
    double mk;
    double ek;
    double ekold;
    double cek, sek;
    double pk;
    double c2pk, s2pk;

    double ik;

    double OneMinusecosE;
    int i, tktmp;

    /* For further use
    double relativistic;
	
    double ekdot;
    double uk;
    double pkdot;
    double cuk,suk;
    double rk;
    double ukdot;
    double ok;
    double rkdot;
    double xpkdot,ypkdot;
    double sik,cik;
    double sok,cok;
    double ikdot;
    double xpk,ypk;
     */

    for (i = 0; i < MAX_SAT; i++) {
#if PRINT_EPH
        printf(" update eph: %i  %i\n",eph[i].svId, eph[i].vflg);
#endif
        if (eph[i].vflg) {

            //Set iodnav
            eph[i].iodnav = g.tow / UPDATE_EPH_TIME;

            tk = ((g.wn * SECONDS_IN_HALF_WEEK) + g.tow) - ((eph[i].toe.wn * SECONDS_IN_HALF_WEEK) + eph[i].toe.tow);
            tktmp = tk;
			// printf("updEph Sat[%d]: \n",i+1);
			// printf("\tg\twn %d tow %.f\n", g.wn, g.tow);
			// printf("\ttow\twn %d tow %.f\n", eph[i].toe.wn, eph[i].toe.tow);
			// printf("\tTK\t%.0f\n", tk);
            //Update wn
            while (1) {
                if (tktmp > SECONDS_IN_HALF_WEEK) {
                    tktmp -= SECONDS_IN_WEEK;
                    eph[i].toe.wn++;
                } else if (tktmp<-SECONDS_IN_HALF_WEEK) {
                    eph[i].toe.wn--;
                    tktmp += SECONDS_IN_WEEK;
                } else
                    break;
            }

            mk = eph[i].m0 + eph[i].n*tk;

            //Update m0
            while (mk > 2 * PI)
                mk -= 2 * PI;

            eph[i].m0 = mk;

            ek = mk;
            ekold = ek + 1.0;

            //Calculation to update i0
            while (fabs(ek - ekold) > 1.0E-14) {
                /// @warning  satpos() bug
                ekold = ek;
                OneMinusecosE = 1.0 - eph[i].ecc * cos(ekold);
                ek = ek + (mk - ekold + eph[i].ecc * sin(ekold)) / OneMinusecosE;
            }

            sek = sin(ek);
            cek = cos(ek);

            /* For further use
            ekdot = eph[i].n/OneMinusecosE;
			
            relativistic = -4.442807633E-10*eph[i].ecc*eph[i].sqrta*sek;
             */

            pk = atan2(eph[i].sq1e2*sek, cek - eph[i].ecc) + eph[i].aop;
            /*
            pkdot = eph[i].sq1e2*ekdot/OneMinusecosE; 
             */

            s2pk = sin(2.0 * pk);
            c2pk = cos(2.0 * pk);

            /* For further use
            uk = pk + eph[i].cus*s2pk + eph[i].cuc*c2pk;
			
            suk = sin(uk);
            cuk = cos(uk);
            ukdot = pkdot*(1.0 + 2.0*(eph[i].cus*c2pk - eph[i].cuc*s2pk));

            rk = eph[i].A*OneMinusecosE + eph[i].crc*c2pk + eph[i].crs*s2pk;

            rkdot = eph[i].A*eph[i].ecc*sek*ekdot + 2.0*pkdot*(eph[i].crs*c2pk - eph[i].crc*s2pk);
             */

            ik = eph[i].inc0 + eph[i].idot * tk + eph[i].cic * c2pk + eph[i].cis*s2pk;

            //Update i0
            eph[i].inc0 = ik;

            //Update TOC & TOE
            eph[i].toc = g;
            eph[i].toe = g;

            /* For further use
            sik = sin(ik);
            cik = cos(ik);
            ikdot = eph[i].idot + 2.0*pkdot*(eph[i].cis*c2pk - eph[i].cic*s2pk);
			
            xpk = rk*cuk;
            ypk = rk*suk;

            xpkdot = rkdot*cuk - ypk*ukdot;
            ypkdot = rkdot*suk + xpk*ukdot;
			
            ok = eph[i].omg0 + tk*eph[i].omgkdot - OMEGA_EARTH*eph[i].toe.tow;
			
            sok = sin(ok);
            cok = cos(ok);
             */
        }
    }
    return;
}




/*! \brief Compute Satellite position, velocity and clock at given time
 *  \param[in] eph Ephemeris data of the satellite
 *  \param[in] g Galileo time at which position is to be computed
 *  \param[out] pos Computed position (vector)
 *  \param[out] vel Computed velociy (vector)
 *  \param[out] clk Computed clock
 */
void satpos(ephemgal_t eph, galileotime_t g, double *pos, double *vel, double *clk) {
    // Computing Satellite Velocity using the Broadcast Ephemeris
    // http://www.ngs.noaa.gov/gps-toolbox/bc_velo.htm

    double tk;
    double mk;
    double ek;
    double ekold;
    double ekdot;
    double cek, sek;
    double pk;
    double pkdot;
    double c2pk, s2pk;
    double uk;
    double ukdot;
    double cuk, suk;
    double ok;
    double sok, cok;
    double ik;
    double ikdot;
    double sik, cik;
    double rk;
    double rkdot;
    double xpk, ypk;
    double xpkdot, ypkdot;

    double relativistic, OneMinusecosE=0, tmp;
    
    tk = g.tow - eph.toe.tow;
	
	// printf("satpos Sat[%d]: \n",eph.svId+1);
	// printf("\tg\twn %d tow %.f\n", g.wn, g.tow);
	// printf("\ttoe\twn %d tow %.f\n",eph.toe.wn, eph.toe.tow);
	// printf("\tTK\t%.0f\n", tk);
	
    if (tk > SECONDS_IN_HALF_WEEK)
        tk -= SECONDS_IN_WEEK;
    else if (tk<-SECONDS_IN_HALF_WEEK)
        tk += SECONDS_IN_WEEK;

    mk = eph.m0 + eph.n*tk;
    ek = mk;
    ekold = ek + 1.0;
    int cpt=0;
    while ((fabs(ek - ekold) > 1.0E-14) && cpt<MAX_ITERATION) {

        cpt++;
        ekold = ek;
        OneMinusecosE = 1.0 - eph.ecc * cos(ekold);
        ek = ek + (mk - ekold + eph.ecc * sin(ekold)) / OneMinusecosE;
        
#if TEST_SATPOSCONV
     /** corr bugg bug block in while
     *  sat pos do not converge at 10.9s for:
     *  RUN_RESOURCE = -e $EPH2
     *  POSITION = -l 44,4
     *  DATA = -t $DATE4
     * 
     */
        if (cpt>MAX_ITERATION-TEST_SATPOSCONV){
            printf("satPos cmpt= %i , ek= %.15f, ekol= %.15f ,ecc= %f mk = %f delta = %.15f\n", cpt, ek, ekold, eph.ecc, mk, fabs(ek - ekold));
        }
#endif
        
    }

    sek = sin(ek);
    cek = cos(ek);

    ekdot = eph.n / OneMinusecosE;

    relativistic = -4.442807633E-10 * eph.ecc * eph.sqrta*sek;

    pk = atan2(eph.sq1e2*sek, cek - eph.ecc) + eph.aop;
    pkdot = eph.sq1e2 * ekdot / OneMinusecosE;

    s2pk = sin(2.0 * pk);
    c2pk = cos(2.0 * pk);

    uk = pk + eph.cus * s2pk + eph.cuc*c2pk;
    suk = sin(uk);
    cuk = cos(uk);
    ukdot = pkdot * (1.0 + 2.0 * (eph.cus * c2pk - eph.cuc * s2pk));

    rk = eph.A * OneMinusecosE + eph.crc * c2pk + eph.crs*s2pk;
    rkdot = eph.A * eph.ecc * sek * ekdot + 2.0 * pkdot * (eph.crs * c2pk - eph.crc * s2pk);

    ik = eph.inc0 + eph.idot * tk + eph.cic * c2pk + eph.cis*s2pk;

    sik = sin(ik);
    cik = cos(ik);
    ikdot = eph.idot + 2.0 * pkdot * (eph.cis * c2pk - eph.cic * s2pk);

    xpk = rk*cuk;
    ypk = rk*suk;
    xpkdot = rkdot * cuk - ypk*ukdot;
    ypkdot = rkdot * suk + xpk*ukdot;

    ok = eph.omg0 + tk * eph.omgkdot - OMEGA_EARTH * eph.toe.tow;
    sok = sin(ok);
    cok = cos(ok);

    //Compute position
    pos[0] = xpk * cok - ypk * cik*sok;
    pos[1] = xpk * sok + ypk * cik*cok;
    pos[2] = ypk*sik;

    tmp = ypkdot * cik - ypk * sik*ikdot;

    //Compute velocity
    vel[0] = -eph.omgkdot * pos[1] + xpkdot * cok - tmp*sok;
    vel[1] = eph.omgkdot * pos[0] + xpkdot * sok + tmp*cok;
    vel[2] = ypk * cik * ikdot + ypkdot*sik;

    //Satellite clock correction
    tk = g.tow - eph.toc.tow;

    if (tk > SECONDS_IN_HALF_WEEK)
        tk -= SECONDS_IN_WEEK;
    else if (tk<-SECONDS_IN_HALF_WEEK)
        tk += SECONDS_IN_WEEK;

    clk[0] = eph.af0 + tk * (eph.af1 + tk * eph.af2) + relativistic - eph.bgde5b;

    clk[1] = eph.af1 + 2.0 * tk * eph.af2;
#if TEST_NAN
    if (clk[0]!=clk[0]||clk[1]!=clk[1]){
        printf("error NAN clk in Satpos() %f, %f",clk[0],clk[1]);
        exit(5);
    }
#endif
    return;
}
