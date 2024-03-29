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
#include "a2dp_api.h"
#include "me_api.h"
#include "btapp.h"
#include "app_bt.h"
#include "app_hfp.h"
#include "bt_if.h"
#include "app_media_player.h"
#include "hal_codec.h"
#include "hal_pwm.h"
#include "iir_process.h"
#include "aud_section.h"
#include "co_math.h"
#include "customparam_section.h"

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
extern const IIR_CFG_T * const audio_eq_hw_dac_iir_cfg_list[];
extern const struct_anc_cfg * anc_coef_list_50p7k[ANC_COEF_LIST_NUM];
extern const struct_anc_cfg * anc_coef_list_48k[ANC_COEF_LIST_NUM];
extern const struct_anc_cfg * anc_coef_list_44p1k[ANC_COEF_LIST_NUM];

extern void app_set_10_second_timer(uint8_t timer_id, uint8_t enable, uint32_t period);
extern uint32_t app_get_count_of_10_second_timer(uint8_t timer_id);
extern uint32_t app_get_period_of_10_second_timer(uint8_t timer_id);

app_user_custom_data_t user_data;
int32_t awareness_mode_gain_table[] = {   
	153, 287, 512
};

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

uint8_t user_custom_get_LR_balance_value(void)
{
	return user_data.LR_balance_val;
}

void user_custom_set_LR_balance_value(uint8_t val, bool isSave)
{
	user_data.LR_balance_val = val;

	if(isSave)
	{
		struct nvrecord_user_t *nvrecord_user;

		nv_record_user_info_get(&nvrecord_user);
		nvrecord_user->LR_balance_val = val;
		nv_record_user_info_set(nvrecord_user);
	}
}

const char *user_custom_get_BT_name(void)
{
	return user_data.redefine_BT_name;
}

void user_custom_set_BT_name(char* name, bool isSave)
{
	uint16_t name_len = 0;

	if(strlen(name) < BT_NAME_LEN) {
		name_len = strlen(name);
	} else{
		name_len = BT_NAME_LEN - 1;
	}

	memset(user_data.redefine_BT_name, 0, sizeof(user_data.redefine_BT_name));
	memcpy(user_data.redefine_BT_name, name, name_len);

	if(isSave)
	{
		struct nvrecord_user_t *nvrecord_user;

		nv_record_user_info_get(&nvrecord_user);
		memset(nvrecord_user->redefine_BT_name, 0, sizeof(nvrecord_user->redefine_BT_name));
		memcpy(nvrecord_user->redefine_BT_name, name, name_len);
		nv_record_user_info_set(nvrecord_user);
	}
}

TOTA_BLE_EQ_MAP user_custom_get_EQ_mode(void)
{
	return user_data.eq_mode;
}

void user_custom_set_EQ_mode(TOTA_BLE_EQ_MAP mode, bool isSave)
{
	user_data.eq_mode = mode;

	if(isSave)
	{
		struct nvrecord_user_t *nvrecord_user;

		nv_record_user_info_get(&nvrecord_user);
		nvrecord_user->eq_mode = mode;
		nv_record_user_info_set(nvrecord_user);
	}
}

void update_user_EQ(USER_IIR_CFG_T user_eq)
{
	IIR_CFG_T **audio_eq_list = NULL;
	uint8_t i = 0;
	
	audio_eq_list = (IIR_CFG_T **)audio_eq_hw_dac_iir_cfg_list;
	TRACE(0, "%s enter", __func__);
	
	TRACE(0, "update user EQ of ANC On");
	//audio_eq_list[USER_EQ_ANC_ON]->gain0 = user_eq.gain0;//don't need
	//audio_eq_list[USER_EQ_ANC_ON]->gain1 = user_eq.gain1;//don't need
	//audio_eq_list[USER_EQ_ANC_ON]->num = user_eq.num;//don't need
	for(i = 0; i < user_eq.num; i++)
	{
		audio_eq_list[USER_EQ_ANC_ON]->param[i] = user_eq.param[i];
	}
	TRACE(0, "gain0/gain1: %d/%d(/100)", (int32_t)(audio_eq_list[USER_EQ_ANC_ON]->gain0 * 100),
		(int32_t)(audio_eq_list[USER_EQ_ANC_ON]->gain1 * 100));
	TRACE(0, "num: %d", audio_eq_list[USER_EQ_ANC_ON]->num);
	for(i = 0; i < audio_eq_list[USER_EQ_ANC_ON]->num; i++)
	{
		TRACE(0, "type: %d fc/gain/Q: %d/%d/%d(/100)", audio_eq_list[USER_EQ_ANC_ON]->param[i].type, (int32_t)(audio_eq_list[USER_EQ_ANC_ON]->param[i].fc * 100),
			(int32_t)(audio_eq_list[USER_EQ_ANC_ON]->param[i].gain * 100), (int32_t)(audio_eq_list[USER_EQ_ANC_ON]->param[i].Q * 100));
	}

	TRACE(0, "update user EQ of ANC Off");
	//audio_eq_list[USER_EQ_ANC_OFF]->gain0 = user_eq.gain0;//don't need
	//audio_eq_list[USER_EQ_ANC_OFF]->gain1 = user_eq.gain1;//don't need
	//audio_eq_list[USER_EQ_ANC_OFF]->num = user_eq.num;//don't need
	for(i = 0; i < user_eq.num; i++)
	{
		audio_eq_list[USER_EQ_ANC_OFF]->param[i] = user_eq.param[i];
	}
	TRACE(0, "gain0/gain1: %d/%d(/100)", (int32_t)(audio_eq_list[USER_EQ_ANC_OFF]->gain0 * 100),
		(int32_t)(audio_eq_list[USER_EQ_ANC_OFF]->gain1 * 100));
	TRACE(0, "num: %d", audio_eq_list[USER_EQ_ANC_OFF]->num);
	for(i = 0; i < audio_eq_list[USER_EQ_ANC_OFF]->num; i++)
	{
		TRACE(0, "type: %d fc/gain/Q: %d/%d/%d(/100)", audio_eq_list[USER_EQ_ANC_OFF]->param[i].type, (int32_t)(audio_eq_list[USER_EQ_ANC_OFF]->param[i].fc * 100),
			(int32_t)(audio_eq_list[USER_EQ_ANC_OFF]->param[i].gain * 100), (int32_t)(audio_eq_list[USER_EQ_ANC_OFF]->param[i].Q * 100));
	}
	
	TRACE(0, "%s exit", __func__);
}

void user_custom_get_user_EQ(USER_IIR_CFG_T *user_eq)
{
	*user_eq = user_data.user_eq;
}

void user_custom_set_user_EQ(USER_IIR_CFG_T user_eq, bool isSave)
{
	user_data.user_eq = user_eq;
	update_user_EQ(user_data.user_eq);

	if(isSave)
	{
		struct nvrecord_user_t *nvrecord_user;

		nv_record_user_info_get(&nvrecord_user);
		nvrecord_user->user_eq = user_eq;
		nv_record_user_info_set(nvrecord_user);
	}
}

bool user_custom_is_sidetone_on(void)
{
	return user_data.sidetone_on;
}

void user_custom_on_off_sidetone(bool isOn, bool isSave)
{
	user_data.sidetone_on = isOn;

	if(isSave)
	{
		struct nvrecord_user_t *nvrecord_user;

		nv_record_user_info_get(&nvrecord_user);
		nvrecord_user->sidetone_on = isOn;
		nv_record_user_info_set(nvrecord_user);
	}
}

uint16_t user_custom_get_shutdown_time(void)
{
	TRACE(0, "%s min: %d", __func__, user_data.shutdown_time);

	return user_data.shutdown_time;
}

void user_custom_set_shutdown_time(uint16_t minute, bool isSave)
{
	user_data.shutdown_time = minute;

	if(isSave)
	{
		struct nvrecord_user_t *nvrecord_user;

		nv_record_user_info_get(&nvrecord_user);
		nvrecord_user->shutdown_time = minute;
		nv_record_user_info_set(nvrecord_user);
	}
}

void update_power_savingmode_shutdown_timer(uint16_t minute, bool isEn)
{
	uint32_t second = 0;

	TRACE(0, "%s min: %d, isEn: %d", __func__, minute, isEn);

	if((minute > BLE_SHUTDOWN_TIME_MAP_SHUTDOWN_NOW) && (minute <= BLE_SHUTDOWN_TIME_MAP_NEVER_SHUTDOWN))
	{
		second = minute * 60 / 10;
	
		switch(minute)
		{
			case BLE_SHUTDOWN_TIME_MAP_NEVER_SHUTDOWN:
				app_set_10_second_timer(APP_POWER_SAVINGMODE_SHUTDOWN_TIMER_ID, 
					false, second);
			break;

			default:
				app_set_10_second_timer(APP_POWER_SAVINGMODE_SHUTDOWN_TIMER_ID, 
					isEn, second);
			break;
		}
	}
}

uint16_t user_custom_get_remaining_time(void)
{
	uint32_t timer_count = 0;
	uint32_t timer_period = 0;
	uint16_t remaining_time = 0;
	
	timer_count = app_get_count_of_10_second_timer(APP_POWER_SAVINGMODE_SHUTDOWN_TIMER_ID);
	timer_period = app_get_period_of_10_second_timer(APP_POWER_SAVINGMODE_SHUTDOWN_TIMER_ID);

	remaining_time = (timer_period - timer_count) * 10 / 60;
	
	TRACE(0, "%s remaining min: %d", __func__, remaining_time);

	return remaining_time;
}

bool user_custom_is_shutdown_timer_need_repeat(void)
{
    return user_data.is_shutdown_timer_need_repeat;
}

void user_custom_on_off_shutdown_timer_repeat_switch(bool isOn, bool isSave)
{
	user_data.is_shutdown_timer_need_repeat = isOn;

	if(isSave)
	{
		struct nvrecord_user_t *nvrecord_user;

		nv_record_user_info_get(&nvrecord_user);
		nvrecord_user->is_shutdown_timer_need_repeat = isOn;
		nv_record_user_info_set(nvrecord_user);
	}
}

uint8_t user_custom_get_nr_mode_level(void)
{
	return user_data.nr_mode_level;
}

void user_custom_set_nr_mode_level(uint8_t anc_level, bool isSave)
{
	user_data.nr_mode_level = anc_level;

	if(isSave)
	{
		struct nvrecord_user_t *nvrecord_user;

		nv_record_user_info_get(&nvrecord_user);
		nvrecord_user->nr_mode_level = anc_level;
		nv_record_user_info_set(nvrecord_user);
	}
}

uint8_t user_custom_get_awareness_mode_level(void)
{
	return user_data.awareness_mode_level;
}

void user_custom_set_awareness_mode_level(uint8_t anc_level, bool isSave)
{
	user_data.awareness_mode_level = anc_level;

	if(isSave)
	{
		struct nvrecord_user_t *nvrecord_user;

		nv_record_user_info_get(&nvrecord_user);
		nvrecord_user->awareness_mode_level = anc_level;
		nv_record_user_info_set(nvrecord_user);
	}
}

uint8_t app_anc_thread_get_anc_level(app_anc_mode_t mode)
{
	uint8_t anc_level = 0;
	const struct_anc_cfg **anc_list; 
	int32_t total_gain_ff_l;
	int32_t total_gain_ff_r;
	uint8_t i = 0;
	
	anc_list = anc_coef_list_50p7k; //need to check whether audio_resample is open

	switch(mode)
	{
		case APP_ANC_MODE2:
			total_gain_ff_l = anc_list[APP_ANC_MODE2 - 1]->anc_cfg_ff_l.total_gain;
			total_gain_ff_r = anc_list[APP_ANC_MODE2 - 1]->anc_cfg_ff_r.total_gain;

			//find gain in table
			for(i = 0; i < ARRAY_LEN(awareness_mode_gain_table); i++)
			{
				if((total_gain_ff_l == awareness_mode_gain_table[i]) && 
					(total_gain_ff_r == awareness_mode_gain_table[i]))
				{
					break;
				}
			}

			if(i == 1) {
				anc_level = BLE_ANC_LEVEL_MAP_MEDIUM;
			} else if(i == 0) {
				anc_level = BLE_ANC_LEVEL_MAP_LOW;
			} else {
				anc_level = BLE_ANC_LEVEL_MAP_HIGH;
			}
			
			TRACE(0, "%s mode/level: %d/0x%X", __func__, mode, anc_level);
		break;

		//Except for APP_ANC_MODE2, other modes should be called through app_anc_switch
		
		default:
			TRACE(0, "%s !!!warning: not mode2 %d ", __func__, mode);
		break;
	}

	return anc_level;
}

void app_anc_thread_update_awareness_mode_anc_level(app_anc_mode_t anc_mode, uint8_t anc_level)
{
	struct_anc_cfg **anc_list = (struct_anc_cfg **)anc_coef_list_50p7k; //need to check whether audio resample is open

	//make sure that APP_ANC_MODE2 is not const value and not be load from audsec
	if(anc_mode == APP_ANC_MODE2)
	{
		TRACE(0, "%s level: 0x%X", __func__, anc_level);
		
		switch(anc_level)
		{
			case BLE_ANC_LEVEL_MAP_MEDIUM:
				anc_list[APP_ANC_MODE2 - 1]->anc_cfg_ff_l.total_gain = awareness_mode_gain_table[1];
				anc_list[APP_ANC_MODE2 - 1]->anc_cfg_ff_r.total_gain = awareness_mode_gain_table[1];
			break;

			case BLE_ANC_LEVEL_MAP_LOW:
				anc_list[APP_ANC_MODE2 - 1]->anc_cfg_ff_l.total_gain = awareness_mode_gain_table[0];
				anc_list[APP_ANC_MODE2 - 1]->anc_cfg_ff_r.total_gain = awareness_mode_gain_table[0];
			break;
			
			case BLE_ANC_LEVEL_MAP_HIGH:
			default:
				anc_list[APP_ANC_MODE2 - 1]->anc_cfg_ff_l.total_gain = awareness_mode_gain_table[2];
				anc_list[APP_ANC_MODE2 - 1]->anc_cfg_ff_r.total_gain = awareness_mode_gain_table[2];
			break;
		}
	}
}

void app_audsec_update_nr_mode_anc_level(void *anc_list)
{
	//struct_anc_cfg **panc_list = (struct_anc_cfg **)anc_list;

	TRACE(0, "%s", __func__);

	//make sure that APP_ANC_MODE4 and APP_ANC_MODE5 is not const value and not be load from audsec
	//*panc_list[APP_ANC_MODE4 - 1] = *panc_list[APP_ANC_MODE1 - 1];
	//*panc_list[APP_ANC_MODE5 - 1] = *panc_list[APP_ANC_MODE1 - 1];

	//panc_list[APP_ANC_MODE4 - 1]->anc_cfg_ff_l.total_gain = 0;
	//panc_list[APP_ANC_MODE4 - 1]->anc_cfg_ff_r.total_gain = 0;

	//panc_list[APP_ANC_MODE5 - 1]->anc_cfg_ff_l.total_gain = 0;
	//panc_list[APP_ANC_MODE5 - 1]->anc_cfg_ff_r.total_gain = 0;
}

bool user_custom_is_VA_control_on(void)
{
	return user_data.VA_control_on;
}

void user_custom_on_off_VA_control(bool isOn, bool isSave)
{
	user_data.VA_control_on = isOn;

	if(isSave)
	{
		struct nvrecord_user_t *nvrecord_user;

		nv_record_user_info_get(&nvrecord_user);
		nvrecord_user->VA_control_on = isOn;
		nv_record_user_info_set(nvrecord_user);
	}
}

TOTA_BLE_KET_FUN_MAP user_custom_find_key_func(TOTA_BLE_LR_EARBUD_MAP lr,
			TOTA_BLE_KET_CODE_MAP key_code, TOTA_BLE_KET_EVENT_MAP key_event)
{
	TOTA_BLE_KET_FUN_MAP key_fun = BLE_KEY_FUN_MAP_INVALID;
	uint8_t i = 0;

	if(lr == BLE_LR_EARBUD_MAP_R)
	{
		switch(key_code)
		{
			case BLE_KET_CODE_MAP_TOUCH:
				for(i = 0; i < ARRAY_LEN(user_data.button_redefine.R_touch_key_cfg); i++)
				{
					if(user_data.button_redefine.R_touch_key_cfg[i].key_event == key_event) {
						key_fun = (TOTA_BLE_KET_FUN_MAP)user_data.button_redefine.R_touch_key_cfg[i].key_function;
						break;
					}
				}
				if(i == ARRAY_LEN(user_data.button_redefine.R_touch_key_cfg))
				{
					key_fun = BLE_KEY_FUN_MAP_INVALID;
					TRACE(0, "%s !!!warning, undefined key event", __func__);
				}
			break;
		
			default:
				key_fun = BLE_KEY_FUN_MAP_INVALID;
				TRACE(0, "%s !!!warning, undefined key code", __func__);
			break;
		}
	}
	else
	{
		key_fun = BLE_KEY_FUN_MAP_INVALID;
		TRACE(0, "%s !!!warning, L is undefined", __func__);
	}
	
	return key_fun;
}

int8_t user_custom_redefine_key_func(TOTA_BLE_LR_EARBUD_MAP lr, TOTA_BLE_KET_CODE_MAP key_code, 
			TOTA_BLE_KET_EVENT_MAP key_event, TOTA_BLE_KET_FUN_MAP key_fun, bool isSave)
{
	uint8_t i = 0, j = 0;

	if(lr == BLE_LR_EARBUD_MAP_R)
	{
		switch(key_code)
		{
			case BLE_KET_CODE_MAP_TOUCH:
				for(i = 0; i < ARRAY_LEN(user_data.button_redefine.R_touch_key_cfg); i++)
				{
					//if find matched key function in local user data
					if(user_data.button_redefine.R_touch_key_cfg[i].key_event == key_event) {
						struct nvrecord_user_t *nvrecord_user;

						nv_record_user_info_get(&nvrecord_user);
						for(j = 0; j < ARRAY_LEN(nvrecord_user->button_redefine.R_touch_key_cfg); j++)
						{
							//if find matched key function in nv user data
							if(nvrecord_user->button_redefine.R_touch_key_cfg[j].key_event == key_event)
							{
								//update local user data
								user_data.button_redefine.R_touch_key_cfg[i].key_function = key_fun;
								if(isSave)
								{
									//update nv user data
									nvrecord_user->button_redefine.R_touch_key_cfg[j].key_function = key_fun;
									nv_record_user_info_set(nvrecord_user);
								}
								TRACE(0, "%s R touch set OK, key event/func: %d/%d", __func__, key_event, key_fun);
								return 0;
							}
						}
						break;
					}
				}
				
				TRACE(0, "%s !!!warning, undefined key event", __func__);
				return -3;
			break;

			default:
				TRACE(0, "%s !!!warning, undefined key code", __func__);
				return -2;
			break;
		}
	}
	else
	{
		TRACE(0, "%s !!!warning, L is undefined", __func__);
		return -1;
	}

	return 0;
}

void update_earphone_color(void)
{
	uint8_t color_param;
	bool isSuccessfullyLoaded;

	isSuccessfullyLoaded = Get_EarphoneColor(&color_param);
	if(isSuccessfullyLoaded)
	{
		switch(color_param)
		{
			case BLE_COLOR_MAP_BLACK:
			case BLE_COLOR_MAP_WHITE:
			case BLE_COLOR_MAP_BLUE:
			case BLE_COLOR_MAP_RED:
			case BLE_COLOR_MAP_GREEN:
			case BLE_COLOR_MAP_PURPLE:
			case BLE_COLOR_MAP_COLOR7:
				user_data.earphone_color = (TOTA_BLE_COLOR_MAP)color_param;
			break;

			default:
				user_data.earphone_color = BLE_COLOR_MAP_DEFAULT;
			break;
		}
	} else
	{
		user_data.earphone_color = BLE_COLOR_MAP_DEFAULT;
	}

	TRACE(0, "%s earphone color: 0x%02x", __func__, user_data.earphone_color);
}

TOTA_BLE_COLOR_MAP user_custom_get_earphone_color(void)
{
	return user_data.earphone_color;
}

void update_earphone_sn(void)
{
	char sn[CUSTOM_PARAM_SERIAL_NUM_LEN] = {0};
	uint32_t snLen;
	bool isSuccessfullyLoaded;

	isSuccessfullyLoaded = Get_EarphoneSN(sn, &snLen);
	if(isSuccessfullyLoaded)
	{
		if(snLen > CUSTOM_PARAM_SERIAL_NUM_LEN) snLen = CUSTOM_PARAM_SERIAL_NUM_LEN;
		memset(user_data.sn, 0, sizeof(user_data.sn));
		memcpy(user_data.sn, sn, snLen);
		
	} else
	{
		//default sn
		memset(user_data.sn, 0, sizeof(user_data.sn));
		memcpy(user_data.sn, "0123456789ABCDEFGH", CUSTOM_PARAM_SERIAL_NUM_LEN);
	}

	TRACE(0, "%s earphone sn: %s", __func__, user_data.sn);
}

const char *user_custom_get_sn(void)
{
	return user_data.sn;
}

void user_custom_restore_default_settings(bool promt_on)
{
	struct nvrecord_user_t *nvrecord_user;
	bool is_BT_connected = false;
	uint8_t i = 0;
	
	nv_record_user_info_get(&nvrecord_user);
	
	//default touch config
	nvrecord_user->touch_lock = false;
	
	//default prompt volume config
	nvrecord_user->prompt_vol_en = true;
	nvrecord_user->prompt_vol_level = MEDIA_VOLUME_LEVEL_WARNINGTONE;

	//default LR balance config
	nvrecord_user->LR_balance_val = 50;
	
	//default user BT name config
	memset(nvrecord_user->redefine_BT_name, 0, sizeof(nvrecord_user->redefine_BT_name));

	//default eq config
	nvrecord_user->eq_mode = BLE_EQ_MAP_STUDIO;
	USER_IIR_CFG_T user_eq  = {
	    .gain0 = 0,
	    .gain1 = 0,
	    .num = USER_EQ_BANDS,
	    .param = {
	        {IIR_TYPE_PEAK,       0,   32.0,   0.7},
	        {IIR_TYPE_PEAK,       0,   64.0,   0.7},
	        {IIR_TYPE_PEAK,       0,  125.0,   0.7},
	        {IIR_TYPE_PEAK,       0,  250.0,   0.7},
	        {IIR_TYPE_PEAK,       0,  500.0,   0.7},
	        {IIR_TYPE_PEAK,       0, 1000.0,   0.7},
	        {IIR_TYPE_PEAK,       0, 2000.0,   0.7},
	        {IIR_TYPE_PEAK,       0, 4000.0,   0.7},
	        {IIR_TYPE_PEAK,       0, 8000.0,   0.7},
	        {IIR_TYPE_PEAK,       0,16000.0,   0.7},
	    }
	};
	nvrecord_user->user_eq = user_eq;
	
	//default sidetone config
	nvrecord_user->sidetone_on = false;

	//default shutdown time config
	nvrecord_user->shutdown_time = BLE_SHUTDOWN_TIME_MAP_NEVER_SHUTDOWN;

	//default ANC level config
	nvrecord_user->nr_mode_level = BLE_ANC_LEVEL_MAP_HIGH;
	nvrecord_user->awareness_mode_level = BLE_ANC_LEVEL_MAP_HIGH;

	//default VA control config
	nvrecord_user->VA_control_on = true;

	//default button redefine config
	KEY_CFG_T *nv_key_cfg = NULL;
	nv_key_cfg = nvrecord_user->button_redefine.R_touch_key_cfg;
	//R_touch_key_cfg is a array with 12 members
	KEY_CFG_T R_touch_key_cfg[12]  = {
		{BLE_KET_EVENT_MAP_CLICK,       BLE_KEY_FUN_MAP_NONE},
		{BLE_KET_EVENT_MAP_DOUBLE,      BLE_KEY_FUN_MAP_PLAYPAUSE},
		{BLE_KEY_EVENT_MAP_TRIPLE,      BLE_KEY_FUN_MAP_NONE},
		{BLE_KEY_EVENT_MAP_LONG,        BLE_KEY_FUN_MAP_NONE},
		{BLE_KEY_EVENT_MAP_SWIPE_UP,    BLE_KEY_FUN_MAP_VOL_UP},
		{BLE_KEY_EVENT_MAP_SWIPE_DOWN,  BLE_KEY_FUN_MAP_VOL_DOWN},
		{BLE_KEY_EVENT_MAP_SWIPE_LEFT,  BLE_KEY_FUN_MAP_PRE_SONG},
		{BLE_KEY_EVENT_MAP_SWIPE_RIGHT, BLE_KEY_FUN_MAP_NEXT_SONG},
		//reserved
		{BLE_KET_EVENT_MAP_INVALID,     BLE_KEY_FUN_MAP_INVALID},
		{BLE_KET_EVENT_MAP_INVALID,     BLE_KEY_FUN_MAP_INVALID},
		{BLE_KET_EVENT_MAP_INVALID,     BLE_KEY_FUN_MAP_INVALID},
		{BLE_KET_EVENT_MAP_INVALID,     BLE_KEY_FUN_MAP_INVALID},
	};
	for(i = 0; i < ARRAY_LEN(nvrecord_user->button_redefine.R_touch_key_cfg); i++)
	{
		nv_key_cfg[i] = R_touch_key_cfg[i];
	}

	//default shutdown timer repeat switch config
	nvrecord_user->is_shutdown_timer_need_repeat = false;

	nv_record_user_info_set(nvrecord_user);
	
	//update local user infor
	user_data.touch_lock = nvrecord_user->touch_lock;
	user_data.prompt_vol_en = nvrecord_user->prompt_vol_en;
	user_data.prompt_vol_level = nvrecord_user->prompt_vol_level;
	user_data.LR_balance_val = nvrecord_user->LR_balance_val;
	memset(user_data.redefine_BT_name, 0, sizeof(user_data.redefine_BT_name));
	memcpy(user_data.redefine_BT_name, nvrecord_user->redefine_BT_name, strlen(nvrecord_user->redefine_BT_name));
	user_data.eq_mode = nvrecord_user->eq_mode;
	user_data.user_eq = nvrecord_user->user_eq;
	update_user_EQ(user_data.user_eq);
	user_data.sidetone_on = nvrecord_user->sidetone_on;
	user_data.shutdown_time = nvrecord_user->shutdown_time;
	user_data.nr_mode_level = nvrecord_user->nr_mode_level;
	user_data.awareness_mode_level = nvrecord_user->awareness_mode_level;
	user_data.VA_control_on = nvrecord_user->VA_control_on;
	//R_touch_key_cfg is a array with 12 members
	KEY_CFG_T *local_key_cfg = NULL;
	local_key_cfg = user_data.button_redefine.R_touch_key_cfg;
	nv_key_cfg = nvrecord_user->button_redefine.R_touch_key_cfg;
	for(i = 0; i < ARRAY_LEN(user_data.button_redefine.R_touch_key_cfg); i++)
	{
		local_key_cfg[i] = nv_key_cfg[i];
	}
	user_data.is_shutdown_timer_need_repeat = nvrecord_user->is_shutdown_timer_need_repeat;
	
	//update function status via local user infor
	app_reset_anc_switch();
	app_ble_eq_set();
	enter_exit_low_latency_mode(false, true);
	ble_sidetone_switch(user_data.sidetone_on);
	is_BT_connected = app_bt_get_connected_device_num()? true : false;
	update_power_savingmode_shutdown_timer(user_data.shutdown_time, is_BT_connected);

	if(promt_on) media_PlayAudio(AUD_ID_BT_FACTORY_RESET, 0);
}

void nvrecord_user_info_init_for_ota(struct nvrecord_user_t *pUserInfo)
{
	uint8_t saved_user_info_ver[13];
	uint8_t	i = 0;
	
	USER_IIR_CFG_T user_eq  = {
	    .gain0 = 0,
	    .gain1 = 0,
	    .num = USER_EQ_BANDS,
	    .param = {
	        {IIR_TYPE_PEAK,       0,   32.0,   0.7},
	        {IIR_TYPE_PEAK,       0,   64.0,   0.7},
	        {IIR_TYPE_PEAK,       0,  125.0,   0.7},
	        {IIR_TYPE_PEAK,       0,  250.0,   0.7},
	        {IIR_TYPE_PEAK,       0,  500.0,   0.7},
	        {IIR_TYPE_PEAK,       0, 1000.0,   0.7},
	        {IIR_TYPE_PEAK,       0, 2000.0,   0.7},
	        {IIR_TYPE_PEAK,       0, 4000.0,   0.7},
	        {IIR_TYPE_PEAK,       0, 8000.0,   0.7},
	        {IIR_TYPE_PEAK,       0,16000.0,   0.7},
	    }
	};
		
	KEY_CFG_T *nv_key_cfg = NULL;
	//R_touch_key_cfg is a array with 12 members
	KEY_CFG_T R_touch_key_cfg[12]  = {
		{BLE_KET_EVENT_MAP_CLICK,       BLE_KEY_FUN_MAP_NONE},
		{BLE_KET_EVENT_MAP_DOUBLE,      BLE_KEY_FUN_MAP_PLAYPAUSE},
		{BLE_KEY_EVENT_MAP_TRIPLE,      BLE_KEY_FUN_MAP_NONE},
		{BLE_KEY_EVENT_MAP_LONG,        BLE_KEY_FUN_MAP_NONE},
		{BLE_KEY_EVENT_MAP_SWIPE_UP,    BLE_KEY_FUN_MAP_VOL_UP},
		{BLE_KEY_EVENT_MAP_SWIPE_DOWN,  BLE_KEY_FUN_MAP_VOL_DOWN},
		{BLE_KEY_EVENT_MAP_SWIPE_LEFT,  BLE_KEY_FUN_MAP_PRE_SONG},
		{BLE_KEY_EVENT_MAP_SWIPE_RIGHT, BLE_KEY_FUN_MAP_NEXT_SONG},
		//reserved
		{BLE_KET_EVENT_MAP_INVALID,     BLE_KEY_FUN_MAP_INVALID},
		{BLE_KET_EVENT_MAP_INVALID,     BLE_KEY_FUN_MAP_INVALID},
		{BLE_KET_EVENT_MAP_INVALID,     BLE_KEY_FUN_MAP_INVALID},
		{BLE_KET_EVENT_MAP_INVALID,     BLE_KEY_FUN_MAP_INVALID},
	};
			
	TRACE(0, "*** [%s] enter", __func__);

	snprintf((char *)saved_user_info_ver, sizeof(saved_user_info_ver), "V%d.%d.%d",
			pUserInfo->nvrecord_user_ver[2], pUserInfo->nvrecord_user_ver[1], pUserInfo->nvrecord_user_ver[0]);

	//init user info according to version in flash
	//BT name and eq are added in V0.0.1, so just update them when saved user infor ver is V0.0.1
	if(strncmp((const char *)saved_user_info_ver, "V0.0.1", strlen("V0.0.1")) == 0)
	{
		//default user BT name config
		memset(pUserInfo->redefine_BT_name, 0, sizeof(pUserInfo->redefine_BT_name));

		//default eq config
		pUserInfo->eq_mode = BLE_EQ_MAP_STUDIO;
		pUserInfo->user_eq = user_eq;

		//default sidetone config
		pUserInfo->sidetone_on = false;

		//default shutdown time config
		pUserInfo->shutdown_time = BLE_SHUTDOWN_TIME_MAP_NEVER_SHUTDOWN;

		//default ANC level config
		pUserInfo->nr_mode_level = BLE_ANC_LEVEL_MAP_HIGH;
		pUserInfo->awareness_mode_level = BLE_ANC_LEVEL_MAP_HIGH;

		//default VA control config
		pUserInfo->VA_control_on = true;

		//default button redefine config
		//R_touch_key_cfg is a array with 12 members
		nv_key_cfg = pUserInfo->button_redefine.R_touch_key_cfg;
		for(i = 0; i < ARRAY_LEN(pUserInfo->button_redefine.R_touch_key_cfg); i++)
		{
			nv_key_cfg[i] = R_touch_key_cfg[i];
		}

		//default shutdown timer repeat switch config
		pUserInfo->is_shutdown_timer_need_repeat = false;
	}
	else if(strncmp((const char *)saved_user_info_ver, "V0.0.2", strlen("V0.0.2")) == 0)
	{
		//default sidetone config
		pUserInfo->sidetone_on = false;

		//default shutdown time config
		pUserInfo->shutdown_time = BLE_SHUTDOWN_TIME_MAP_NEVER_SHUTDOWN;

		//default ANC level config
		pUserInfo->nr_mode_level = BLE_ANC_LEVEL_MAP_HIGH;
		pUserInfo->awareness_mode_level = BLE_ANC_LEVEL_MAP_HIGH;

		//default VA control config
		pUserInfo->VA_control_on = true;

		//default button redefine config
		//R_touch_key_cfg is a array with 12 members
		nv_key_cfg = pUserInfo->button_redefine.R_touch_key_cfg;
		for(i = 0; i < ARRAY_LEN(pUserInfo->button_redefine.R_touch_key_cfg); i++)
		{
			nv_key_cfg[i] = R_touch_key_cfg[i];
		}

		//default shutdown timer repeat switch config
		pUserInfo->is_shutdown_timer_need_repeat = false;
	}
	else if(strncmp((const char *)saved_user_info_ver, "V0.0.3", strlen("V0.0.3")) == 0)
	{
		//default button redefine config
		//R_touch_key_cfg is a array with 12 members
		nv_key_cfg = pUserInfo->button_redefine.R_touch_key_cfg;
		for(i = 0; i < ARRAY_LEN(pUserInfo->button_redefine.R_touch_key_cfg); i++)
		{
			nv_key_cfg[i] = R_touch_key_cfg[i];
		}

		//default shutdown timer repeat switch config
		pUserInfo->is_shutdown_timer_need_repeat = false;
	}
	else if(strncmp((const char *)saved_user_info_ver, "V0.0.4", strlen("V0.0.4")) == 0)
	{
		//default shutdown timer repeat switch config
		pUserInfo->is_shutdown_timer_need_repeat = false;
	}
	//if saved user infor ver is V0.0.0 or other, should init all user infor
	else
	{
		//default touch config
		pUserInfo->touch_lock = false;
	
		//default prompt volume config
		pUserInfo->prompt_vol_en = true;
		pUserInfo->prompt_vol_level = MEDIA_VOLUME_LEVEL_WARNINGTONE;

		//default LR balance config
		pUserInfo->LR_balance_val = 50;

		//default user BT name config
		memset(pUserInfo->redefine_BT_name, 0, sizeof(pUserInfo->redefine_BT_name));

		//default eq config
		pUserInfo->eq_mode = BLE_EQ_MAP_STUDIO;
		pUserInfo->user_eq = user_eq;

		//default sidetone config
		pUserInfo->sidetone_on = false;

		//default shutdown time config
		pUserInfo->shutdown_time = BLE_SHUTDOWN_TIME_MAP_NEVER_SHUTDOWN;

		//default ANC level config
		pUserInfo->nr_mode_level = BLE_ANC_LEVEL_MAP_HIGH;
		pUserInfo->awareness_mode_level = BLE_ANC_LEVEL_MAP_HIGH;

		//default VA control config
		pUserInfo->VA_control_on = true;

		//default button redefine config
		//R_touch_key_cfg is a array with 12 members
		nv_key_cfg = pUserInfo->button_redefine.R_touch_key_cfg;
		for(i = 0; i < ARRAY_LEN(pUserInfo->button_redefine.R_touch_key_cfg); i++)
		{
			nv_key_cfg[i] = R_touch_key_cfg[i];
		}

		//default shutdown timer repeat switch config
		pUserInfo->is_shutdown_timer_need_repeat = false;
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
	uint8_t i = 0;

	TRACE(0, "*** [%s] total/used bytes: %d/%d", __func__, NV_SDK_RESERVED_LEN, sizeof(NV_SDK_INFO_T));

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

	user_data.LR_balance_val = nvrecord_user->LR_balance_val;
	TRACE(0, "*** [%s] LR balance: %d", __func__, user_data.LR_balance_val);

	memset(user_data.redefine_BT_name, 0, sizeof(user_data.redefine_BT_name));
	memcpy(user_data.redefine_BT_name, nvrecord_user->redefine_BT_name, strlen(nvrecord_user->redefine_BT_name));
	TRACE(0, "*** [%s] user BT name: %s", __func__, user_data.redefine_BT_name);

	user_data.eq_mode = nvrecord_user->eq_mode;
	TRACE(0, "*** [%s] EQ mode: %d", __func__, user_data.eq_mode);

	user_data.user_eq = nvrecord_user->user_eq;
	TRACE(0, "*** [%s] gain0/gain1: %d/%d(/100)", __func__, (int32_t)(user_data.user_eq.gain0 * 100), (int32_t)(user_data.user_eq.gain1 * 100));
	TRACE(0, "*** [%s] num: %d", __func__, user_data.user_eq.num);
	for(i = 0; i < USER_EQ_BANDS; i++)
	{
		TRACE(0, "*** [%s] type: %d fc/gain/Q: %d/%d/%d(/100)", __func__, user_data.user_eq.param[i].type, (int32_t)(user_data.user_eq.param[i].fc * 100),
			(int32_t)(user_data.user_eq.param[i].gain * 100), (int32_t)(user_data.user_eq.param[i].Q * 100));
	}
	update_user_EQ(user_data.user_eq);

	user_data.sidetone_on = nvrecord_user->sidetone_on;
	TRACE(0, "*** [%s] sidetone on: %d", __func__, user_data.sidetone_on);
	update_sidetone_on_status(user_data.sidetone_on);

	user_data.shutdown_time = nvrecord_user->shutdown_time;
	TRACE(0, "*** [%s] shutdown time: 0x%X", __func__, user_data.shutdown_time);
	update_power_savingmode_shutdown_timer(user_data.shutdown_time, false); //don't start shutdown timer when system is in init status

	user_data.nr_mode_level = nvrecord_user->nr_mode_level;
	TRACE(0, "*** [%s] nr mode level: 0x%X", __func__, user_data.nr_mode_level);
	user_data.awareness_mode_level = nvrecord_user->awareness_mode_level;
	TRACE(0, "*** [%s] awareness mode level: 0x%X", __func__, user_data.awareness_mode_level);

	user_data.VA_control_on = nvrecord_user->VA_control_on;
	TRACE(0, "*** [%s] VA control on: %d", __func__, user_data.VA_control_on);

	KEY_CFG_T *nv_key_cfg = NULL;
	KEY_CFG_T *local_key_cfg = NULL;
	nv_key_cfg = nvrecord_user->button_redefine.R_touch_key_cfg;
	local_key_cfg = user_data.button_redefine.R_touch_key_cfg;
	//R_touch_key_cfg is a array with 12 members
	for(i = 0; i < ARRAY_LEN(user_data.button_redefine.R_touch_key_cfg); i++)
	{
		local_key_cfg[i] = nv_key_cfg[i];
		TRACE(0, "*** [%s] R touch-->event/func: %d/%d", __func__, local_key_cfg[i].key_event, local_key_cfg[i].key_function);
	}

	update_earphone_color();
	TRACE(0, "*** [%s] earphone color: 0x%02x", __func__, user_data.earphone_color);

	update_earphone_sn();
	TRACE(0, "*** [%s] earphone sn: %s", __func__, user_data.sn);

	//default shutdown timer repeat switch config
	user_data.is_shutdown_timer_need_repeat = nvrecord_user->is_shutdown_timer_need_repeat;
	TRACE(0, "*** [%s] is shutdown timer need repeat: %d", __func__, user_data.is_shutdown_timer_need_repeat);
}

void user_custom_nvrecord_rebuild_user_info(uint8_t *pUserInfo, bool isRebuildAll)
{
	struct nvrecord_user_t *user_info = (struct nvrecord_user_t *)pUserInfo;
	uint8_t i = 0;
	
	//default touch config
	user_info->touch_lock = false;
	
	//default prompt volume config
	user_info->prompt_vol_en = true;
	user_info->prompt_vol_level = MEDIA_VOLUME_LEVEL_WARNINGTONE;

	//default LR balance config
	user_info->LR_balance_val = 50;
	
	//default user BT name config
	memset(user_info->redefine_BT_name, 0, sizeof(user_info->redefine_BT_name));

	//default eq config
	user_info->eq_mode = BLE_EQ_MAP_STUDIO;
	USER_IIR_CFG_T user_eq  = {
	    .gain0 = 0,
	    .gain1 = 0,
	    .num = USER_EQ_BANDS,
	    .param = {
	        {IIR_TYPE_PEAK,       0,   32.0,   0.7},
	        {IIR_TYPE_PEAK,       0,   64.0,   0.7},
	        {IIR_TYPE_PEAK,       0,  125.0,   0.7},
	        {IIR_TYPE_PEAK,       0,  250.0,   0.7},
	        {IIR_TYPE_PEAK,       0,  500.0,   0.7},
	        {IIR_TYPE_PEAK,       0, 1000.0,   0.7},
	        {IIR_TYPE_PEAK,       0, 2000.0,   0.7},
	        {IIR_TYPE_PEAK,       0, 4000.0,   0.7},
	        {IIR_TYPE_PEAK,       0, 8000.0,   0.7},
	        {IIR_TYPE_PEAK,       0,16000.0,   0.7},
	    }
	};
	user_info->user_eq = user_eq;

	//default sidetone config
	user_info->sidetone_on = false;

	//default shutdown time config
	user_info->shutdown_time = BLE_SHUTDOWN_TIME_MAP_NEVER_SHUTDOWN;

	//default ANC level config
	user_info->nr_mode_level = BLE_ANC_LEVEL_MAP_HIGH;
	user_info->awareness_mode_level = BLE_ANC_LEVEL_MAP_HIGH;

	//default VA control config
	user_info->VA_control_on = true;

	//default button redefine config
	KEY_CFG_T *nv_key_cfg = NULL;
	nv_key_cfg = user_info->button_redefine.R_touch_key_cfg;
	//R_touch_key_cfg is a array with 12 members
	KEY_CFG_T R_touch_key_cfg[12]  = {
		{BLE_KET_EVENT_MAP_CLICK,       BLE_KEY_FUN_MAP_NONE},
		{BLE_KET_EVENT_MAP_DOUBLE,      BLE_KEY_FUN_MAP_PLAYPAUSE},
		{BLE_KEY_EVENT_MAP_TRIPLE,      BLE_KEY_FUN_MAP_NONE},
		{BLE_KEY_EVENT_MAP_LONG,        BLE_KEY_FUN_MAP_NONE},
		{BLE_KEY_EVENT_MAP_SWIPE_UP,    BLE_KEY_FUN_MAP_VOL_UP},
		{BLE_KEY_EVENT_MAP_SWIPE_DOWN,  BLE_KEY_FUN_MAP_VOL_DOWN},
		{BLE_KEY_EVENT_MAP_SWIPE_LEFT,  BLE_KEY_FUN_MAP_PRE_SONG},
		{BLE_KEY_EVENT_MAP_SWIPE_RIGHT, BLE_KEY_FUN_MAP_NEXT_SONG},
		//reserved
		{BLE_KET_EVENT_MAP_INVALID,     BLE_KEY_FUN_MAP_INVALID},
		{BLE_KET_EVENT_MAP_INVALID,     BLE_KEY_FUN_MAP_INVALID},
		{BLE_KET_EVENT_MAP_INVALID,     BLE_KEY_FUN_MAP_INVALID},
		{BLE_KET_EVENT_MAP_INVALID,     BLE_KEY_FUN_MAP_INVALID},
	};
	for(i = 0; i < ARRAY_LEN(user_info->button_redefine.R_touch_key_cfg); i++)
	{
		nv_key_cfg[i] = R_touch_key_cfg[i];
	}

	//default shutdown timer repeat switch config
	user_info->is_shutdown_timer_need_repeat = false;
	
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
		//update local user infor
		user_data.touch_lock = user_info->touch_lock;
		user_data.prompt_vol_en = user_info->prompt_vol_en;
		user_data.prompt_vol_level = user_info->prompt_vol_level;
		user_data.LR_balance_val = user_info->LR_balance_val;
		memset(user_data.redefine_BT_name, 0, sizeof(user_data.redefine_BT_name));
		memcpy(user_data.redefine_BT_name, user_info->redefine_BT_name, strlen(user_info->redefine_BT_name));
		user_data.eq_mode = user_info->eq_mode;
		user_data.user_eq = user_info->user_eq;
		update_user_EQ(user_data.user_eq);
		user_data.sidetone_on = user_info->sidetone_on;
		user_data.shutdown_time = user_info->shutdown_time;
		user_data.nr_mode_level = user_info->nr_mode_level;
		user_data.awareness_mode_level = user_info->awareness_mode_level;
		user_data.VA_control_on = user_info->VA_control_on;
		//R_touch_key_cfg is a array with 12 members
		KEY_CFG_T *local_key_cfg = NULL;
		local_key_cfg = user_data.button_redefine.R_touch_key_cfg;
		nv_key_cfg = user_info->button_redefine.R_touch_key_cfg;
		for(i = 0; i < ARRAY_LEN(user_data.button_redefine.R_touch_key_cfg); i++)
		{
			local_key_cfg[i] = nv_key_cfg[i];
		}
		user_data.is_shutdown_timer_need_repeat = user_info->is_shutdown_timer_need_repeat;
	}
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
				//hal_set_cst820_rst_low(); //TODO: when new PCBA done, delete it
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

				app_disconnect_all_bt_connections(false);
				app_ibrt_if_event_entry(APP_UI_EV_DOCK);
	       		app_ibrt_if_event_entry(APP_UI_EV_CASE_CLOSE);

#ifdef CMT_008_AC107_ADC
				app_audio_linein_key_init();

				ac107_hw_open();
				ac107_i2c_init();
#endif				
				app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);
				app_stop_10_second_timer(APP_POWER_SAVINGMODE_SHUTDOWN_TIMER_ID);
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
				if(!bt_media_is_media_active_by_type(BT_STREAM_MUSIC | BT_STREAM_MEDIA | BT_STREAM_VOICE) && !app_is_prompt_on_playing())
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
			//hal_set_cst820_rst_high(); //TODO: when new PCBA done, delete it
#endif
			app_key_init();
			
#ifdef AUDIO_LINEIN
			//app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP, BT_STREAM_LINEIN, BT_DEVICE_ID_1, 0); //lewis: use this will cause some pop noise, I don't know why
			app_play_linein_onoff(false);
#endif

#ifdef CMT_008_AC107_ADC
			ac107_hw_close();
#endif

			//app_ibrt_if_event_entry(APP_UI_EV_CASE_OPEN);
        	//app_ibrt_if_event_entry(APP_UI_EV_UNDOCK);
			app_bt_profile_connect_manager_opening_reconnect();

			jack_irq_update();

			app_start_10_second_timer(APP_POWEROFF_TIMER_ID);
			app_stop_10_second_timer(APP_POWER_SAVINGMODE_SHUTDOWN_TIMER_ID);
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
     //!!!very important, can't use APP_SYSFREQ_USER_APP_12 here, otherwise will cause af_thread warning when set APP_SYSFREQ_USER_APP_12 to 32K during a call 
     if(level == 0 || level == 100)
     {
        app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_32K);
     }
     else if(level >= 1)
     {
        app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_26M);
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
     //!!!very important, can't use APP_SYSFREQ_USER_APP_12 here, otherwise will cause af_thread warning when set APP_SYSFREQ_USER_APP_12 to 32K during a call 
     if(ratio == 0 || ratio == 100)
     {
        app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_32K);
     }
     else if(ratio >= 1)
     {
        app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_26M);
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
