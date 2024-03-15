/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __APP_STATUS_IND_H__
#define __APP_STATUS_IND_H__

#ifdef RTOS
#include "cmsis_os.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum APP_STATUS_INDICATION_T {
    APP_STATUS_INDICATION_POWERON = 0,
    APP_STATUS_INDICATION_INITIAL,
    APP_STATUS_INDICATION_PAGESCAN,
    APP_STATUS_INDICATION_POWEROFF,
    APP_STATUS_INDICATION_CHARGENEED,
    APP_STATUS_INDICATION_CHARGING,
    APP_STATUS_INDICATION_FULLCHARGE,
    APP_STATUS_INDICATION_NO_REPEAT_NUM,
    /* repeatable status: */
    APP_STATUS_INDICATION_BOTHSCAN = APP_STATUS_INDICATION_NO_REPEAT_NUM,
    APP_STATUS_INDICATION_CONNECTING,
    APP_STATUS_INDICATION_CONNECTED,
    APP_STATUS_INDICATION_DISCONNECTED,
    APP_STATUS_INDICATION_CALLNUMBER,
    APP_STATUS_INDICATION_INCOMINGCALL,
    APP_STATUS_INDICATION_PAIRSUCCEED,
    APP_STATUS_INDICATION_PAIRFAIL,
    APP_STATUS_INDICATION_HANGUPCALL,
    APP_STATUS_INDICATION_REFUSECALL,
    APP_STATUS_INDICATION_ANSWERCALL,
    APP_STATUS_INDICATION_CLEARSUCCEED,
    APP_STATUS_INDICATION_CLEARFAIL,
    APP_STATUS_INDICATION_WARNING,
    APP_STATUS_INDICATION_ALEXA_START,
    APP_STATUS_INDICATION_ALEXA_STOP,
    APP_STATUS_INDICATION_GSOUND_MIC_OPEN,
    APP_STATUS_INDICATION_GSOUND_MIC_CLOSE,
    APP_STATUS_INDICATION_GSOUND_NC,
    APP_STATUS_INDICATION_INVALID,
    APP_STATUS_INDICATION_MUTE,
    APP_STATUS_INDICATION_TESTMODE,
    APP_STATUS_INDICATION_TESTMODE1,
    APP_STATUS_RING_WARNING,
#ifdef __INTERACTION__	
    APP_STATUS_INDICATION_FINDME,
#endif	
    APP_STATUS_INDICATION_FIND_MY_BUDS,
    APP_STATUS_INDICATION_TILE_FIND,
/* Add by lewis */
#ifdef CMT_008_NTC_DETECT
	APP_STATUS_INDICATION_NTC_ERROR,
#endif

	APP_STATUS_INDICATION_FACTORY_RESET,

	APP_STATUS_INDICATION_AUDIO_LINEIN,
/* End Add by lewis */

    APP_STATUS_INDICATION_NUM
}APP_STATUS_INDICATION_T;

/* Add by lewis */
//typedef struct {
//	APP_STATUS_INDICATION_T status_ind_next;
//	uint8_t repeat_cnt; //At the beginning, it was for the connection status indication to be displayed repeatedly
//} APP_STATUS_INDICATION_CFG_T;

/* End Add by lewis */

const char *status2str(uint16_t status);
int app_status_indication_filter_set(APP_STATUS_INDICATION_T status);
APP_STATUS_INDICATION_T app_status_indication_get(void);
int app_status_indication_set(APP_STATUS_INDICATION_T status);
/* Add by lewis */
int app_status_indication_delay_set(APP_STATUS_INDICATION_T status, uint32_t delay_ms);
/* End Add by lewis */


#ifdef __cplusplus
}
#endif

#endif

