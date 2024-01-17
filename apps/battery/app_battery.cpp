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
#include "tgt_hardware.h"
#include "pmu.h"
#include "hal_timer.h"
#include "hal_gpadc.h"
#include "hal_trace.h"
#include "hal_gpio.h"
#include "hal_iomux.h"
#include "hal_chipid.h"
#include "app_thread.h"
#include "app_battery.h"
#include "audio_policy.h"
#include "app_bt.h"
#include "apps.h"
#include "app_hfp.h"
#include "audio_player_adapter.h"

#include "app_status_ind.h"
#include "bluetooth_bt_api.h"
#include "app_media_player.h"
/* Add by lewis */
#ifdef CMT_008_BLE_ENABLE
#include "tota_ble_custom.h"
#endif
#include "app_user.h"
#if (defined(BT_USB_AUDIO_DUAL_MODE) || defined(BTUSB_AUDIO_MODE))
#include "hal_usb.h"
#endif
/* End add by lewis */

#ifdef BT_USB_AUDIO_DUAL_MODE
#include "btusb_audio.h"
#endif
#include <stdlib.h>

#ifdef __INTERCONNECTION__
#include "bluetooth_ble_api.h"
#endif

#if (defined(BTUSB_AUDIO_MODE) || defined(BTUSB_AUDIO_MODE))
extern "C" bool app_usbaudio_mode_on(void);
#endif

#define APP_BATTERY_TRACE(s,...)
// TRACE(s, ##__VA_ARGS__)

#ifndef APP_BATTERY_MIN_MV
#define APP_BATTERY_MIN_MV (3450) //Modifed by Jay, changed from 3200 to 3450.
#endif

#ifndef APP_BATTERY_MAX_MV
#define APP_BATTERY_MAX_MV (4200)
#endif

#ifndef APP_BATTERY_PD_MV
#define APP_BATTERY_PD_MV   (3400) //Modifed by Jay, changed from 3100 to 3400. 
#endif

#ifndef APP_BATTERY_CHARGE_TIMEOUT_MIN
#define APP_BATTERY_CHARGE_TIMEOUT_MIN (120) //Modifed by lewis, changed from 90 to 120.
#endif

#ifndef APP_BATTERY_CHARGE_OFFSET_MV
#define APP_BATTERY_CHARGE_OFFSET_MV (50) //Modifed by Jay, changed from 20 to 50.
#endif

#ifndef CHARGER_PLUGINOUT_RESET
#define CHARGER_PLUGINOUT_RESET (1)
#endif

#ifndef CHARGER_PLUGINOUT_DEBOUNCE_MS
#define CHARGER_PLUGINOUT_DEBOUNCE_MS (50)
#endif

#ifndef CHARGER_PLUGINOUT_DEBOUNCE_CNT
#define CHARGER_PLUGINOUT_DEBOUNCE_CNT (3)
#endif

#define APP_BATTERY_CHARGING_PLUGOUT_DEDOUNCE_CNT (APP_BATTERY_CHARGING_PERIODIC_MS<500?3:1)

#define APP_BATTERY_CHARGING_EXTPIN_MEASURE_CNT (APP_BATTERY_CHARGING_PERIODIC_MS<2*1000?2*1000/APP_BATTERY_CHARGING_PERIODIC_MS:1)
#define APP_BATTERY_CHARGING_EXTPIN_DEDOUNCE_CNT (6)

#define APP_BATTERY_CHARGING_OVERVOLT_MEASURE_CNT (APP_BATTERY_CHARGING_PERIODIC_MS<2*1000?2*1000/APP_BATTERY_CHARGING_PERIODIC_MS:1)
#define APP_BATTERY_CHARGING_OVERVOLT_DEDOUNCE_CNT (3)

#define APP_BATTERY_CHARGING_SLOPE_MEASURE_CNT (APP_BATTERY_CHARGING_PERIODIC_MS<20*1000?20*1000/APP_BATTERY_CHARGING_PERIODIC_MS:1)
#define APP_BATTERY_CHARGING_SLOPE_TABLE_COUNT (6)


#define APP_BATTERY_REPORT_INTERVAL (5)

#define APP_BATTERY_MV_BASE ((APP_BATTERY_MAX_MV-APP_BATTERY_PD_MV)/(APP_BATTERY_LEVEL_NUM))

#define APP_BATTERY_STABLE_COUNT (5)
#define APP_BATTERY_MEASURE_PERIODIC_FAST_MS (200)
#ifdef BLE_ONLY_ENABLED
#define APP_BATTERY_MEASURE_PERIODIC_NORMAL_MS (25000)
#else
#define APP_BATTERY_MEASURE_PERIODIC_NORMAL_MS (10000)
#endif
#define APP_BATTERY_CHARGING_PERIODIC_MS (APP_BATTERY_MEASURE_PERIODIC_NORMAL_MS)

#define APP_BATTERY_SET_MESSAGE(appevt, status, volt) (appevt = (((uint32_t)status&0xffff)<<16)|(volt&0xffff))
#define APP_BATTERY_GET_STATUS(appevt, status) (status = (appevt>>16)&0xffff)
#define APP_BATTERY_GET_VOLT(appevt, volt) (volt = appevt&0xffff)
#define APP_BATTERY_GET_PRAMS(appevt, prams) ((prams) = appevt&0xffff)

enum APP_BATTERY_MEASURE_PERIODIC_T
{
    APP_BATTERY_MEASURE_PERIODIC_FAST = 0,
    APP_BATTERY_MEASURE_PERIODIC_NORMAL,
    APP_BATTERY_MEASURE_PERIODIC_CHARGING,

    APP_BATTERY_MEASURE_PERIODIC_QTY,
};

struct APP_BATTERY_MEASURE_CHARGER_STATUS_T
{
    HAL_GPADC_MV_T prevolt;
    int32_t slope_1000[APP_BATTERY_CHARGING_SLOPE_TABLE_COUNT];
    int slope_1000_index;
    int cnt;
};


typedef void (*APP_BATTERY_EVENT_CB_T)(enum APP_BATTERY_STATUS_T, APP_BATTERY_MV_T volt);

struct APP_BATTERY_MEASURE_T
{
    uint32_t start_time;
    enum APP_BATTERY_STATUS_T status;
#ifdef __INTERCONNECTION__
    uint8_t currentBatteryInfo;
    uint8_t lastBatteryInfo;
    uint8_t isMobileSupportSelfDefinedCommand;
#else
	uint8_t prelevel; //Add by lewis
    uint8_t currlevel;
#endif
	/* Add by lewis */
	uint16_t isBatteryLow:1;
	uint16_t BatteryLowCnt:15;
	APP_BATTERY_MV_T prevolt;
	/* End Add by lewis */
    APP_BATTERY_MV_T currvolt;
    APP_BATTERY_MV_T lowvolt;
    APP_BATTERY_MV_T highvolt;
    APP_BATTERY_MV_T pdvolt;
    uint32_t chargetimeout;
    enum APP_BATTERY_MEASURE_PERIODIC_T periodic;
    HAL_GPADC_MV_T voltage[APP_BATTERY_STABLE_COUNT];
    uint16_t index;
    struct APP_BATTERY_MEASURE_CHARGER_STATUS_T charger_status;
    APP_BATTERY_EVENT_CB_T cb;
    APP_BATTERY_CB_T user_cb;
};

#ifdef IS_BES_BATTERY_MANAGER_ENABLED

/* Add by jay */
static APP_BATTERY_MV_T batterylevel_table[] = 
{4030, 3920, 3820, 3740, 3670, 3620, 3570, 3510, 3440};
/* End Add by jay */

static enum APP_BATTERY_CHARGER_T app_battery_charger_forcegetstatus(void);

static void app_battery_pluginout_debounce_start(void);
static void app_battery_pluginout_debounce_handler(void const *param);
osTimerDef (APP_BATTERY_PLUGINOUT_DEBOUNCE, app_battery_pluginout_debounce_handler);
static osTimerId app_battery_pluginout_debounce_timer = NULL;
static uint32_t app_battery_pluginout_debounce_ctx = 0;
static uint32_t app_battery_pluginout_debounce_cnt = 0;

static void app_battery_timer_handler(void const *param);
osTimerDef (APP_BATTERY, app_battery_timer_handler);
static osTimerId app_battery_timer = NULL;
static struct APP_BATTERY_MEASURE_T app_battery_measure;

static int app_battery_charger_handle_process(void);

#ifdef __INTERCONNECTION__
uint8_t* app_battery_get_mobile_support_self_defined_command_p(void)
{
    return &app_battery_measure.isMobileSupportSelfDefinedCommand;
}
#endif


void app_battery_irqhandler(uint16_t irq_val, HAL_GPADC_MV_T volt)
{
    uint8_t i;
    uint32_t meanBattVolt = 0;
    HAL_GPADC_MV_T vbat = volt;
    APP_BATTERY_TRACE(2,"%s %d",__func__, vbat);
    if (vbat == HAL_GPADC_BAD_VALUE)
    {
        app_battery_measure.cb(APP_BATTERY_STATUS_INVALID, vbat);
        return;
    }

#if (defined(BTUSB_AUDIO_MODE) || defined(BTUSB_AUDIO_MODE))
    if(app_usbaudio_mode_on()) return ;
#endif
    app_battery_measure.voltage[app_battery_measure.index++%APP_BATTERY_STABLE_COUNT] = vbat<<2;

    if (app_battery_measure.index > APP_BATTERY_STABLE_COUNT)
    {
        for (i=0; i<APP_BATTERY_STABLE_COUNT; i++)
        {
            meanBattVolt += app_battery_measure.voltage[i];
        }
        meanBattVolt /= APP_BATTERY_STABLE_COUNT;
        if (app_battery_measure.cb)
        {
            TRACE(3, "highvolt[%d], lowvolt[%d], pdvolt[%d],", app_battery_measure.highvolt, app_battery_measure.lowvolt, app_battery_measure.pdvolt);
            if (meanBattVolt>app_battery_measure.highvolt) //more than 4200mV.
            {
                TRACE(2, "%s   OVER   BattVolt[%d]", __func__, meanBattVolt);
                app_battery_measure.cb(APP_BATTERY_STATUS_OVERVOLT, meanBattVolt);
            }
            /* BattVolt > 3400mV && BattVolt < 3450mV, now is low battery state. */
            else if((meanBattVolt>app_battery_measure.pdvolt) && (meanBattVolt<app_battery_measure.lowvolt))
            {
                TRACE(2, "%s   UNDER   BattVolt[%d]", __func__, meanBattVolt);
                app_battery_measure.cb(APP_BATTERY_STATUS_UNDERVOLT, meanBattVolt);
            }
            else if(meanBattVolt<=app_battery_measure.pdvolt) //lenss than or equal to 3400mV.
            {
                TRACE(2, "%s   PD   BattVolt[%d]", __func__, meanBattVolt);
                app_battery_measure.cb(APP_BATTERY_STATUS_PDVOLT, meanBattVolt);
            }
            else
            {
                TRACE(2, "%s   NORMAL   BattVolt[%d]", __func__, meanBattVolt);
                app_battery_measure.cb(APP_BATTERY_STATUS_NORMAL, meanBattVolt);
            }
        }
    }
    else
    {
        int8_t level = 0;
        meanBattVolt = vbat<<2;
#if 0 //Modify by lewis
        level = (meanBattVolt-APP_BATTERY_PD_MV)/APP_BATTERY_MV_BASE;
#else
		for(i = 0; i < 9; i++)
        {
            if(meanBattVolt >= batterylevel_table[i])
                break;
        }

		level = 9 - i;
#endif
        if (level<APP_BATTERY_LEVEL_MIN)
            level = APP_BATTERY_LEVEL_MIN;
        if (level>APP_BATTERY_LEVEL_MAX)
            level = APP_BATTERY_LEVEL_MAX;

        app_battery_measure.currvolt = meanBattVolt;
#ifdef __INTERCONNECTION__
        APP_BATTERY_INFO_T* pBatteryInfo = (APP_BATTERY_INFO_T*)&app_battery_measure.currentBatteryInfo;
        pBatteryInfo->batteryLevel = level;
#else
        app_battery_measure.currlevel = level;
#endif
    }
}

static void app_battery_timer_start(enum APP_BATTERY_MEASURE_PERIODIC_T periodic)
{
    uint32_t periodic_millisec = 0;

    if (app_battery_measure.periodic != periodic){
        app_battery_measure.periodic = periodic;
        switch (periodic)
        {
            case APP_BATTERY_MEASURE_PERIODIC_FAST:
                periodic_millisec = APP_BATTERY_MEASURE_PERIODIC_FAST_MS;
                break;
            case APP_BATTERY_MEASURE_PERIODIC_CHARGING:
                periodic_millisec = APP_BATTERY_CHARGING_PERIODIC_MS;
                break;
            case APP_BATTERY_MEASURE_PERIODIC_NORMAL:
                periodic_millisec = APP_BATTERY_MEASURE_PERIODIC_NORMAL_MS;
            default:
                break;
        }
        osTimerStop(app_battery_timer);
        osTimerStart(app_battery_timer, periodic_millisec);
    }
}

static void app_battery_timer_handler(void const *param)
{
#ifdef CHARGER_1802
    charger_vbat_div_adc_enable(true);
    hal_gpadc_open(HAL_GPADC_CHAN_5, HAL_GPADC_ATP_ONESHOT, app_battery_irqhandler);
#else
    hal_gpadc_open(HAL_GPADC_CHAN_BATTERY, HAL_GPADC_ATP_ONESHOT, app_battery_irqhandler);
#endif
}

static void app_battery_event_process(enum APP_BATTERY_STATUS_T status, APP_BATTERY_MV_T volt)
{
    uint32_t app_battevt;
    APP_MESSAGE_BLOCK msg;

    APP_BATTERY_TRACE(3,"%s %d,%d",__func__, status, volt);
    msg.mod_id = APP_MODUAL_BATTERY;
    APP_BATTERY_SET_MESSAGE(app_battevt, status, volt);
    msg.msg_body.message_id = app_battevt;
    msg.msg_body.message_ptr = (uint32_t)NULL;
    app_mailbox_put(&msg);

}

int app_status_battery_report(uint8_t level)
{
#if defined(__BTIF_EARPHONE__)
    app_10_second_timer_check();
#endif

    if (app_is_stack_ready())
    {
// #if (HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_BATTERY_REPORT) || (HF_SDK_FEATURES & HF_FEATURE_HF_INDICATORS)
#if defined(SUPPORT_BATTERY_REPORT) || defined(SUPPORT_HF_INDICATORS)
#if defined(IBRT)
        uint8_t hfp_device = app_bt_audio_get_curr_hfp_device();
        struct BT_DEVICE_T *curr_device = app_bt_get_device(hfp_device);
        if (curr_device->hf_conn_flag)
        {
            app_hfp_set_battery_level(level);
        }
#elif defined(BT_HFP_SUPPORT)
        app_hfp_set_battery_level(level);
#endif
#else
        TRACE(1,"[%s] Can not enable SUPPORT_BATTERY_REPORT", __func__);
#endif
        bes_bt_osapi_notify_evm();
    }
    return 0;
}

int app_battery_handle_process_normal(uint32_t status,  union APP_BATTERY_MSG_PRAMS prams)
{
    int8_t level = 0;
	/* Add by lewis */
	uint8_t i;
	/* End Add by lewis */

    switch (status)
    {
        case APP_BATTERY_STATUS_UNDERVOLT:
            TRACE(1,"UNDERVOLT:%d", prams.volt);
#if 0 //Disable by lewis
            app_status_indication_set(APP_STATUS_INDICATION_CHARGENEED);

#ifdef MEDIA_PLAYER_SUPPORT
#if defined(IBRT)

#else
            audio_player_play_prompt(AUD_ID_BT_CHARGE_PLEASE, 0);
#endif
#endif
#endif
            // FALLTHROUGH
        case APP_BATTERY_STATUS_NORMAL:
        case APP_BATTERY_STATUS_OVERVOLT:
            app_battery_measure.currvolt = prams.volt;
#if 0 //Modify by lewis
            level = (prams.volt-APP_BATTERY_PD_MV)/APP_BATTERY_MV_BASE;
#else
			//when discharge, battery votage should gradually decrease instead of increasing
			if(app_battery_measure.prevolt >= app_battery_measure.currvolt)
			{
				app_battery_measure.prevolt = app_battery_measure.currvolt;
				
			} 
			else{
				app_battery_measure.currvolt = app_battery_measure.prevolt;
			}
			
			if((app_battery_measure.currvolt > APP_BATTERY_PD_MV) && (app_battery_measure.currvolt < APP_BATTERY_MIN_MV))
			{
				//warning battery low every 1min
				app_status_indication_set(APP_STATUS_INDICATION_CHARGENEED);
				
				if(!app_battery_measure.isBatteryLow) {
					app_battery_measure.isBatteryLow = true;
#ifdef MEDIA_PLAYER_SUPPORT
					audio_player_play_prompt(AUD_ID_BT_CHARGE_PLEASE, 0);
#endif
				} else{
					app_battery_measure.BatteryLowCnt++;
					if(app_battery_measure.BatteryLowCnt >= 6)
					{
						app_battery_measure.BatteryLowCnt = 0;
#ifdef MEDIA_PLAYER_SUPPORT
						audio_player_play_prompt(AUD_ID_BT_CHARGE_PLEASE, 0);
#endif					
					}
				}
			}
			
			for(i = 0; i < 9; i++)
			{
				if(app_battery_measure.currvolt >= batterylevel_table[i])
					break;
			}
			level = 9 - i;
#endif

            if (level<APP_BATTERY_LEVEL_MIN)
                level = APP_BATTERY_LEVEL_MIN;
            if (level>APP_BATTERY_LEVEL_MAX)
                level = APP_BATTERY_LEVEL_MAX;
#ifdef __INTERCONNECTION__
            APP_BATTERY_INFO_T* pBatteryInfo;
            pBatteryInfo = (APP_BATTERY_INFO_T*)&app_battery_measure.currentBatteryInfo;
            pBatteryInfo->batteryLevel = level;
            if(level == APP_BATTERY_LEVEL_MAX)
            {
                level = 9;
            }
            else
            {
                level /= 10;
            }
#else
            app_battery_measure.currlevel = level;
#endif
            app_status_battery_report(level);

			/* Add by lewis */
			if(app_battery_measure.prelevel > app_battery_measure.currlevel)
			{
				app_battery_measure.prelevel = app_battery_measure.currlevel;
#ifdef CMT_008_BLE_ENABLE
				battery_level_change_notify(app_battery_current_level() + 1);
#endif
			}
			/* End Add by lewis */
            break;
        case APP_BATTERY_STATUS_PDVOLT:
#if 1 //ndef BT_USB_AUDIO_DUAL_MODE //Mofify by lewis
            TRACE(1,"PDVOLT-->POWEROFF:%d", prams.volt);
#if 0 //Mofify by lewis
            osTimerStop(app_battery_timer);
#else
			app_battery_stop();
#endif
            app_shutdown();
#endif
            break;
        case APP_BATTERY_STATUS_CHARGING:
            TRACE(1,"CHARGING-->APP_BATTERY_CHARGER :%d", prams.charger);
            if (prams.charger == APP_BATTERY_CHARGER_PLUGIN)
            {
#ifdef BT_USB_AUDIO_DUAL_MODE
                TRACE(1,"%s:PLUGIN.", __func__);
                btusb_switch(BTUSB_MODE_USB);
#else
#if CHARGER_PLUGINOUT_RESET
                app_reset();
#else
                app_battery_measure.status = APP_BATTERY_STATUS_CHARGING;
#endif
#endif
            }
            break;
        case APP_BATTERY_STATUS_INVALID:
        default:
            break;
    }

    app_battery_timer_start(APP_BATTERY_MEASURE_PERIODIC_NORMAL);
    return 0;
}

/* Add by lewis **/
#if defined(BT_USB_AUDIO_DUAL_MODE)
osTimerId usb_unplug_sw_timer = NULL;
static void usb_unplug_swtimer_handler(void const *param);
osTimerDef(USB_UNPLUG_TIMER, usb_unplug_swtimer_handler);// define timers
#define USB_UNPLUG_SWTIMER_MS	(1000)

static void usb_unplug_swtimer_handler(void const *param)
{
    if(app_is_power_off_in_progress()){
		osTimerDelete(usb_unplug_sw_timer);
	} else{
		app_shutdown();
		osTimerStart(usb_unplug_sw_timer,USB_UNPLUG_SWTIMER_MS);
	}
}

void app_usb_unplug_swtimer_start(void)
{
	TRACE(0,"%s",__func__);
	if(usb_unplug_sw_timer == NULL)
		usb_unplug_sw_timer = osTimerCreate(osTimer(USB_UNPLUG_TIMER), osTimerOnce, NULL);

	osTimerStop(usb_unplug_sw_timer);
	osTimerStart(usb_unplug_sw_timer,USB_UNPLUG_SWTIMER_MS);
}

void app_usb_unplug_swtimer_stop(void)
{
	TRACE(0,"%s",__func__);

	if(usb_unplug_sw_timer == NULL)
		return;
	
	osTimerStop(usb_unplug_sw_timer);
}
#endif
/* End add by lewis */

int app_battery_handle_process_charging(uint32_t status,  union APP_BATTERY_MSG_PRAMS prams)
{
    switch (status)
    {
        case APP_BATTERY_STATUS_OVERVOLT:
        case APP_BATTERY_STATUS_NORMAL:
        case APP_BATTERY_STATUS_UNDERVOLT:
            app_battery_measure.currvolt = prams.volt;
            app_status_battery_report(prams.volt);
            break;
        case APP_BATTERY_STATUS_CHARGING:
            TRACE(1,"CHARGING:%d", prams.charger);
            if (prams.charger == APP_BATTERY_CHARGER_PLUGOUT)
            {
#if 1 //ndef BT_USB_AUDIO_DUAL_MODE //Mofify by lewis
#if CHARGER_PLUGINOUT_RESET
                TRACE(0,"CHARGING-->RESET");
                osTimerStop(app_battery_timer);
                app_shutdown();
#else
/* Mofify by lewis */
#if 0
                app_battery_measure.status = APP_BATTERY_STATUS_NORMAL;
#else
				TRACE(0,"CHARGING-->SHUTDOWN");
                app_battery_stop();
                app_shutdown();
#if defined(BT_USB_AUDIO_DUAL_MODE)
				app_usb_unplug_swtimer_start();
#endif
/* End Mofify by lewis */

#endif
#endif
#endif
            }
            else if (prams.charger == APP_BATTERY_CHARGER_PLUGIN)
            {
#if defined(BT_USB_AUDIO_DUAL_MODE)
                TRACE(1,"%s:PLUGIN.", __func__);
                btusb_switch(BTUSB_MODE_USB);
#endif
            }
            break;
        case APP_BATTERY_STATUS_INVALID:
        default:
            break;
    }

    if (app_battery_charger_handle_process()<=0)
    {
        if (app_status_indication_get() != APP_STATUS_INDICATION_FULLCHARGE)
        {
            TRACE(1,"FULL_CHARGING:%d", app_battery_measure.currvolt);
            app_status_indication_set(APP_STATUS_INDICATION_FULLCHARGE);
#ifdef MEDIA_PLAYER_SUPPORT
#if defined(BT_USB_AUDIO_DUAL_MODE) || defined(IBRT)
#else
            //media_PlayAudio(AUD_ID_BT_CHARGE_FINISH, 0); //Disable by lewis
#endif
#endif
			/* Add by Jay */
			en_dis_charging(false);
			app_battery_stop();
//#ifdef  CMT_008_NTC_DETECT
			//app_ntc_swtimer_stop();
//#endif
			/* End Add by Jay */
        }
    }

	//TODO: after full charge, recharge when battery voltage is lower than a certain value
	//if (app_status_indication_get() == APP_STATUS_INDICATION_FULLCHARGE)
	
    app_battery_timer_start(APP_BATTERY_MEASURE_PERIODIC_CHARGING);

    return 0;
}

static int app_battery_handle_process(APP_MESSAGE_BODY *msg_body)
{
    uint8_t status;
    union APP_BATTERY_MSG_PRAMS msg_prams;

    APP_BATTERY_GET_STATUS(msg_body->message_id, status);
    APP_BATTERY_GET_PRAMS(msg_body->message_id, msg_prams.prams);

    uint32_t generatedSeed = hal_sys_timer_get();
    for (uint8_t index = 0; index < sizeof(bt_global_addr); index++)
    {
        generatedSeed ^= (((uint32_t)(bt_global_addr[index])) << (hal_sys_timer_get()&0xF));
    }
    srand(generatedSeed);

    if (status == APP_BATTERY_STATUS_PLUGINOUT){
        app_battery_pluginout_debounce_start();
    }
    else
    {
        switch (app_battery_measure.status)
        {
            case APP_BATTERY_STATUS_NORMAL:
                app_battery_handle_process_normal((uint32_t)status, msg_prams);
                break;

            case APP_BATTERY_STATUS_CHARGING:
                app_battery_handle_process_charging((uint32_t)status, msg_prams);
                break;

            default:
                break;
        }
    }
    if (NULL != app_battery_measure.user_cb)
    {
        uint8_t batteryLevel;
#ifdef __INTERCONNECTION__
        APP_BATTERY_INFO_T* pBatteryInfo;
        pBatteryInfo = (APP_BATTERY_INFO_T*)&app_battery_measure.currentBatteryInfo;
        pBatteryInfo->chargingStatus = ((app_battery_measure.status == APP_BATTERY_STATUS_CHARGING)? 1:0);
        batteryLevel = pBatteryInfo->batteryLevel;

#else
        batteryLevel = app_battery_measure.currlevel;
#endif
        app_battery_measure.user_cb(app_battery_measure.currvolt,
                                    batteryLevel, app_battery_measure.status,status,msg_prams);
    }

    return 0;
}

int app_battery_register(APP_BATTERY_CB_T user_cb)
{
    if(NULL == app_battery_measure.user_cb)
    {
        app_battery_measure.user_cb = user_cb;
        return 0;
    }
    return 1;
}

int app_battery_get_info(APP_BATTERY_MV_T *currvolt, uint8_t *currlevel, enum APP_BATTERY_STATUS_T *status)
{
    if (currvolt)
    {
        *currvolt = app_battery_measure.currvolt;
    }

    if (currlevel)
    {
#ifdef __INTERCONNECTION__
        *currlevel = app_battery_measure.currentBatteryInfo;
#else
        *currlevel = app_battery_measure.currlevel;
#endif
    }

    if (status)
    {
        *status = app_battery_measure.status;
    }

    return 0;
}

int app_battery_open(void)
{
    APP_BATTERY_TRACE(3,"%s batt range:%d~%d",__func__, APP_BATTERY_MIN_MV, APP_BATTERY_MAX_MV);
    int nRet = APP_BATTERY_OPEN_MODE_INVALID;

    if (app_battery_timer == NULL)
        app_battery_timer = osTimerCreate (osTimer(APP_BATTERY), osTimerPeriodic, NULL);

    if (app_battery_pluginout_debounce_timer == NULL)
        app_battery_pluginout_debounce_timer = osTimerCreate (osTimer(APP_BATTERY_PLUGINOUT_DEBOUNCE), osTimerOnce, &app_battery_pluginout_debounce_ctx);

    app_battery_measure.status = APP_BATTERY_STATUS_NORMAL;
#ifdef __INTERCONNECTION__
    app_battery_measure.currentBatteryInfo = APP_BATTERY_DEFAULT_INFO;
    app_battery_measure.lastBatteryInfo = APP_BATTERY_DEFAULT_INFO;
    app_battery_measure.isMobileSupportSelfDefinedCommand = 0;
#else
	app_battery_measure.prelevel = APP_BATTERY_LEVEL_MAX; //Add by lewis
    app_battery_measure.currlevel = APP_BATTERY_LEVEL_MAX;
#endif
	/* Add by lewis */
	app_battery_measure.isBatteryLow = false;
	app_battery_measure.BatteryLowCnt = 0;
	app_battery_measure.prevolt = APP_BATTERY_MAX_MV;
	/* End Add by lewis */
    app_battery_measure.currvolt = APP_BATTERY_MAX_MV;
    app_battery_measure.lowvolt = APP_BATTERY_MIN_MV;
    app_battery_measure.highvolt = APP_BATTERY_MAX_MV;
    app_battery_measure.pdvolt = APP_BATTERY_PD_MV;
    app_battery_measure.chargetimeout = APP_BATTERY_CHARGE_TIMEOUT_MIN;

    app_battery_measure.periodic = APP_BATTERY_MEASURE_PERIODIC_QTY;
    app_battery_measure.cb = app_battery_event_process;
    app_battery_measure.user_cb = NULL;

    app_battery_measure.charger_status.prevolt = 0;
    app_battery_measure.charger_status.slope_1000_index = 0;
    app_battery_measure.charger_status.cnt = 0;

    app_set_threadhandle(APP_MODUAL_BATTERY, app_battery_handle_process);

    if (app_battery_ext_charger_detecter_cfg.pin != HAL_IOMUX_PIN_NUM)
    {
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&app_battery_ext_charger_detecter_cfg, 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_battery_ext_charger_detecter_cfg.pin, HAL_GPIO_DIR_IN, 1);
    }

    if (app_battery_ext_charger_enable_cfg.pin != HAL_IOMUX_PIN_NUM)
    {
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&app_battery_ext_charger_enable_cfg, 1); //Modified by Jay
#if 0 //Modify by lewis
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_battery_ext_charger_detecter_cfg.pin, HAL_GPIO_DIR_OUT, 1);
#else
		en_dis_charging(false);
#endif
    }

    if (app_battery_charger_indication_open() == APP_BATTERY_CHARGER_PLUGIN)
    {
        TRACE(2,"[%s]       status [ APP_BATTERY_CHARGER_PLUGIN ]", __func__);
        app_battery_measure.status = APP_BATTERY_STATUS_CHARGING;
        app_battery_measure.start_time = hal_sys_timer_get();
        //pmu_charger_plugin_config();
#if 0 //Modify by lewis
        if (app_battery_ext_charger_enable_cfg.pin != HAL_IOMUX_PIN_NUM)
        {
            hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_battery_ext_charger_detecter_cfg.pin, HAL_GPIO_DIR_OUT, 0);
        }
#else
		en_dis_charging(true);
#endif

#if (CHARGER_PLUGINOUT_RESET == 0)
        nRet = APP_BATTERY_OPEN_MODE_CHARGING_PWRON;
        TRACE(2," [%s] , if [%d]",__func__,nRet);
#else
        nRet = APP_BATTERY_OPEN_MODE_CHARGING;
        TRACE(2," [%s] , else [%d]",__func__,nRet);
#endif
    }
    else
    {
        app_battery_measure.status = APP_BATTERY_STATUS_NORMAL;
        //pmu_charger_plugout_config();
        nRet = APP_BATTERY_OPEN_MODE_NORMAL;
        TRACE(2," [%s] , _else_ [%d]",__func__,nRet);
    }

    return nRet;
}

int app_battery_start(void)
{
    APP_BATTERY_TRACE(2,"%s %d",__func__, APP_BATTERY_MEASURE_PERIODIC_FAST_MS);

    app_battery_timer_start(APP_BATTERY_MEASURE_PERIODIC_FAST);
/* Add by lewis */
#ifdef CMT_008_NTC_DETECT
	app_ntc_detect_start();
#endif
/* End Add by lewis */

    return 0;
}

int app_battery_stop(void)
{
    osTimerStop(app_battery_timer);

/* Add by lewis */
#ifdef CMT_008_NTC_DETECT
	app_ntc_swtimer_stop();
#endif
/* End Add by lewis */

    return 0;
}

int app_battery_close(void)
{
    hal_gpadc_close(HAL_GPADC_CHAN_BATTERY);

    return 0;
}

#if 0 //Disable by lewis
static int32_t app_battery_charger_slope_calc(int32_t t1, int32_t v1, int32_t t2, int32_t v2)
{
    int32_t slope_1000;
    slope_1000 = (v2-v1)*1000/(t2-t1);
    return slope_1000;
}
#endif

static int app_battery_charger_handle_process(void)
{
    int nRet = 1;
#if 0 //Disable by lewis
    int8_t i=0,cnt=0;
    uint32_t slope_1000 = 0;
#endif
    uint32_t charging_min;
    static uint8_t overvolt_full_charge_cnt = 0;
    static uint8_t ext_pin_full_charge_cnt = 0;

    charging_min = hal_sys_timer_get() - app_battery_measure.start_time;
    charging_min = TICKS_TO_MS(charging_min)/1000/60;
    if (charging_min >= app_battery_measure.chargetimeout)
    {
/* Add by lewis for stopping charging when in USBaudio mode*/
#if (defined(BT_USB_AUDIO_DUAL_MODE) || defined(BTUSB_AUDIO_MODE))
		if(hal_usb_configured())
		{
			if(app_battery_measure.currvolt < app_battery_measure.highvolt - 5)
			{	
				app_battery_measure.start_time = hal_sys_timer_get();
				goto exit;
			}
		}
#endif
/* End Add by lewis */

        TRACE(0,"TIMEROUT-->FULL_CHARGING");
        nRet = -1;
        goto exit;
    }

    if ((app_battery_measure.charger_status.cnt++%APP_BATTERY_CHARGING_OVERVOLT_MEASURE_CNT) == 0)
    {
        if (app_battery_measure.currvolt>=(app_battery_measure.highvolt+APP_BATTERY_CHARGE_OFFSET_MV))
        {
            overvolt_full_charge_cnt++;
        }
/* Add by lewis for stopping charging when in USBaudio mode*/
#if (defined(BT_USB_AUDIO_DUAL_MODE) || defined(BTUSB_AUDIO_MODE))
		else if(hal_usb_configured() && (app_battery_measure.currvolt >= app_battery_measure.highvolt - 5))
		{
			overvolt_full_charge_cnt++;
		}
#endif
/* End add by lewis */
        else
        {
            overvolt_full_charge_cnt = 0;
        }
        if (overvolt_full_charge_cnt>=APP_BATTERY_CHARGING_OVERVOLT_DEDOUNCE_CNT)
        {
            TRACE(0,"OVERVOLT-->FULL_CHARGING");
            nRet = -1;
            goto exit;
        }
    }

    if ((app_battery_measure.charger_status.cnt++%APP_BATTERY_CHARGING_EXTPIN_MEASURE_CNT) == 0)
    {
        if (app_battery_ext_charger_detecter_cfg.pin != HAL_IOMUX_PIN_NUM)
        {
            if (!hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)app_battery_ext_charger_detecter_cfg.pin)) //Modified by Jay
            {
                ext_pin_full_charge_cnt++;
            }
            else
            {
                ext_pin_full_charge_cnt = 0;
            }
            if (ext_pin_full_charge_cnt>=APP_BATTERY_CHARGING_EXTPIN_DEDOUNCE_CNT)
            {
                TRACE(0,"EXT PIN-->FULL_CHARGING");
                nRet = -1;
                goto exit;
            }
        }
    }

#if 0 //Disable by lewis
    if ((app_battery_measure.charger_status.cnt++%APP_BATTERY_CHARGING_SLOPE_MEASURE_CNT) == 0)
    {
        if (!app_battery_measure.charger_status.prevolt)
        {
            app_battery_measure.charger_status.slope_1000[app_battery_measure.charger_status.slope_1000_index%APP_BATTERY_CHARGING_SLOPE_TABLE_COUNT] = slope_1000;
            app_battery_measure.charger_status.prevolt = app_battery_measure.currvolt;
            for (i=0; i<APP_BATTERY_CHARGING_SLOPE_TABLE_COUNT; i++)
            {
                app_battery_measure.charger_status.slope_1000[i]=100;
            }
        }
        else
        {
            slope_1000 = app_battery_charger_slope_calc(0, app_battery_measure.charger_status.prevolt,
                         APP_BATTERY_CHARGING_PERIODIC_MS*APP_BATTERY_CHARGING_SLOPE_MEASURE_CNT/1000, app_battery_measure.currvolt);
            app_battery_measure.charger_status.slope_1000[app_battery_measure.charger_status.slope_1000_index%APP_BATTERY_CHARGING_SLOPE_TABLE_COUNT] = slope_1000;
            app_battery_measure.charger_status.prevolt = app_battery_measure.currvolt;
            for (i=0; i<APP_BATTERY_CHARGING_SLOPE_TABLE_COUNT; i++)
            {
                if (app_battery_measure.charger_status.slope_1000[i]>0)
                    cnt++;
                else
                    cnt--;
                TRACE(3,"slope_1000[%d]=%d cnt:%d", i,app_battery_measure.charger_status.slope_1000[i], cnt);
            }
            TRACE(3,"app_battery_charger_slope_proc slope*1000=%d cnt:%d nRet:%d", slope_1000, cnt, nRet);
            if (cnt>1)
            {
                nRet = 1;
            }/*else (3>=cnt && cnt>=-3){
                nRet = 0;
            }*/else
            {
                if (app_battery_measure.currvolt>=(app_battery_measure.highvolt-APP_BATTERY_CHARGE_OFFSET_MV))
                {
                    TRACE(0,"SLOPE-->FULL_CHARGING");
                    nRet = -1;
                }
            }
        }
        app_battery_measure.charger_status.slope_1000_index++;
    }
#endif

exit:
    return nRet;
}

static enum APP_BATTERY_CHARGER_T app_battery_charger_forcegetstatus(void)
{
    enum APP_BATTERY_CHARGER_T status = APP_BATTERY_CHARGER_QTY;
    enum PMU_CHARGER_STATUS_T charger;

    charger = pmu_charger_get_status();

    if (charger == PMU_CHARGER_PLUGIN)
    {
        status = APP_BATTERY_CHARGER_PLUGIN;
        // TRACE(0,"force APP_BATTERY_CHARGER_PLUGIN");
    }
    else
    {
        status = APP_BATTERY_CHARGER_PLUGOUT;
        // TRACE(0,"force APP_BATTERY_CHARGER_PLUGOUT");
    }

    return status;
}

static void app_battery_charger_handler(enum PMU_CHARGER_STATUS_T status)
{
    TRACE(2,"%s: status=%d", __func__, status);
    pmu_charger_set_irq_handler(NULL);
    app_battery_event_process(APP_BATTERY_STATUS_PLUGINOUT,
                              (status == PMU_CHARGER_PLUGIN) ? APP_BATTERY_CHARGER_PLUGIN : APP_BATTERY_CHARGER_PLUGOUT);
}

static void app_battery_pluginout_debounce_start(void)
{
    TRACE(1,"%s", __func__);
#if defined(BT_USB_AUDIO_DUAL_MODE)
    btusb_switch(BTUSB_MODE_BT);
#endif
    app_battery_pluginout_debounce_ctx = (uint32_t)app_battery_charger_forcegetstatus();
    app_battery_pluginout_debounce_cnt = 1;
    osTimerStart(app_battery_pluginout_debounce_timer, CHARGER_PLUGINOUT_DEBOUNCE_MS);
}

static void app_battery_pluginout_debounce_handler(void const *param)
{
    enum APP_BATTERY_CHARGER_T status_charger = app_battery_charger_forcegetstatus();

    if(app_battery_pluginout_debounce_ctx == (uint32_t) status_charger){
        app_battery_pluginout_debounce_cnt++;
    }
    else
    {
        TRACE(2,"%s dithering cnt %u", __func__, app_battery_pluginout_debounce_cnt);
        app_battery_pluginout_debounce_cnt = 0;
        app_battery_pluginout_debounce_ctx = (uint32_t)status_charger;
    }

    if (app_battery_pluginout_debounce_cnt >= CHARGER_PLUGINOUT_DEBOUNCE_CNT){
        TRACE(2,"%s %s", __func__, status_charger == APP_BATTERY_CHARGER_PLUGOUT ? "PLUGOUT" : "PLUGIN");
        if (status_charger == APP_BATTERY_CHARGER_PLUGIN)
        {
#if 0 //Modify by lewis
            if (app_battery_ext_charger_enable_cfg.pin != HAL_IOMUX_PIN_NUM)
            {
                hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_battery_ext_charger_detecter_cfg.pin, HAL_GPIO_DIR_OUT, 0);
            }
#else
			en_dis_charging(true);
#endif
            app_battery_measure.start_time = hal_sys_timer_get();
        }
        else
        {
#if 0 //Modify by lewis
            if (app_battery_ext_charger_enable_cfg.pin != HAL_IOMUX_PIN_NUM)
            {
                hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_battery_ext_charger_detecter_cfg.pin, HAL_GPIO_DIR_OUT, 1);
            }
#else
			en_dis_charging(false);
#endif
        }
        app_battery_event_process(APP_BATTERY_STATUS_CHARGING, status_charger);
        pmu_charger_set_irq_handler(app_battery_charger_handler);
        osTimerStop(app_battery_pluginout_debounce_timer);
    }else{
        osTimerStart(app_battery_pluginout_debounce_timer, CHARGER_PLUGINOUT_DEBOUNCE_MS);
    }
}

int app_battery_charger_indication_open(void)
{
    enum APP_BATTERY_CHARGER_T status = APP_BATTERY_CHARGER_QTY;
    uint8_t cnt = 0;

    APP_BATTERY_TRACE(1,"%s",__func__);

    pmu_charger_init();

    do
    {
        status = app_battery_charger_forcegetstatus();
        if (status == APP_BATTERY_CHARGER_PLUGIN)
            break;
        osDelay(20);
    }
    while(cnt++<5);

#if 0 //Disable by lewis
    if (app_battery_ext_charger_detecter_cfg.pin != HAL_IOMUX_PIN_NUM)
    {
        if (!hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)app_battery_ext_charger_detecter_cfg.pin))
        {
            status = APP_BATTERY_CHARGER_PLUGIN;
        }
    }
#endif

    pmu_charger_set_irq_handler(app_battery_charger_handler);

    return status;
}

int8_t app_battery_current_level(void)
{
#ifdef __INTERCONNECTION__
    return app_battery_measure.currentBatteryInfo & 0x7f;
#else
    return app_battery_measure.currlevel;
#endif
}

int8_t app_battery_is_charging(void)
{
    return (APP_BATTERY_STATUS_CHARGING == app_battery_measure.status);
}

/* Add by lewis */
int8_t app_is_battery_low(void)
{
	return app_battery_measure.isBatteryLow;
}
/* End Add by lewis */

/* Add by Jay */
#ifdef CMT_008_NTC_DETECT

osTimerId ntc_sw_timer = NULL;
static void ntc_swtimer_handler(void const *param);
osTimerDef(NTC_TIMER, ntc_swtimer_handler);// define timers
#define NTC_SWTIMER_MS	(5000)

/* Software timer hanlde func */
static void ntc_swtimer_handler(void const *param)
{
    ntc_capture_start();
}

/* Create a software timer task */
void app_ntc_swtimer_start(void)
{
	TRACE(0,"%s",__func__);
	/* Create a software Timer and return a Timer ID for later indexing. */
	if(ntc_sw_timer == NULL)
		ntc_sw_timer = osTimerCreate(osTimer(NTC_TIMER), osTimerPeriodic, NULL);

	/* The following are the Stop and Start Timer tasks,
     * which take the parameters of the created Timer ID and time. */ 
	osTimerStop(ntc_sw_timer);
	osTimerStart(ntc_sw_timer,NTC_SWTIMER_MS);
}

void app_ntc_swtimer_stop(void)
{
	TRACE(0,"%s",__func__);

	if(ntc_sw_timer == NULL)
		return;
	
	osTimerStop(ntc_sw_timer);
}

void app_ntc_detect_start(void)
{
	ntc_capture_open();
	app_ntc_swtimer_start();
}
#endif
/* End Add by Jay */

typedef uint16_t NTP_VOLTAGE_MV_T;
typedef uint16_t NTP_TEMPERATURE_C_T;

#define NTC_CAPTURE_STABLE_COUNT (5)
#define NTC_CAPTURE_TEMPERATURE_STEP (4)
#define NTC_CAPTURE_TEMPERATURE_REF (15)
#define NTC_CAPTURE_VOLTAGE_REF (1100)
/* Add by lewis */
#ifdef CMT_008_NTC_DETECT
#if 1
/* ntc 15.1k, NTC_REF is 1.7v  */
#define NTC_CHARGE_LOW_VOLTAGE           415//590     //45C 31C
#define NTC_CHARGE_HIGH_VOLTAGE          1101//1320    //0C -14C
#define NTC_RECHARGE_LOW_VOLTAGE         448//530     //42C 35C
#define NTC_RECHARGE_HIGH_VOLTAGE        1048//1270    //3C -10C

#define NTC_DISCHARGE_LOW_VOLTAGE        281//400     //60C 46C
#define NTC_DISCHARGE_HIGH_VOLTAGE       1402//1587    //-20C <-25C
#else
/* ntc 30k ok */
#define CHARGE_HIGH_TEMPERATURE          225     // 45C
#define CHARGE_LOW_TEMPERATURE           949    // 0C
#define CHARGE_HIGH_TEMPERATURE_RECOVER  256    // 41C
#define CHARGE_LOW_TEMPERATURE_RECOVER   840    // 4C

#define DISCHARGE_HIGH_TEMPERATURE       193    // 50C
#define DISCHARGE_LOW_TEMPERATURE        1266   // -10C
#endif

#define THERMAL_PROTECTION_DET_TIMES     5
#endif
/* End Add by lewis */

typedef void (*NTC_CAPTURE_MEASURE_CB_T)(NTP_TEMPERATURE_C_T);

struct NTC_CAPTURE_MEASURE_T
{
    NTP_TEMPERATURE_C_T temperature;
    NTP_VOLTAGE_MV_T currvolt;
    NTP_VOLTAGE_MV_T voltage[NTC_CAPTURE_STABLE_COUNT];
    uint16_t index;
    NTC_CAPTURE_MEASURE_CB_T cb;
	/* Add by lewis */
	uint8_t stop_charge_cnt;
	uint8_t recover_charge_cnt;
	uint8_t pwroff_cnt;
	uint8_t isChargeProtect;
	/* End Add by lewis */
};

static struct NTC_CAPTURE_MEASURE_T ntc_capture_measure;

void ntc_capture_irqhandler(uint16_t irq_val, HAL_GPADC_MV_T volt)
{
    uint32_t meanVolt = 0;
    TRACE(3,"%s %d irq:0x%04x",__func__, volt, irq_val);

    if (volt == HAL_GPADC_BAD_VALUE)
    {
        return;
    }

    ntc_capture_measure.voltage[ntc_capture_measure.index++%NTC_CAPTURE_STABLE_COUNT] = volt;

    if (ntc_capture_measure.index > NTC_CAPTURE_STABLE_COUNT)
    {
        for (uint8_t i=0; i<NTC_CAPTURE_STABLE_COUNT; i++)
        {
            meanVolt += ntc_capture_measure.voltage[i];
        }
        meanVolt /= NTC_CAPTURE_STABLE_COUNT;
        ntc_capture_measure.currvolt = meanVolt;
    }
/* Modify by lewis */
#if 0
    else if (!ntc_capture_measure.currvolt)
#else
	else
#endif
/* End Modify by lewis */
    {
        ntc_capture_measure.currvolt = volt;
    }
    ntc_capture_measure.temperature = ((int32_t)ntc_capture_measure.currvolt - NTC_CAPTURE_VOLTAGE_REF)/NTC_CAPTURE_TEMPERATURE_STEP + NTC_CAPTURE_TEMPERATURE_REF;
    pmu_ntc_capture_disable();
    TRACE(3,"%s ad:%d temperature:%d",__func__, ntc_capture_measure.currvolt, ntc_capture_measure.temperature);

/* Add by lewis */
#ifdef CMT_008_NTC_DETECT
	if(app_battery_is_charging())
	{
		if(ntc_capture_measure.isChargeProtect == false) 
		{
			ntc_capture_measure.recover_charge_cnt = 0;
			if((ntc_capture_measure.currvolt < NTC_CHARGE_LOW_VOLTAGE) || (ntc_capture_measure.currvolt > NTC_CHARGE_HIGH_VOLTAGE)) {
				ntc_capture_measure.stop_charge_cnt++;
			} else{
				ntc_capture_measure.stop_charge_cnt = 0;
			}

			if(ntc_capture_measure.stop_charge_cnt >= THERMAL_PROTECTION_DET_TIMES)
			{
				TRACE(0,"NTC-->STOP_CHARGING");
				ntc_capture_measure.isChargeProtect = true;
				en_dis_charging(false);
				app_status_indication_set(APP_STATUS_INDICATION_NTC_ERROR);
				osTimerStop(app_battery_timer);
			}
		}
		else
		{
			ntc_capture_measure.stop_charge_cnt = 0;
			if((ntc_capture_measure.currvolt >= NTC_RECHARGE_LOW_VOLTAGE) && (ntc_capture_measure.currvolt <= NTC_RECHARGE_HIGH_VOLTAGE)) {
				ntc_capture_measure.recover_charge_cnt++;
			} else{
				ntc_capture_measure.recover_charge_cnt = 0;
			}

			if(ntc_capture_measure.recover_charge_cnt >= THERMAL_PROTECTION_DET_TIMES)
			{
				TRACE(0,"NTC-->RECOVER_CHARGING");
				ntc_capture_measure.isChargeProtect = false;
				en_dis_charging(true);
				app_status_indication_set(APP_STATUS_INDICATION_CHARGING);
				osTimerStart(app_battery_timer, APP_BATTERY_CHARGING_PERIODIC_MS);
			}
		}
	} 
	else
	{
		ntc_capture_measure.recover_charge_cnt = 0;
		ntc_capture_measure.stop_charge_cnt = 0;
		if((ntc_capture_measure.currvolt < NTC_DISCHARGE_LOW_VOLTAGE) || (ntc_capture_measure.currvolt > NTC_DISCHARGE_HIGH_VOLTAGE)) {
			ntc_capture_measure.pwroff_cnt++;
		} else{
			ntc_capture_measure.pwroff_cnt = 0;
		}

		if(ntc_capture_measure.pwroff_cnt >= THERMAL_PROTECTION_DET_TIMES)
		{
			TRACE(0,"NTC-->POWER_OFF");
			osTimerStop(app_battery_timer);
			osTimerStop(ntc_sw_timer);
            app_shutdown();
		}
	}
#endif
/* End Add by lewis */
}

int ntc_capture_open(void)
{

    ntc_capture_measure.currvolt = 0;
    ntc_capture_measure.index = 0;
    ntc_capture_measure.temperature = 0;
    ntc_capture_measure.cb = NULL;
	/* Add by lewis */
	ntc_capture_measure.stop_charge_cnt = 0;
	ntc_capture_measure.recover_charge_cnt = 0;
	ntc_capture_measure.pwroff_cnt = 0;
	ntc_capture_measure.isChargeProtect = false;
	/* End Add by lewis */

    pmu_ntc_capture_enable();
/* Modify by Jay */
#ifdef CMT_008_NTC_DETECT
    hal_gpadc_open(HAL_GPADC_CHAN_4, HAL_GPADC_ATP_ONESHOT, ntc_capture_irqhandler);
#else
    hal_gpadc_open(HAL_GPADC_CHAN_0, HAL_GPADC_ATP_ONESHOT, ntc_capture_irqhandler);
#endif
/* End Modify by Jay */

    return 0;
}

int ntc_capture_start(void)
{
    pmu_ntc_capture_enable();
/* Modify by lewis */	
#ifdef CMT_008_NTC_DETECT
	hal_gpadc_open(HAL_GPADC_CHAN_4, HAL_GPADC_ATP_ONESHOT, ntc_capture_irqhandler);
#else
	hal_gpadc_open(HAL_GPADC_CHAN_0, HAL_GPADC_ATP_ONESHOT, ntc_capture_irqhandler);
#endif
/* End Modify by lewis */

    return 0;
}
#else

#define IS_USE_SOC_PMU_PLUGINOUT

#ifdef IS_USE_SOC_PMU_PLUGINOUT

#ifndef CHARGER_PLUGINOUT_DEBOUNCE_MS
#define CHARGER_PLUGINOUT_DEBOUNCE_MS (50)
#endif

#ifndef CHARGER_PLUGINOUT_DEBOUNCE_CNT
#define CHARGER_PLUGINOUT_DEBOUNCE_CNT (3)
#endif

static void app_battery_pluginout_debounce_start(void);
static void app_battery_pluginout_debounce_handler(void const *param);
osTimerDef (APP_BATTERY_PLUGINOUT_DEBOUNCE, app_battery_pluginout_debounce_handler);
static osTimerId app_battery_pluginout_debounce_timer = NULL;
static uint32_t app_battery_pluginout_debounce_ctx = 0;
static uint32_t app_battery_pluginout_debounce_cnt = 0;

static int app_battery_handle_process(APP_MESSAGE_BODY *msg_body)
{
    uint8_t status;

    APP_BATTERY_GET_STATUS(msg_body->message_id, status);

    if (status == APP_BATTERY_STATUS_PLUGINOUT){
        app_battery_pluginout_debounce_start();
    }

    return 0;
}

static void app_battery_event_process(enum APP_BATTERY_STATUS_T status, APP_BATTERY_MV_T volt)
{
    uint32_t app_battevt;
    APP_MESSAGE_BLOCK msg;

    APP_BATTERY_TRACE(3,"%s %d,%d",__func__, status, volt);
    msg.mod_id = APP_MODUAL_BATTERY;
    APP_BATTERY_SET_MESSAGE(app_battevt, status, volt);
    msg.msg_body.message_id = app_battevt;
    msg.msg_body.message_ptr = (uint32_t)NULL;
    app_mailbox_put(&msg);
}

static void app_battery_charger_handler(enum PMU_CHARGER_STATUS_T status)
{
    TRACE(2,"%s: status=%d", __func__, status);
    pmu_charger_set_irq_handler(NULL);
    app_battery_event_process(APP_BATTERY_STATUS_PLUGINOUT,
                              (status == PMU_CHARGER_PLUGIN) ? APP_BATTERY_CHARGER_PLUGIN : APP_BATTERY_CHARGER_PLUGOUT);
}

static enum APP_BATTERY_CHARGER_T app_battery_charger_forcegetstatus(void)
{
    enum APP_BATTERY_CHARGER_T status = APP_BATTERY_CHARGER_QTY;
    enum PMU_CHARGER_STATUS_T charger;

    charger = pmu_charger_get_status();

    if (charger == PMU_CHARGER_PLUGIN)
    {
        status = APP_BATTERY_CHARGER_PLUGIN;
    }
    else
    {
        status = APP_BATTERY_CHARGER_PLUGOUT;
    }

    return status;
}

static void app_battery_pluginout_debounce_start(void)
{
    TRACE(1,"%s", __func__);

    app_battery_pluginout_debounce_ctx = (uint32_t)app_battery_charger_forcegetstatus();
    app_battery_pluginout_debounce_cnt = 1;
    osTimerStart(app_battery_pluginout_debounce_timer, CHARGER_PLUGINOUT_DEBOUNCE_MS);
}

static void app_battery_pluginout_event_callback(enum APP_BATTERY_CHARGER_T event);

static void app_battery_pluginout_debounce_handler(void const *param)
{
    enum APP_BATTERY_CHARGER_T status_charger = app_battery_charger_forcegetstatus();

    if(app_battery_pluginout_debounce_ctx == (uint32_t) status_charger){
        app_battery_pluginout_debounce_cnt++;
    }
    else
    {
        TRACE(2,"%s dithering cnt %u", __func__, app_battery_pluginout_debounce_cnt);
        app_battery_pluginout_debounce_cnt = 0;
        app_battery_pluginout_debounce_ctx = (uint32_t)status_charger;
    }

    if (app_battery_pluginout_debounce_cnt >= CHARGER_PLUGINOUT_DEBOUNCE_CNT){
        TRACE(2,"%s %s", __func__, status_charger == APP_BATTERY_CHARGER_PLUGOUT ? "PLUGOUT" : "PLUGIN");

        app_battery_pluginout_event_callback(status_charger);
        pmu_charger_set_irq_handler(app_battery_charger_handler);
        osTimerStop(app_battery_pluginout_debounce_timer);
    }else{
        osTimerStart(app_battery_pluginout_debounce_timer, CHARGER_PLUGINOUT_DEBOUNCE_MS);
    }
}

int app_battery_charger_indication_open(void)
{
    enum APP_BATTERY_CHARGER_T status = APP_BATTERY_CHARGER_QTY;

    APP_BATTERY_TRACE(1,"%s",__func__);

    pmu_charger_init();

    pmu_charger_set_irq_handler(app_battery_charger_handler);

    return status;
}
#endif

int app_battery_open(void)
{
    app_battery_opened_callback();
#ifdef IS_USE_SOC_PMU_PLUGINOUT
    if (app_battery_pluginout_debounce_timer == NULL)
    {
        app_battery_pluginout_debounce_timer =
            osTimerCreate (osTimer(APP_BATTERY_PLUGINOUT_DEBOUNCE),
            osTimerOnce, &app_battery_pluginout_debounce_ctx);
    }

    app_set_threadhandle(APP_MODUAL_BATTERY, app_battery_handle_process);

    app_battery_charger_indication_open();
#endif

    // initialize the custom battery manager here
    // returned value could be:
    // #define APP_BATTERY_OPEN_MODE_INVALID        (-1)
    // #define APP_BATTERY_OPEN_MODE_NORMAL         (0)
    // #define APP_BATTERY_OPEN_MODE_CHARGING       (1)
    // #define APP_BATTERY_OPEN_MODE_CHARGING_PWRON (2)
    return APP_BATTERY_OPEN_MODE_NORMAL;
}

int app_battery_start(void)
{
    // start battery measurement timer here
    return 0;
}

int app_battery_stop(void)
{
    // stop battery measurement timer here
    return 0;
}

int app_battery_get_info(APP_BATTERY_MV_T *currvolt, uint8_t *currlevel, enum APP_BATTERY_STATUS_T *status)
{
    // should just return battery level via currlevel for hfp battery level indication
    *currlevel = APP_BATTERY_LEVEL_MAX;
    return 0;
}

#ifdef IS_USE_SOC_PMU_PLUGINOUT
static void app_battery_pluginout_event_callback(enum APP_BATTERY_CHARGER_T event)
{
    if (APP_BATTERY_CHARGER_PLUGOUT == event)
    {
        TRACE(0, "Charger plug out.");
    }
    else if (APP_BATTERY_CHARGER_PLUGIN == event)
    {
        TRACE(0, "Charger plug in.");
    }
}
#endif

int app_battery_register(APP_BATTERY_CB_T user_cb)
{
    // register the battery level update event callback
    return 0;
}

#endif

WEAK void app_battery_opened_callback(void)
{

}
