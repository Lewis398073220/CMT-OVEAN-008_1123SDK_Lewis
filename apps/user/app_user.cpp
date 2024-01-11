/***************************************************************************
 *
 * Add by lewis in 2023, which referenced Pang's code.
 * Copyright 2023 CMT.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of CMT.
 *
 * Use of this work is governed by a license granted by CMT.
 * This work contains confidential and proprietary information of
 * CMT. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "app_user.h"
#include "stdio.h"
#include "cmsis_os.h"
#include "list.h"
#include "string.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_bootmode.h"
#include "hal_sleep.h"
#include "pmu.h"
#include "audioflinger.h"
#include "apps.h"
#include "app_thread.h"
#include "app_key.h"
#include "bluetooth_bt_api.h"
#include "app_bt_media_manager.h"
#include "app_pwl.h"
#include "app_audio.h"
#include "app_overlay.h"
#include "app_battery.h"
#include "app_trace_rx.h"
#include "app_utils.h"
#include "app_status_ind.h"
#include "bt_drv_interface.h"
#include "besbt.h"
#include "norflash_api.h"
#include "nvrecord_appmode.h"
#include "nvrecord_bt.h"
#include "nvrecord_dev.h"
#include "nvrecord_env.h"
#include "crash_dump_section.h"
#include "log_section.h"
#include "factory_section.h"
#include "a2dp_api.h"
#include "me_api.h"
#include "btapp.h"
#include "app_bt.h"
#include "app_hfp.h"
#include "bt_if.h"
#include "app_media_player.h"
#include "hal_codec.h"
#include "hal_pwm.h"

#if defined(IBRT)
#include "app_ibrt_internal.h"
#include "app_ibrt_customif_ui.h"
#include "app_ibrt_voice_report.h"

#if defined(IBRT_UI)
#include "app_tws_ibrt_ui_test.h"
#include "app_ibrt_tws_ext_cmd.h"
#include "app_tws_ibrt_conn_api.h"
#include "app_custom_api.h"
#include "app_ibrt_auto_test.h"
#include "app_tws_ctrl_thread.h"
#endif
#include "earbud_ux_api.h"
#endif

#ifdef ANC_APP
#include "app_anc.h"
#endif

#ifdef CMT_008_AC107_ADC
#include "ac107.h"
#endif

#ifdef CMT_008_CST820_TOUCH
#include "cst_capacitive_tp_hynitron_cst0xx.h"
#include "cst_ctp_hynitron_ext.h"
#endif

/********************************************** User Info Start **********************************************/
app_user_custom_data_t user_data;

bool user_custom_is_touch_locked(void)
{
    return user_data.touch_lock;
}

void user_custom_lock_unlock_touch(bool isEn, bool isSave)
{
	user_data.touch_lock = isEn;

	if(isSave)
	{
		struct nvrecord_user_t *nvrecord_user;

		nv_record_user_info_get(&nvrecord_user);
		nvrecord_user->touch_lock = isEn;
		nv_record_user_info_set(nvrecord_user);
	}
}

bool user_custom_is_prompt_en(void)
{
	return user_data.prompt_vol_en;
}

void user_custom_en_dis_prompt(bool isEn, bool isSave)
{
	user_data.prompt_vol_en = isEn;

	if(isSave)
	{
		struct nvrecord_user_t *nvrecord_user;

		nv_record_user_info_get(&nvrecord_user);
		nvrecord_user->prompt_vol_en = isEn;
		nv_record_user_info_set(nvrecord_user);
	}
}

uint8_t user_custom_get_prompt_volume_level(void)
{
	return user_data.prompt_vol_level;
}

void user_custom_set_prompt_volume_level(uint8_t vol, bool isSave)
{
	user_data.prompt_vol_level = vol;

	if(isSave)
	{
		struct nvrecord_user_t *nvrecord_user;

		nv_record_user_info_get(&nvrecord_user);
		nvrecord_user->prompt_vol_level = vol;
		nv_record_user_info_set(nvrecord_user);
	}
}

void nvrecord_user_info_init_for_ota(struct nvrecord_user_t *pUserInfo)
{
	uint8_t saved_user_info_ver[13];
			
	TRACE(0, "*** [%s] enter", __func__);

	snprintf((char *)saved_user_info_ver, sizeof(saved_user_info_ver), "V%d.%d.%d",
			pUserInfo->nvrecord_user_ver[2], pUserInfo->nvrecord_user_ver[1], pUserInfo->nvrecord_user_ver[0]);

	//init user info according to version in flash 
	if(strncmp((const char *)saved_user_info_ver, "V0.0.0", strlen("V0.0.0")) == 0)
	{
		//default touch config
		pUserInfo->touch_lock = false;
	
		//default prompt volume config
		pUserInfo->prompt_vol_en = true;
		pUserInfo->prompt_vol_level = MEDIA_VOLUME_LEVEL_WARNINGTONE;
	}
	
	//update user info's history
	pUserInfo->nvrecord_user_ver[2] = NV_USER_VERSION_H;
	pUserInfo->nvrecord_user_ver[1] = NV_USER_VERSION_M;
	pUserInfo->nvrecord_user_ver[0] = NV_USER_VERSION_L;

	nv_record_user_info_set(pUserInfo);
	nv_record_flash_flush();

	TRACE(0, "*** [%s] exit", __func__);
}

void user_custom_nvrecord_user_info_get(void)
{
	struct nvrecord_user_t *nvrecord_user;
	uint8_t saved_user_info_ver[13];
	uint8_t local_user_info_ver[13];
		
	nv_record_user_info_get(&nvrecord_user);
	
	snprintf((char *)saved_user_info_ver, sizeof(saved_user_info_ver), "V%d.%d.%d",
		nvrecord_user->nvrecord_user_ver[2], nvrecord_user->nvrecord_user_ver[1], nvrecord_user->nvrecord_user_ver[0]);
	snprintf((char *)local_user_info_ver, sizeof(local_user_info_ver), "V%d.%d.%d",
		NV_USER_VERSION_H, NV_USER_VERSION_M, NV_USER_VERSION_L);
	TRACE(0, "*** [%s] user info version in flash: %s", __func__, saved_user_info_ver);
	//if local user info version is different from user info version in flash, init user info
	if(strncmp((const char *)saved_user_info_ver, (const char *)local_user_info_ver, strlen((const char *)local_user_info_ver)) != 0)
	{
		nvrecord_user_info_init_for_ota(nvrecord_user);
	}
	//update local user info version
	user_data.nvrecord_user_ver_H = nvrecord_user->nvrecord_user_ver[2];
	user_data.nvrecord_user_ver_M = nvrecord_user->nvrecord_user_ver[1];
	user_data.nvrecord_user_ver_L = nvrecord_user->nvrecord_user_ver[0];
	
	user_data.touch_lock = nvrecord_user->touch_lock;
	TRACE(0, "*** [%s] touch lock: %d", __func__, user_data.touch_lock);
		
	user_data.prompt_vol_en = nvrecord_user->prompt_vol_en;
	user_data.prompt_vol_level = nvrecord_user->prompt_vol_level;
	TRACE(0, "*** [%s] prompt en/level: %d/%d", __func__, user_data.prompt_vol_en, user_data.prompt_vol_level);
}

void user_custom_nvrecord_rebuild_user_info(uint8_t *pUserInfo, bool isRebuildAll)
{
	struct nvrecord_user_t *user_info = (struct nvrecord_user_t *)pUserInfo;

	//when BES chip is blank, nv_record_extension_init
	if(isRebuildAll)
	{
		//Record user info's history
		user_info->nvrecord_user_ver[0] = NV_USER_VERSION_L;
		user_info->nvrecord_user_ver[1] = NV_USER_VERSION_M;
		user_info->nvrecord_user_ver[2] = NV_USER_VERSION_H;
	}
	//factory reset, nvrecord_rebuild_sdk_info
	else
	{
		
	}

	//default touch config
	user_info->touch_lock = false;
	
	//default prompt volume config
	user_info->prompt_vol_en = true;
	user_info->prompt_vol_level = MEDIA_VOLUME_LEVEL_WARNINGTONE;
}
/********************************************** User Info End **********************************************/


/********************************************** User Module Start **********************************************/
USER_MODULE_DATA_T user_module_data;

#ifdef CMT_008_3_5JACK_CTR

#ifdef AUDIO_LINEIN
extern int app_play_linein_onoff(bool onoff);
#endif

APP_JACK_STATUS_T app_3_5jack_status_get(void)
{
	ASSERT(JACK_DET_PIN != HAL_IOMUX_PIN_NUM, "[%s] !!!3.5 jack det pin is not defined", __func__);

	uint8_t io_read_val;
	io_read_val = hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)JACK_DET_PIN);
	TRACE(0, "%s: %d", __func__, io_read_val);

	if(io_read_val) {
		return APP_JACK_STATUS_PLUGIN;
	} else{
		return APP_JACK_STATUS_PLUGOUT;
	}
}

bool app_is_3_5jack_inplug(void)
{
	return user_module_data.is3_5JackInplug;
}

osTimerId jack_sw_timer = NULL;
static void jack_detn_handler(void const *param);
osTimerDef(JACK_DETN_TIMER, jack_detn_handler);// define timers
#define JACK_QUICK_SWTIMER_MS	     (100)
#define JACK_SLOW_SWTIMER_MS	     (500)
#define JACK_PLUGINOUT_DEBOUNCE_CNT  (3) 

void app_3_5jack_swtimer_start(uint32 periodic_ms)
{
	TRACE(0,"%s %d",__func__, periodic_ms);
	
	if(jack_sw_timer == NULL)
		jack_sw_timer = osTimerCreate(osTimer(JACK_DETN_TIMER), osTimerOnce, NULL);

	osTimerStop(jack_sw_timer);
	osTimerStart(jack_sw_timer,periodic_ms);
}

void app_3_5jack_swtimer_stop(void)
{
	TRACE(0,"%s",__func__);

	if(jack_sw_timer == NULL)
		return;
	
	osTimerStop(jack_sw_timer);
}

static void jack_irq_update(void);
static void jack_irq_disable(void);
static void jack_detn_handler(void const *param)
{
	TRACE(0,"%s",__func__);

	APP_JACK_STATUS_T status_3_5jack = app_3_5jack_status_get();

	if(user_module_data.jack_pluginout_debounce_ctx == status_3_5jack) {
		user_module_data.jack_pluginout_debounce_cnt++;
	} else{
		TRACE(2,"%s dithering cnt %u", __func__, user_module_data.jack_pluginout_debounce_cnt);
		user_module_data.jack_pluginout_debounce_cnt = 0;
		user_module_data.jack_pluginout_debounce_ctx = status_3_5jack;
	}

	if(user_module_data.jack_pluginout_debounce_cnt >= JACK_PLUGINOUT_DEBOUNCE_CNT) {
        TRACE(2,"%s %s", __func__, status_3_5jack == APP_JACK_STATUS_PLUGIN ? "PLUGIN" : "PLUGOUT");

		if(status_3_5jack == true)
		{
			user_module_data.is3_5JackInplug = true;

			if(user_module_data.jack_pluginout_debounce_cnt == JACK_PLUGINOUT_DEBOUNCE_CNT)
			{
#ifdef CMT_008_CST820_TOUCH
				hal_set_cst820_rst_low(); //TODO: when new PCBA done, delete it
#endif
#if defined(BT_USB_AUDIO_DUAL_MODE) || defined(BTUSB_AUDIO_MODE)
				if(app_battery_is_charging()) {
					TRACE(0,"!!!jack is plugged in when charging-->Shutdown");
					jack_irq_disable();
		            app_battery_stop();
		            app_shutdown();
					return;
				}
#endif

				//app_ibrt_if_event_entry(APP_UI_EV_DOCK);
	       		app_ibrt_if_event_entry(APP_UI_EV_CASE_CLOSE);

#ifdef CMT_008_AC107_ADC
				app_audio_linein_key_init();

				ac107_hw_open();
				ac107_i2c_init();
#endif				
			}

			//delay some time to init ac107
			if(user_module_data.jack_pluginout_debounce_cnt == JACK_PLUGINOUT_DEBOUNCE_CNT + 1)
			{
#ifdef CMT_008_AC107_ADC
				ac107_hw_init();
#endif
			}

			//delay some time to open audio linein
			if(user_module_data.jack_pluginout_debounce_cnt >= JACK_PLUGINOUT_DEBOUNCE_CNT + 2)
			{
#ifdef AUDIO_LINEIN
				if(!bt_media_is_media_active_by_type(BT_STREAM_MUSIC | BT_STREAM_MEDIA | BT_STREAM_VOICE))
				{
					//app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_START, BT_STREAM_LINEIN, BT_DEVICE_ID_1, 0); //lewis: use this will cause some pop noise, I don't know why
					hal_codec_dac_mute(1);
					osDelay(200);
					app_play_linein_onoff(true);
					osDelay(200);
					hal_codec_dac_mute(0);
					
					jack_irq_update();

					app_status_indication_set(APP_STATUS_INDICATION_AUDIO_LINEIN);
					return;
				}
#endif
			}

			app_3_5jack_swtimer_start(JACK_SLOW_SWTIMER_MS);
		}
		else
		{
			user_module_data.is3_5JackInplug = false;
#ifdef CMT_008_CST820_TOUCH
			hal_set_cst820_rst_high(); //TODO: when new PCBA done, delete it
#endif
			app_key_init();
			
#ifdef AUDIO_LINEIN
			//app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP, BT_STREAM_LINEIN, BT_DEVICE_ID_1, 0); //lewis: use this will cause some pop noise, I don't know why
			app_play_linein_onoff(false);
#endif

#ifdef CMT_008_AC107_ADC
			ac107_hw_close();
#endif

			app_ibrt_if_event_entry(APP_UI_EV_CASE_OPEN);
        	//app_ibrt_if_event_entry(APP_UI_EV_UNDOCK);
			app_bt_profile_connect_manager_opening_reconnect();

			jack_irq_update();
		}
	} else{
		app_3_5jack_swtimer_start(JACK_QUICK_SWTIMER_MS);
	}
}

static int jack_msg_post(void)
{
    APP_MESSAGE_BLOCK msg;

    msg.mod_id = APP_MODUAL_USERDEF;
    msg.msg_body.message_id = USER_EVENT_3_5JACK;
    msg.msg_body.message_ptr = (uint32_t)NULL;
    app_mailbox_put(&msg);
	
    return 0;
}

static void jack_irq_handler(enum HAL_GPIO_PIN_T pin)
{
	TRACE(0, "*** [%s] %s", __func__, (pin == (enum HAL_GPIO_PIN_T)JACK_DET_PIN)? "3_5jack intr trigger" : "not 3_5jack det pin, ignore");

	if(pin != (enum HAL_GPIO_PIN_T)JACK_DET_PIN)
	{
		return;
	}

	jack_irq_disable();

	user_module_data.jack_pluginout_debounce_ctx = app_3_5jack_status_get();
	user_module_data.jack_pluginout_debounce_cnt = 1;
	
	jack_msg_post();
}

static void jack_irq_set(enum HAL_GPIO_IRQ_POLARITY_T polarity)
{
	TRACE(2,"%s %s", __func__, polarity == HAL_GPIO_IRQ_POLARITY_HIGH_RISING ? "HIGH_RISING" : "LOW_FALLING");

	struct HAL_GPIO_IRQ_CFG_T cfg;
		
	cfg.irq_debounce = 1;
	cfg.irq_enable = 1;
	cfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
	cfg.irq_handler = jack_irq_handler;
	cfg.irq_polarity = polarity;
		
	hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)JACK_DET_PIN, &cfg);
}

static void jack_irq_update(void)
{
	if(hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)JACK_DET_PIN))
		jack_irq_set(HAL_GPIO_IRQ_POLARITY_LOW_FALLING);
	else
		jack_irq_set(HAL_GPIO_IRQ_POLARITY_HIGH_RISING);
}

static void jack_irq_disable(void)
{
	TRACE(0,"%s",__func__);

	struct HAL_GPIO_IRQ_CFG_T cfg;
		
	cfg.irq_debounce = 1;
	cfg.irq_enable = 0;
	cfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
	cfg.irq_handler = NULL;
	cfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
		
	hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)JACK_DET_PIN, &cfg);

	cfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_HIGH_RISING;
	hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)JACK_DET_PIN, &cfg);
}

int32_t app_3_5jack_module_open(void)
{
	TRACE(0,"%s",__func__);

	APP_JACK_STATUS_T status_3_5jack = app_3_5jack_status_get();

	user_module_data.is3_5JackInplug = false;
	
	//when 3.5jack is inplugged, then power on
	if(status_3_5jack == APP_JACK_STATUS_PLUGIN) 
	{
		user_module_data.jack_pluginout_debounce_ctx = status_3_5jack;
		user_module_data.jack_pluginout_debounce_cnt = 1;
		app_3_5jack_swtimer_start(JACK_QUICK_SWTIMER_MS);
	} 
	else
	{
		jack_irq_update();
	}
	
	return 0;
}
#endif

#ifdef CMT_008_EN_LED_BREATH
//Please note pwm_id needs to match GPIO
const HAL_PIN_PWM_CFG cfg_hw_pinmux_pwm[CFG_HW_PWM_NUM] = {
#if (CFG_HW_PWM_NUM > 0)
    //white led
    {HAL_PWM_ID_2, {HAL_IOMUX_PIN_P3_2, HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}},
    //Red led
    {HAL_PWM1_ID_2, {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_PWM6, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}},
#endif
};

APP_BREATH_T app_breath[APP_BREATH_ID_QTY];

void app_test_pwm_set(uint8_t level)
{
     struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pinmux;
     struct HAL_PWM_CFG_T cfg;

     // make sure breath led run normal, which add power
     if(level == 0 || level == 100)
     {
        app_sysfreq_req(APP_SYSFREQ_USER_APP_12, APP_SYSFREQ_32K);
     }
     else if(level >= 1)
     {
        app_sysfreq_req(APP_SYSFREQ_USER_APP_12, APP_SYSFREQ_26M);
     }

     if(level == 0 || level == 100)
     {
        hal_pwm_disable(cfg_hw_pinmux_pwm[APP_BREATH_ID_1].pwm_id);
		cfg_hw_pinmux = cfg_hw_pinmux_pwm[APP_BREATH_ID_1].pinmux_pwm;
        cfg_hw_pinmux.function = HAL_IOMUX_FUNC_AS_GPIO;
        hal_iomux_init(&cfg_hw_pinmux, 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_hw_pinmux.pin, HAL_GPIO_DIR_OUT, level?1:0);
     }
     else
     {        
        cfg.freq = PWM_FREQ;
        cfg.inv = false;
        cfg.sleep_on = true;
        cfg.ratio = level;
        hal_pwm_enable(cfg_hw_pinmux_pwm[APP_BREATH_ID_1].pwm_id, &cfg);
        hal_iomux_init(&(cfg_hw_pinmux_pwm[APP_BREATH_ID_1].pinmux_pwm), 1);
     }
}

void app_pwm_set(APP_BREATH_ID_T id, uint8_t ratio)
{
     struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pinmux;
     struct HAL_PWM_CFG_T cfg;

	 if(id >= APP_BREATH_ID_QTY) return;

	 if(ratio > 100) ratio = 100;
	 
     //make sure breath led run normal, which add power
     if(ratio == 0 || ratio == 100)
     {
        app_sysfreq_req(APP_SYSFREQ_USER_APP_12, APP_SYSFREQ_32K);
     }
     else if(ratio >= 1)
     {
        app_sysfreq_req(APP_SYSFREQ_USER_APP_12, APP_SYSFREQ_26M);
     }

     if(ratio == 0 || ratio == 100)
     {
        hal_pwm_disable(cfg_hw_pinmux_pwm[id].pwm_id);
		cfg_hw_pinmux = cfg_hw_pinmux_pwm[id].pinmux_pwm;
        cfg_hw_pinmux.function = HAL_IOMUX_FUNC_AS_GPIO;
        hal_iomux_init(&cfg_hw_pinmux, 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_hw_pinmux.pin, HAL_GPIO_DIR_OUT, ratio?1:0);
     }
     else
     {        
        cfg.freq = PWM_FREQ;
        cfg.inv = false;
        cfg.sleep_on = true;
        cfg.ratio = ratio;
        hal_pwm_enable(cfg_hw_pinmux_pwm[id].pwm_id, &cfg);
        hal_iomux_init(&(cfg_hw_pinmux_pwm[id].pinmux_pwm), 1);
     }
}

static void app_breath_led_timehandler(void *param)
{
	APP_BREATH_T *pwm = (APP_BREATH_T *)param;
	APP_BREATH_CFG_T *cfg = &(pwm->config);

	//TRACE(1,"%s",__func__);

	hwtimer_stop(pwm->timer);

	switch(pwm->cur_stage)
	{
		case APP_BREATH_STAGE_HIGH:
			if(pwm->cur_ratio >= 100)
			{
				pwm->cur_stage = APP_BREATH_STAGE_ON;
				hwtimer_start(pwm->timer, MS_TO_TICKS(cfg->on_state_time));
			}
			else
			{
				pwm->cur_ratio++;			
				app_pwm_set(pwm->id, pwm->cur_ratio);
				hwtimer_start(pwm->timer, MS_TO_TICKS(pwm->high_time_step));
			}
		break;

		case APP_BREATH_STAGE_ON:
			pwm->cur_ratio = 100;
			pwm->cur_stage = APP_BREATH_STAGE_LOW;
			hwtimer_start(pwm->timer, MS_TO_TICKS(pwm->low_time_step));
		break;

		case APP_BREATH_STAGE_LOW:
			if(pwm->cur_ratio == 0)
			{
				pwm->cur_stage = APP_BREATH_STAGE_OFF;
				hwtimer_start(pwm->timer, MS_TO_TICKS(cfg->off_state_time));
			}
			else
			{
				pwm->cur_ratio--;
				app_pwm_set(pwm->id, pwm->cur_ratio);
				hwtimer_start(pwm->timer, MS_TO_TICKS(pwm->low_time_step));
			}
		break;

		case APP_BREATH_STAGE_OFF:
			pwm->cur_ratio = 1;
			pwm->cur_stage = APP_BREATH_STAGE_HIGH;
			hwtimer_start(pwm->timer, MS_TO_TICKS(pwm->high_time_step));
		break;
		
		default:
		break;
	}
}

int app_breath_led_open(void)
{
#if (CFG_HW_PWM_NUM > 0)
	uint8_t i;

	TRACE(1,"%s",__func__);
	for(i = 0; i < APP_BREATH_ID_QTY; i++)
	{
		app_breath[i].id = APP_BREATH_ID_QTY;
		memset(&(app_breath[i].config), 0, sizeof(APP_BREATH_CFG_T));
		//not need to init GPIO or enable PWM
	}
	app_breath[APP_BREATH_ID_0].timer = hwtimer_alloc(app_breath_led_timehandler, &app_breath[APP_BREATH_ID_0]);
#if (CFG_HW_PWM_NUM == 2)
	app_breath[APP_BREATH_ID_1].timer = hwtimer_alloc(app_breath_led_timehandler, &app_breath[APP_BREATH_ID_1]);
#endif
#endif

	return 0;
}

int app_breath_led_setup(APP_BREATH_ID_T id, APP_BREATH_CFG_T *cfg)
{
#if (CFG_HW_PWM_NUM > 0)
	if(cfg == NULL || id >= APP_BREATH_ID_QTY) return -1;
	TRACE(1,"%s",__func__);

	app_breath[id].id = id;
	memcpy(&(app_breath[id].config), cfg, sizeof(APP_BREATH_CFG_T));
	
	hwtimer_stop(app_breath[id].timer);
#endif

	return 0;
}

int app_breath_led_start(APP_BREATH_ID_T id)
{	
#if (CFG_HW_PWM_NUM > 0)
	APP_BREATH_T *breath = NULL;
	APP_BREATH_CFG_T *cfg = NULL;
	
	if(id >= APP_BREATH_ID_QTY) return -1;
	TRACE(1,"%s",__func__);

	breath = &app_breath[id];
	cfg = &(breath->config);

	if(breath->id >= APP_BREATH_ID_QTY) return -1;

	//high_time_step = high_time / ratio_max = high_time / 100
	//high_time_step is equal to 1ms at least
	breath->high_time_step = (cfg->high_time / 100)? (cfg->high_time / 100) : 1;
	breath->low_time_step = (cfg->low_time / 100)? (cfg->low_time / 100) : 1;
	breath->cur_ratio = 1; //first fade in, so cur_ratio=1
	breath->cur_stage = APP_BREATH_STAGE_HIGH; //first fade in
	
	hwtimer_stop(breath->timer);
	app_pwm_set(id, breath->cur_ratio);
	hwtimer_start(breath->timer, MS_TO_TICKS(breath->high_time_step)); //first fade in
#endif

	return 0;
}

int app_breath_led_stop(APP_BREATH_ID_T id)
{
#if (CFG_HW_PWM_NUM > 0)
    if(id >= APP_BREATH_ID_QTY) return -1;

	hwtimer_stop(app_breath[id].timer);
	app_pwm_set(app_breath[id].id, 0);
#endif

    return 0;
}

uint8_t app_breath_led_init(void)
{
	APP_BREATH_CFG_T cfg;

	memset(&cfg, 0, sizeof(APP_BREATH_CFG_T));
	app_breath_led_open();
	app_breath_led_setup(APP_BREATH_ID_0, &cfg);
	app_breath_led_setup(APP_BREATH_ID_1, &cfg);

	return 0;
}

#endif

static int app_user_event_handle_process(APP_MESSAGE_BODY *msg_body)
{   
    uint32_t evt = msg_body->message_id;

	switch (evt)
    {
#ifdef CMT_008_3_5JACK_CTR
		case USER_EVENT_3_5JACK:
			app_3_5jack_swtimer_start(JACK_QUICK_SWTIMER_MS);
		break;
#endif

		default:
		break;
	}

	return 0;
}

void app_user_event_open_module(void)
{
	TRACE(0,"%s",__func__);

	app_set_threadhandle(APP_MODUAL_USERDEF, app_user_event_handle_process);

#ifdef CMT_008_3_5JACK_CTR
	app_3_5jack_module_open();
#endif
}

void app_user_event_close_module(void)
{
	app_set_threadhandle(APP_MODUAL_USERDEF, NULL);

	
}
/********************************************** User Module End **********************************************/


void en_dis_charging(bool isEn)
{
	TRACE(0, "%s: %d", __func__, isEn);
	
	ASSERT(app_battery_ext_charger_enable_cfg.pin != HAL_IOMUX_PIN_NUM, "[%s] !!!charging enable pin is not defined", __func__);
	
	if(isEn) {
		hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_battery_ext_charger_enable_cfg.pin, HAL_GPIO_DIR_OUT, 0);
	} else{
		hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_battery_ext_charger_enable_cfg.pin, HAL_GPIO_DIR_OUT, 1);
	}
}


#ifdef CMT_008_UART_USBAUDIO_SWITCH
void uart_usbaudio_start_switch_to(UARTUSB_SWITCH switcher)
{
	const char *str = NULL;
	if(switcher == UARTUSB_SWITCH_UART)
	{
		str = "[UARTUSB_SWITCH_UART]";
	} else if(switcher == UARTUSB_SWITCH_USB){
		str = "[UARTUSB_SWITCH_USB]";
	} else{
		str = "[UNKNOW]";
	}
	
	TRACE(0, "%s %s", __func__, str);

	ASSERT(cfg_hw_uart_usbaudio_switch.pin != HAL_IOMUX_PIN_NUM, "[%s] !!!usbaudio switch pin is not defined", __func__);

	if(switcher == UARTUSB_SWITCH_UART) {
		hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_hw_uart_usbaudio_switch.pin, HAL_GPIO_DIR_OUT, 0);
	} else if(switcher == UARTUSB_SWITCH_USB){
		hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_hw_uart_usbaudio_switch.pin, HAL_GPIO_DIR_OUT, 1);
	}
}
#endif

#ifdef CMT_008_CHARGE_CURRENT
void charge_current_switch_to(CHARGING_SPEED speed)
{
	const char *str = NULL;
	if(speed == CHARGING_SPEED_HIGH)
	{
		str = "[CHARGING_SPEED_HIGH]";
	} else if(speed == CHARGING_SPEED_LOW){
		str = "[CHARGING_SPEED_LOW]";
	} else{
		str = "[UNKNOW]";
	}
	
	TRACE(0, "%s %s", __func__, str);

	ASSERT(cfg_hw_charge_current_control.pin != HAL_IOMUX_PIN_NUM, "[%s] !!!current control pin is not defined", __func__);

	if(speed == CHARGING_SPEED_HIGH) {
		hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_hw_charge_current_control.pin, HAL_GPIO_DIR_OUT, 1);
	} else if(speed == CHARGING_SPEED_LOW){
		hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_hw_charge_current_control.pin, HAL_GPIO_DIR_OUT, 0);
	}
}
#endif
