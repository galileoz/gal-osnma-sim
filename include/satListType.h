
/* 
 * channel list strucutre definition.
 * @section usage
 * @subsection mainTread
 * 
 * 
 * @file:   satListType.h
 * @author: FDC
 *
 * Created on 6 avril 2018, 14:55
 */

#ifndef CHANNEL_H
#define CHANNEL_H
#include "receiver.h"
#include <pthread.h>
#include "timeType.h"
#include "satDataType.h"
#include "galWord.h"

/*! \brief Structure representing a Channel.
 *  one channel is creat for each satellite in wiex
 */
typedef struct channel_s {
    int svId;                           ///< satelit identifier [0;NSAT-1] @note svId = PRN-1
    short boccode_E1B[2 * GAL_SEQ_LEN]; ///< @brief E1B Subcarrier. sequence for boc(1,1) of E1B Primary code 
    short boccode_E1C[2 * GAL_SEQ_LEN]; ///< @brief E1C Subcarrier. sequence for boc(1,1) of E1C primary code
    double fDoppler;                    ///< Doppler Frequency of carrier
    double f_code;                      ///< sub-carrier frequency 
    double carr_phase;                  ///< carrier phase (rad/2PI)
    double code_phase;                  ///< sub-carrier phase (rad/2PI)
    //int page[N_PAGE][N_SYM_PAGE];       ///< Data page of sub-frame 
    int *Cframe;                        /*!< @brief curent page*/
    int *Nframe;                        /*!< @brief next page*/
    int ipage;                          ///< initial page
    int ibit;                           ///< initial bit 
    ephemgal_t* eph;                    ///< pointer to ephemirs 
    galileotime_t g;
    double r;
    pthread_cond_t* newFrame;           ///< point to satList.newFrame
    pthread_cond_t* frameUpdated;       ///< point to satList.frameUpdated
    pthread_mutex_t* listLock;          ///< point to satList.listLock
}channel_t;



/** \brief list of channel.
 * 
 * @ingroup main_structure
 * @see satListType.h
 * 
 *  only the n first  of list are  used.
 *  channel are store for increasing SVID order.
 *  unsed channel of list are set to NULL 
 * 
 *   */
typedef struct satList_t{
    channel_t* list[MAX_CHAN];  /**< @brief list of channel. 
                                 * tab of pointer of channel_t. only the n first element is used.
                                 * channel are store for increasing SVID
                                 *  unsed element are set to NULL */
    int n;                      /**< \brief numbre of channel used*/
    int updateCmpt;              /**< \brief update compter. satList while be update when compteur egale to 0 */
	int updatesatlist_time;
    pthread_mutex_t listLock;   /**< \brief mutex */
    pthread_cond_t newFrame;    /**< \brief set when channel require a new frame*/
    pthread_cond_t frameUpdated;/**< @brief send when new frame is creat*/
}satList_t;


#define INIT_CHANLIST_T {{NULL},0, 0,0, PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER}


/**
 * @brief start transmiting next page
 * 
 * use next page for transmission and call update thread for creat a new page
 * @see upDateFrame()
 * @param chan
 */
void newPage(channel_t* chan);
/**
 * @brief init for stalist_t structure
 * @param satData
 * @param receiver
 * @param sat
 */
void initSatList(satData_t* satData, receiver_t* receiver, satList_t* sat);

/**
 * @brief update range and speed between satellite and receiver
 * @param[in,out] chan channel_t to update
 * @param[in] receiver position & time  
 */
void updateChan( channel_t* chan, receiver_t* receiver);

/**
 *  @brief update list of visible sat
 * 
 * @see SAT_MASK
 * @param sat
 * @param satData
 * @param receiver
 */
void updateSatList(satList_t* sat, satData_t satData, receiver_t* receiver, int print); 


/**@brief generation navigation data word for one channel
 *  
 * generat Navigation Data for  \p sat in time \p tow
 * @param[in] satData   satellite date
 * @param[in] sat channel_t of satellite
 * @return navigation data symbol
 */
int* genNavData(satData_t* satData, channel_t* sat);

void initChan(channel_t* chan,  satData_t* satData, const receiver_t* receiver);

#endif /* CHANNEL_H */
