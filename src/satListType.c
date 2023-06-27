/* 
 * File:   channel.c
 * @author: FDC
 * 
 * Created on 6 avril 2020, 14:55
 */
/*****************************************************************************
   Include
 *****************************************************************************/
#include <conversion.h>
#include "galsignal.h"
#include "updateThread.h"
#include "readTestvectors.h"
#include "gal-sdr-sim.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
/*****************************************************************************
   Typedefs and define
 *****************************************************************************/


/*****************************************************************************
   Function declaration
 *****************************************************************************/
void iPageUpdate(channel_t* chan);

int  satVisible( ephemgal_t eph, const receiver_t* receiver, int print);
double computeRange(ephemgal_t eph, const receiver_t* receiver);
channel_t* creatChannel(int sv, satList_t* sat);

/*****************************************************************************
   Function prototypes
 *****************************************************************************/
void testvect_set_halfpage(halfpage_t halfpage_type,hPage_t even_odd, tU8 * navm_testvect)
{
	int i, istop;
	int j = 0;
	int index = 0;

	/* Select navm data according to halpage type */
	if ( halfpage_type == EVEN)
	{
		i = 0;
		istop = SIZEOF_NAV_MSG_TU8/2;
	}
	else
	{
		i = SIZEOF_NAV_MSG_TU8/2;
		istop = SIZEOF_NAV_MSG_TU8;
	}

	/* Set navm data in even/odd convert char: one bit eq to one int*/
	for (; i < istop; i++)
	{
		for (j = 7; j >= 0; j--)
		{
			even_odd[index] = BIT_ISSET(navm_testvect[i], j);
			index++;
		}
	}
}

static inline void removeSat(satList_t* sat, int nsat) {
    free(sat->list[nsat]); // remove channel
    for (int i = nsat; i < MAX_CHAN - 1; i++) { //move list element left
        sat->list[i] = sat->list[i + 1];
    }
    sat->list[MAX_CHAN - 1] = NULL;
}


void newPage(channel_t* chan){
    while(chan->Nframe==NULL){
        printf("nFrame= NULL\n"); 
        waitUpDateFrameThread();
    }
    free(chan->Cframe);
    chan->Cframe = chan->Nframe;
    chan->Nframe = NULL;
    unlockUpDateFrameThread();
    
}


void updateSatList(satList_t* sat, satData_t satData, receiver_t* receiver, int print) {
    ephemgal_t* eph = satData.eph;

    int nsat = 0; // Number of visible satellites;   



    if (receiver->log != NULL) {
        fprintf(receiver->log, "tow:%f\n", getTime(*receiver).tow);
    }

    for (int sv = 0; sv < MAX_SAT; sv++) {
        if (satVisible(eph[sv], receiver, print)) {
            while (sat->list[nsat] != NULL && sat->list[nsat]->svId < sv) { // list[nsat] not visible and should be remove
                removeSat(sat, nsat);
            }

            if (sat->list[nsat] == NULL) { // end of the list
                sat->list[nsat] = creatChannel(sv, sat);

            } else if (sat->list[nsat]->svId > sv) { /// add channel between nsat and nsat -1
                free(sat->list[MAX_CHAN - 1]); // remove last element in the list
                for (int i = MAX_CHAN - 1; i > nsat; i--) { // move right
                    sat->list[i] = sat->list[i - 1];
                }

                sat->list[nsat] = creatChannel(sv, sat);

            }// else: sat already in list : nothing to do 

            nsat++;
        }
    }
    if ( print)
    {
    	printf("\n");
    }
    sat->updateCmpt = sat->updatesatlist_time*nsat; ///next update when updateCmpt=0
    sat->n = nsat;
    return;

}


channel_t* creatChannel(int sv, satList_t* sat){
    channel_t* chan=NULL;
    chan = calloc(sizeof (channel_t),1); // Initialize channel to 0. may create segfault if not initialized
    chan->svId = sv;
    chan->Cframe = NULL;
    chan->Nframe = NULL;
    chan->eph = NULL; // eph=NULL <=> chan not initialize
    chan->newFrame=&sat->newFrame;
    chan->frameUpdated = &sat->frameUpdated;
    chan->listLock = &sat->listLock;
    
    return chan;
}


void initChan(channel_t* chan, struct satData_t* satData, const receiver_t* receiver){
    chan->eph = &satData->eph[chan->svId];
    
    //double ref[3] = {0.0};
    receiver_t ref={0};
    double r_ref, r_xyz;
    double phase_init;

    
    r_ref = computeRange(*chan->eph, &ref);
    
    r_xyz =computeRange(*chan->eph, receiver);

    
    phase_init = (2 * r_ref - r_xyz) / LAMBDA_L1;
    phase_init -= floor(phase_init);
    chan->carr_phase =  phase_init;
    
    ////////////////////////////////////////////////////////////////////////
    // init code
    ////////////////////////////////////////////////////////////////////////
    
    short code[GAL_SEQ_LEN];
    // E1B signal component
    galcodegen_E1B(code, chan->svId);
    boc(chan->boccode_E1B, code, GAL_SEQ_LEN, 1, 1);
    
    // E1C signal component
    galcodegen_E1C(code, chan->svId);
    boc(chan->boccode_E1C, code, GAL_SEQ_LEN, 1, 1);
    
    ////////////////////////////////////////////////////////////////////////
    // init frame
    ////////////////////////////////////////////////////////////////////////
    
    chan->r=r_xyz;
    chan->g=getTime(*receiver);
     
    chan->Nframe = genNavData(satData, chan);
    chan->Cframe = genNavData(satData, chan);
    iPageUpdate(chan);
	// exit(0);
}


/** 
 * @brief update channel
 * compute satellite position and update signal phase and Doppler shift
 * @param[in,out] chan  channel_t struc 
 * @param[in] receiver
 */
void updateChan(channel_t* chan, receiver_t* receiver){
    float deltaT = getTime(*receiver).tow-chan->g.tow;
    if (deltaT>DELTA_T/100){
            // update range
        pthread_mutex_lock(&receiver->lock);
            double range = computeRange(*chan->eph, receiver);
       pthread_mutex_unlock(&receiver->lock);
            // Pseudorange rate
    
    double rhorate = (range - chan->r) / deltaT;
    
    // Carrier and code frequency
    chan->fDoppler = -rhorate / LAMBDA_L1;
    chan->f_code =  CODE_FREQ +  chan->fDoppler*CARR_TO_CODE;//*CARR_TO_CODE;
    chan->r = range;
    chan->g = getTime(*receiver);
    iPageUpdate(chan);
#if TEST_NAN
    if (chan->fDoppler==0){
        printf(" fDoppler = %f",chan->fDoppler);
    }
    if (chan->f_code!=chan->f_code){
        printf("error NAN chan.f_code in updateFrec() %f\n", chan->f_code);
        exit(5);
    }
#endif
    }
}



/**
 * @brief update page and phase index
 * 
 * use range and record in channel_t 
 * 
 * @param[in,out] chan  channel_t struc
 */
void iPageUpdate(channel_t* chan){
    double ms = ((chan->g.tow + SUBF_TRANS_TIME) - chan->r / SPEED_OF_LIGHT)*1000.0;
    
    int ipage = time2ipage(ms); // 1 word = 250 symbols = 1000ms = 1s
    ms -= ipage * PAGE_TRANS_TIME_ms;
    
    int ibit =(unsigned int) ms / BIT_TRANS_TIME; // 1 symbol = 1 code = 4ms
    ms -= ibit * BIT_TRANS_TIME;
    double code_phase = ms / BIT_TRANS_TIME * GAL_SEQ_LEN;
    
	// printf("ibit 1 %d\n",ibit);
	ibit = (ibit + (N_SYM_PAGE/2) ) % N_SYM_PAGE;
	// printf("ibit 2 %d\n",ibit);
    
	if(chan->ipage!=ipage%N_PAGE){
        //newPage(chan);
#if PRINT_INIT_PAGE
        printf("init chan %i: ibit %i, ipage %i\n", chan->svId+1, ibit, ipage);
#endif
    }
    
#if TEST_PHASE_UPDATE
    if (fabs(chan->code_phase - (ms / BIT_TRANS_TIME*GAL_SEQ_LEN))>=0.00003){    
        printf("%i :%f\n",chan->svId+1,chan->fDoppler);  
    }
#endif
    
    chan->code_phase = code_phase;
    chan->ipage = ipage%N_PAGE;  
    chan->ibit = ibit;
	// printf("init ibit %d\n", ibit);
#if TEST_NAN
    if(chan->code_phase!=chan->code_phase){
        printf("error NAN code phase en ipageInit() %f", chan->code_phase);
        exit(5);
    }
#endif
            //sat.chan[i].Cframe = sat.chan[i].page[sat.chan[i].ipage];
            //sat.chan[i].Nframe = sat.chan[i].page[sat.chan[i].ipage+1];
}



/**
 * @brief check if sat is visible.
 * sat is visible if elevation > SAT_MASK (defined in constants.h) 
 * @param eph ephemgal_t struc contain satellite data
 * @param receiver receiver_t struc contain position and time
 * @return true if sat is visible
 */

int satVisible( ephemgal_t eph, const receiver_t* receiver, int print){
    double tmat[3][3]; //Intermediate matrix for ECEF to NEU conversion
    double azel[2]; //Azimuth and elevation
    double llh[3];
    int useSat=0;
    //ecef_t position = receiver->currentPosition;
    xyz2llh(receiver->currentPosition, llh); /// @todo enabled change position
    ltcmat(llh, tmat);
    if (eph.vflg == 1) {
        double los[3];
        double neu[3]; //North-East Up
        double pos[3], vel[3], clk[2]; //Position, velocity and clock
        galileotime_t galtime;
        
        galtime = getTime(*receiver);
        satpos(eph, galtime, pos, vel, clk);
        subVect(los, pos, receiver->currentPosition);
        ecef2neu(los, tmat, neu);
        neu2azel(azel, neu);
#if TEST_SAT>1
		if ( print)
		{
			if ( azel[1] > SAT_MASK)
			{
				printf("\nsat: %02d az: %6.1f el: %5.1f wn:%d tow:%f", eph.svId+1, azel[0] * R2D, azel[1] * R2D,galtime.wn, galtime.tow);
	//        	printf("GSV:%02d;%.0f;%.0f\n", eph.svId+301, azel[1] * R2D, azel[0] * R2D);
			}
		}
#if TEST_SAT>1 && LOGTEST
        if (receiver->log!=NULL){
        fprintf(receiver->log,"sat: %02d az: %6.1f el: %5.1f\n", eph.svId, azel[0] * R2D, azel[1] * R2D);
        }
#endif
#endif  
        useSat=azel[1] > SAT_MASK;
        
    }
    return useSat;
}


void initSatList(satData_t* satData, receiver_t* receiver, satList_t* sat){
    sat->n = 0;
	
	/* The timing for updating eph struct differs in test vector */
	sat->updatesatlist_time = UPDATE_SATLIST_TIME;
	
    pthread_mutex_init(&sat->listLock, NULL);
    pthread_cond_init(&sat->newFrame, NULL);
    pthread_cond_init(&sat->frameUpdated, NULL);
    for (int i = 0; i < MAX_CHAN; i++) {
        sat->list[i] = NULL;
    }
    pthread_mutex_lock(&sat->listLock);
    
    updateSatList(sat, (*satData), receiver, 1);
    
    // Initialize carrier phase
    for (int i = 0; i < sat->n; i++) {
        initChan(sat->list[i], satData, receiver);
    }
    pthread_cond_signal(&sat->newFrame); // Initialize Nframe
    pthread_mutex_unlock(&sat->listLock);
}



int* genNavData(satData_t* satData, channel_t* sat){
    
	// static int i_subframe = 0;
	// static int ipage_prec = -1;
	unsigned long tow=(unsigned long)sat->g.tow+2;
	
	
    double ms = ((sat->g.tow + SUBF_TRANS_TIME) - sat->r / SPEED_OF_LIGHT)*1000.0;
    int ipage;

    int * page = calloc(N_SYM_PAGE,sizeof(int));
    hPage_t even = INIT_HPAGE;
	hPage_t odd = INIT_HPAGE;
    
	ipage = (time2ipage(ms)+1)%N_FPAGE_SBF;
	
	static tU8 navm_testvect[MAX_SAT][SIZEOF_NAV_MSG_TU8] = {0};
	
	// DEBUG
	if ( verbose)
	{
		printf("sat: %i ipage: %i tow: %lu\n",sat->svId+1, ipage%N_FPAGE_SBF, tow);
	}

	testvect_update_navm( navm_testvect, satData->testvectFile, tow);

	testvect_set_halfpage(EVEN, even, navm_testvect[sat->svId]);
	testvect_set_halfpage(ODD, odd, navm_testvect[sat->svId]);

    genTrame(even,&page[0]);
	genTrame(odd, &page[250]);
    return page;
     
}


/*! \brief Compute range between a satellite and the receiver
 *  \return The computed range
 *  \param[in] eph Ephemeris data of the satellite
 *  \param[in] receiver struct (contains position and time)
 */
double computeRange(const ephemgal_t eph, const receiver_t* receiver) {
    double satPos[3], satVel[3], satClk[2];
    double los[3];
    double tau;
    double range;
    galileotime_t galTime = getTime(*receiver);
    //double* xyz = receiver->userMotion[0];  /// \TODO use receiver.position
    //double xrot, yrot;
    
    // SV position at time of the pseudorange observation.
    satpos(eph, galTime, satPos, satVel, satClk);

    // Receiver to satellite vector and light-time.
    subVect(los, satPos, receiver->currentPosition);
    tau = normVect(los) / SPEED_OF_LIGHT;

    // Extrapolate the satellite position backwards to the transmission time.
    satPos[0] -= satVel[0] * tau;
    satPos[1] -= satVel[1] * tau;
    satPos[2] -= satVel[2] * tau;

    // Earth rotation correction. The change in velocity can be neglected.
    satPos[0] = satPos[0] + satPos[1] * OMEGA_EARTH*tau;
    satPos[1] = satPos[1] - satPos[0] * OMEGA_EARTH*tau;

    // New observer to satellite vector and satellite range.
    subVect(los, satPos, receiver->currentPosition);
    range = normVect(los);

    // Pseudorange.
    //rho->range = range - SPEED_OF_LIGHT * satClk[0];

    // Relative velocity of SV and receiver.
//    rate = dotProd(satVel, los) / range;


    // Time of application
    //rho->g = galTime;
#if TEST_NAN
    if(range!=range){
        printf("error NAN in computrange range = %f", range);
        exit(5);
    }
    if (satClk[0]!=satClk[0]){
        printf("error NAN in computrange clk[0] = %f", satClk[0]);
        exit(5);
    }
#endif

    return range - SPEED_OF_LIGHT * satClk[0];
}
