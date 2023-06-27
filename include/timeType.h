/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   timeType.h
 * @author: FDC
 *
 * Created on 6 avril 2018, 18:15
 */

#ifndef TIMETYPE_H
#define TIMETYPE_H


/*! \brief Structure representing Galileo time */
typedef struct {
    int wn; /*!< Galileo week number (since 21/22 August 1999) */
    double tow; /*!< time of week: second inside the Galileo week */
} galileotime_t;
/**
 *  @brief initialyse galileotime_t structure
 *  @note tow & wn are set to -1 
 */
#define INIT_GALILEOTIME_T (galileotime_t) {-1,-1}
/*! \brief Structure repreenting UTC time
 * @todo used time.h std lib */
typedef struct {
    int y; /*!< Calendar year */
    int m; /*!< Calendar month */
    int d; /*!< Calendar day */
    int hh; /*!< Calendar hour */
    int mm; /*!< Calendar minutes */
    double sec; /*!< Calendar seconds */
} datetime_t;

typedef struct
{
	int enable;
	int vflg;
	double AI0,AI1,AI2,zero;
	double A0,A1;
	int dtls;
} ionoutc_t;


#endif /* TIMETYPE_H */
