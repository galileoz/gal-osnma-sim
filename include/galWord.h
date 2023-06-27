
/** 
 * @brief galileo I/NAV word definition
 * @file   galWord.h
 * @author: FDC
 * 
 * @see genNavData()
 *
 * Created on 29 mars 2018, 11:14
 *  
 */

#ifndef GALWORD_H
#define GALWORD_H
#include "ephemgal.h"

/*! \brief Number of 1/2 page per subframe for Galileo*/
#define N_HPAGE_SBF 30 // 30 half pages per subframe

/**@brief  numbre of page per sub frame */
#define N_FPAGE_SBF 15

/*! \brief Number of bit per half page */
#define N_BIT_PAGE 120
/*! \brief number of symbole per half page
 */
#define N_SYM_PAGE 500
/**@brief numbre of bit in I/NAV word */
#define N_BIT_WORD 128
/*! \brief Number of half page in a frame 
 */
#define N_PAGE  360
/**  numbre of page per sub frame 
 */
#define N_PAGE_SBF 15
/** @brief page transmission time (s)
 */
#define PAGE_TRANS_TIME 2
/*! \brief Galileo Page transmission time (ms)
 */
#define PAGE_TRANS_TIME_ms 2000


/*! \brief Galileo PrimarycCode sequence length */
#define GAL_SEQ_LEN 4092

/*! \brief Galileo SecondarycCode sequence length */
#define GAL_SEC_CODE_LEN 25
/*! \brief Galileo symbol transmission time (ms) */
#define BIT_TRANS_TIME 4
/*! \brief Subframe transmission time @todo remove*/
#define SUBF_TRANS_TIME 0 

typedef int word_t[N_BIT_WORD];      ///< \brief Galileo I/NAV word (128 bits)
typedef int hPage_t[N_BIT_PAGE];        ///< \brief half page (120 Bits)
typedef hPage_t subf_t[N_HPAGE_SBF];    ///< \brief subframe (30 half pages)
typedef subf_t frame_t[N_SBF];          ///< \brief frame (24 subframes)
typedef int page_t[N_SYM_PAGE];         ///< @brief page (500 symbol). symbol of 2 half pages

#define INIT_HPAGE {0}                  ///< \brief initialyse hPage_t to 0
#define INIT_SUBF {{0}}                 ///< \brief initialyse subf_t to 0
#define INIT_FRAME {{{0}}}              ///< \brief initialyse frame_t to 0

/**
 * @brief part of galileo I/NAV page
 */
typedef enum halfPage{
 EVEN = 0, ///< first part of page
 ODD= 1    ///< second part of page
}halfpage_t;

typedef enum wordType_t{
                    W0=0,
                    W1=1,
                    W2=2,
                    W3=3,
                    W4=4,
                    W5=5,
                    W6=6,
                    W7=7,
                    W8=8,
                    W9=9,
                    W10=10,
                    Alm1=-1, //almanach word 7 or 9 change for eache surb frame
                    Alm2=-2  //almanach word 8 or 10 change for eache surb frame
}wordType_t;

    static inline unsigned int time2ipage(double TxTime){
        return (unsigned int) TxTime / PAGE_TRANS_TIME_ms;
    }
void genTrame(hPage_t page, int* symbol);

#endif /* GALWORD_H */
