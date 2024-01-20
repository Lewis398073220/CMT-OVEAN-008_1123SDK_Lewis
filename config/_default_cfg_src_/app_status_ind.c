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
#include "cmsis_os.h"
#include "stdbool.h"
#include "hal_trace.h"
#include "app_pwl.h"
#include "app_status_ind.h"
#include "string.h"
/* Add by lewis */
#include "app_user.h"
#include "apps.h"
/* End Add by lewis */

static APP_STATUS_INDICATION_T app_status = APP_STATUS_INDICATION_NUM;
static APP_STATUS_INDICATION_T app_status_ind_filter = APP_STATUS_INDICATION_NUM;

static const char * const app_status_indication_str[] =
{
    "[POWERON]",
    "[INITIAL]",
    "[PAGESCAN]",
    "[POWEROFF]",
    "[CHARGENEED]",
    "[CHARGING]",
    "[FULLCHARGE]",
    /* repeatable status: */
    "[BOTHSCAN]",
    "[CONNECTING]",
    "[CONNECTED]",
    "[DISCONNECTED]",
    "[CALLNUMBER]",
    "[INCOMINGCALL]",
    "[PAIRSUCCEED]",
    "[PAIRFAIL]",
    "[HANGUPCALL]",
    "[REFUSECALL]",
    "[ANSWERCALL]",
    "[CLEARSUCCEED]",
    "[CLEARFAIL]",
    "[WARNING]",
    "[ALEXA_START]",
    "[ALEXA_STOP]",
    "[GSOUND_MIC_OPEN]",
    "[GSOUND_MIC_CLOSE]",
    "[GSOUND_NC]",
    "[INVALID]",
    "[MUTE]",
    "[TESTMODE]",
    "[TESTMODE1]",
    "[RING_WARNING]",
#ifdef __INTERACTION__	
    "[FINDME]",
#endif	
    "[MY_BUDS_FIND]",
    "[TILE_FIND]",
/* Add by lewis */
#ifdef CMT_008_NTC_DETECT
	"[NTC_ERROR]",
#endif
	"[AUDIO_LINEIN]",
/* End Add by lewis */
};


const char *status2str(uint16_t status)
{
    const char *str = NULL;

    if (status >= 0 && status < APP_STATUS_INDICATION_NUM)
    {
        str = app_status_indication_str[status];
    }
    else
    {
        str = "[UNKNOWN]";
    }

    return str;
}

int app_status_indication_filter_set(APP_STATUS_INDICATION_T status)
{
    app_status_ind_filter = status;
    return 0;
}

APP_STATUS_INDICATION_T app_status_indication_get(void)
{
    return app_status;
}

int app_status_indication_set(APP_STATUS_INDICATION_T status)
{
    struct APP_PWL_CFG_T cfg0;
    struct APP_PWL_CFG_T cfg1;
/* Add by lewis */
#ifdef CMT_008_EN_LED_BREATH
	APP_BREATH_CFG_T breath_cfg0;
	APP_BREATH_CFG_T breath_cfg1;
#endif
/* End Add by lewis */

    TRACE(2,"%s %d",__func__, status);

	/* Add by lewis */
	if(app_is_power_off_in_progress())
	{
		switch(status)
		{
			case APP_STATUS_INDICATION_POWEROFF:
			break;
			
			default:
				TRACE(0,"%s now is in shutdown process, ignore LED: %d", __func__, status);
			return false;
		}
	}
	/* End Add by lewis */
	
    if (app_status == status)
        return 0;

    if (app_status_ind_filter == status)
        return 0;

    app_status = status;
    memset(&cfg0, 0, sizeof(struct APP_PWL_CFG_T)); //white led
    memset(&cfg1, 0, sizeof(struct APP_PWL_CFG_T)); //red led
    app_pwl_stop(APP_PWL_ID_0);
    app_pwl_stop(APP_PWL_ID_1);
/* Add by lewis */
#ifdef CMT_008_EN_LED_BREATH
	memset(&breath_cfg0, 0, sizeof(APP_BREATH_CFG_T));
	memset(&breath_cfg1, 0, sizeof(APP_BREATH_CFG_T));
	app_breath_led_stop(APP_BREATH_ID_0);
	app_breath_led_stop(APP_BREATH_ID_1);
#endif
/* End Add by lewis */

    switch (status) {
        case APP_STATUS_INDICATION_POWERON:
            cfg0.part[0].level = 1;
            cfg0.part[0].time = (2000); 
            cfg0.part[1].level = 0;
            cfg0.part[1].time = (300); 
            cfg0.parttotal = 2;
            cfg0.startlevel = 1;
            cfg0.periodic = false;

            app_pwl_setup(APP_PWL_ID_0, &cfg0);
            app_pwl_start(APP_PWL_ID_0);

            break;
        case APP_STATUS_INDICATION_INITIAL:
            break;
        case APP_STATUS_INDICATION_PAGESCAN:
            cfg0.part[0].level = 0;
			cfg0.part[0].time = (2000);
			cfg0.part[1].level = 1;
			cfg0.part[1].time = (300);
			cfg0.parttotal = 2;
			cfg0.startlevel = 0;
			cfg0.periodic = true;
            app_pwl_setup(APP_PWL_ID_0, &cfg0);
            app_pwl_start(APP_PWL_ID_0);
            break;
        case APP_STATUS_INDICATION_BOTHSCAN:
#ifdef CMT_008_EN_LED_BREATH	
			breath_cfg0.high_time = 1500;
			breath_cfg0.low_time = 1500;
			breath_cfg0.on_state_time = 500;
			breath_cfg0.off_state_time = 500;

			app_breath_led_setup(APP_BREATH_ID_0, &breath_cfg0);
			app_breath_led_start(APP_BREATH_ID_0);
#endif			
            /*cfg0.part[0].level = 0;
            cfg0.part[0].time = (300);
            cfg0.part[1].level = 1;
            cfg0.part[1].time = (300);
            cfg0.parttotal = 2;
            cfg0.startlevel = 0;
            cfg0.periodic = true;

            cfg1.part[0].level = 1;
            cfg1.part[0].time = (300);
            cfg1.part[1].level = 0;
            cfg1.part[1].time = (300);
            cfg1.parttotal = 2;
            cfg1.startlevel = 1;
            cfg1.periodic = true;

            app_pwl_setup(APP_PWL_ID_0, &cfg0);
            app_pwl_start(APP_PWL_ID_0);
            app_pwl_setup(APP_PWL_ID_1, &cfg1);
            app_pwl_start(APP_PWL_ID_1);*/
            break;
        case APP_STATUS_INDICATION_CONNECTING:
            cfg0.part[0].level = 1;
            cfg0.part[0].time = (300);
            cfg0.part[1].level = 0;
            cfg0.part[1].time = (300);
            cfg0.parttotal = 2;
            cfg0.startlevel = 0;
            cfg0.periodic = true;

            cfg1.part[0].level = 1;
            cfg1.part[0].time = (300);
            cfg1.part[1].level = 0;
            cfg1.part[1].time = (300);
            cfg1.parttotal = 2;
            cfg1.startlevel = 1;
            cfg1.periodic = true;

            app_pwl_setup(APP_PWL_ID_0, &cfg0);
            app_pwl_start(APP_PWL_ID_0);
            app_pwl_setup(APP_PWL_ID_1, &cfg1);
            app_pwl_start(APP_PWL_ID_1);
            break;
        case APP_STATUS_INDICATION_CONNECTED:
            cfg0.part[0].level = 1;
            cfg0.part[0].time = (2000);
            cfg0.part[1].level = 0;
            cfg0.part[1].time = (300);
            cfg0.parttotal = 2;
            cfg0.startlevel = 1;
            cfg0.periodic = false;
            app_pwl_setup(APP_PWL_ID_0, &cfg0);
            app_pwl_start(APP_PWL_ID_0);
            break;
		case APP_STATUS_INDICATION_DISCONNECTED:
			cfg0.part[0].level = 1;
			cfg0.part[0].time = (300);
			cfg0.part[1].level = 0;
			cfg0.part[1].time = (5000);
			cfg0.parttotal = 2;
			cfg0.startlevel = 1;
			cfg0.periodic = true;
			
			app_pwl_setup(APP_PWL_ID_0, &cfg0);
			app_pwl_start(APP_PWL_ID_0);
			break;
        case APP_STATUS_INDICATION_CHARGING:
#ifdef CMT_008_EN_LED_BREATH	
			breath_cfg1.high_time = 1500;
			breath_cfg1.low_time = 1500;
			breath_cfg1.on_state_time = 500;
			breath_cfg1.off_state_time = 500;

			app_breath_led_setup(APP_BREATH_ID_1, &breath_cfg1);
			app_breath_led_start(APP_BREATH_ID_1);
#endif
            /*cfg1.part[0].level = 1;
            cfg1.part[0].time = (5000);
            cfg1.parttotal = 1;
            cfg1.startlevel = 1;
            cfg1.periodic = false;
            app_pwl_setup(APP_PWL_ID_1, &cfg1);
            app_pwl_start(APP_PWL_ID_1);*/
            break;
        case APP_STATUS_INDICATION_FULLCHARGE:
            break;
        case APP_STATUS_INDICATION_POWEROFF:
            cfg1.part[0].level = 1;
            cfg1.part[0].time = (1000);
            cfg1.part[1].level = 0;
            cfg1.part[1].time = (300);            
            cfg1.parttotal = 2;
            cfg1.startlevel = 1;
            cfg1.periodic = false;

            app_pwl_setup(APP_PWL_ID_1, &cfg1);
            app_pwl_start(APP_PWL_ID_1);
            break;
        case APP_STATUS_INDICATION_CHARGENEED:
            cfg1.part[0].level = 1;
            cfg1.part[0].time = (300);
            cfg1.part[1].level = 0;
            cfg1.part[1].time = (5000);
            cfg1.parttotal = 2;
            cfg1.startlevel = 1;
            cfg1.periodic = true;
            app_pwl_setup(APP_PWL_ID_1, &cfg1);
            app_pwl_start(APP_PWL_ID_1);
            break;
        case APP_STATUS_INDICATION_TESTMODE:
            cfg0.part[0].level = 0;
            cfg0.part[0].time = (300);
            cfg0.part[1].level = 1;
            cfg0.part[1].time = (300);
            cfg0.parttotal = 2;
            cfg0.startlevel = 0;
            cfg0.periodic = true;

            cfg1.part[0].level = 0;
            cfg1.part[0].time = (300);
            cfg1.part[1].level = 1;
            cfg1.part[1].time = (300);
            cfg1.parttotal = 2;
            cfg1.startlevel = 1;
            cfg1.periodic = true;

            app_pwl_setup(APP_PWL_ID_0, &cfg0);
            app_pwl_start(APP_PWL_ID_0);
            app_pwl_setup(APP_PWL_ID_1, &cfg1);
            app_pwl_start(APP_PWL_ID_1);
            break;
        case APP_STATUS_INDICATION_TESTMODE1:
            cfg0.part[0].level = 0;
            cfg0.part[0].time = (1000);
            cfg0.part[1].level = 1;
            cfg0.part[1].time = (1000);
            cfg0.parttotal = 2;
            cfg0.startlevel = 0;
            cfg0.periodic = true;

            cfg1.part[0].level = 0;
            cfg1.part[0].time = (1000);
            cfg1.part[1].level = 1;
            cfg1.part[1].time = (1000);
            cfg1.parttotal = 2;
            cfg1.startlevel = 1;
            cfg1.periodic = true;

            app_pwl_setup(APP_PWL_ID_0, &cfg0);
            app_pwl_start(APP_PWL_ID_0);
            app_pwl_setup(APP_PWL_ID_1, &cfg1);
            app_pwl_start(APP_PWL_ID_1);
            break;

/* Add by lewis */
#ifdef CMT_008_NTC_DETECT
		case APP_STATUS_INDICATION_NTC_ERROR:
            cfg1.part[0].level = 1;
            cfg1.part[0].time = (200);
            cfg1.part[1].level = 0;
            cfg1.part[1].time = (200);
            cfg1.part[2].level = 1;
            cfg1.part[2].time = (200);
            cfg1.part[3].level = 0;
            cfg1.part[3].time = (200);
            cfg1.part[4].level = 1;
            cfg1.part[4].time = (200);
            cfg1.part[5].level = 0;
            cfg1.part[5].time = (200);
			cfg1.part[6].level = 0;
            cfg1.part[6].time = (1000);
            cfg1.parttotal = 7;
            cfg1.startlevel = 1;
            cfg1.periodic = true;

            app_pwl_setup(APP_PWL_ID_1, &cfg1);
            app_pwl_start(APP_PWL_ID_1);
            break;	
#endif
		case APP_STATUS_INDICATION_AUDIO_LINEIN:
			cfg0.part[0].level = 1;
			cfg0.part[0].time = (300);
			cfg0.part[1].level = 0;
			cfg0.part[1].time = (300);
			cfg0.part[2].level = 1;
			cfg0.part[2].time = (300);
			cfg0.part[3].level = 0;
			cfg0.part[3].time = (300);
			cfg0.part[4].level = 0;
			cfg0.part[4].time = (5000);
			cfg0.parttotal = 5;
            cfg0.startlevel = 1;
            cfg0.periodic = true;
			
			app_pwl_setup(APP_PWL_ID_0, &cfg0);
			app_pwl_start(APP_PWL_ID_0);
			break;
/* End Add by lewis */

        default:
            break;
    }
    return 0;
}
