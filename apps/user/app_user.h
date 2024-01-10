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

#ifndef __APP_USER_H__
#define __APP_USER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "hal_iomux.h"
#include "../../platform/hal/best1502x/hal_cmu_best1502x.h"
#include "../../utils/hwtimer_list/hwtimer_list.h"

typedef struct {
	uint8_t nvrecord_user_ver_H;
	uint8_t nvrecord_user_ver_M;
	uint8_t nvrecord_user_ver_L;

	bool touch_lock;
	
	uint8_t prompt_vol_en:1;
	uint8_t prompt_vol_level:7;
	
	uint8_t quick_conversation_mode;
} app_user_custom_data_t;

//Record user info's history
#define NV_USER_VERSION_H       0
#define NV_USER_VERSION_M       0
#define NV_USER_VERSION_L       1

typedef enum {
#ifdef CMT_008_3_5JACK_CTR
    USER_EVENT_3_5JACK,
#endif
    USER_EVENT_NONE
}USER_EVENT;

typedef struct {
#ifdef CMT_008_3_5JACK_CTR
	uint32_t jack_pluginout_debounce_ctx;
	uint32_t jack_pluginout_debounce_cnt;
	bool is3_5JackInplug;
#endif
}USER_MODULE_DATA_T;

#ifdef CMT_008_3_5JACK_CTR
typedef enum {
	APP_JACK_STATUS_PLUGOUT,
	APP_JACK_STATUS_PLUGIN,
	APP_JACK_STATUS_INVALID
}APP_JACK_STATUS_T;
#endif

#ifdef CMT_008_EN_LED_BREATH
typedef struct {
	enum HAL_PWM_ID_T pwm_id;
	struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pwm;
}HAL_PIN_PWM_CFG;

#define CFG_HW_PWM_NUM     (2)
#define PWM_FREQ           (500)

typedef enum {
    APP_BREATH_ID_0 = 0,
    APP_BREATH_ID_1,
	APP_BREATH_ID_QTY,
}APP_BREATH_ID_T;

typedef struct {
	uint16_t high_time; //fade in, ms
	uint16_t on_state_time; //stable on, ms
	uint16_t low_time; //fade out, ms
	uint16_t off_state_time; //off, ms
}APP_BREATH_CFG_T;

typedef enum {
	APP_BREATH_STAGE_HIGH,
	APP_BREATH_STAGE_ON,
	APP_BREATH_STAGE_LOW,
	APP_BREATH_STAGE_OFF,
	APP_BREATH_STAGE_INVALID
}APP_BREATH_STAGE_T;	

typedef struct {
    APP_BREATH_ID_T id;
    APP_BREATH_CFG_T config;
	uint16_t high_time_step; //at least = 1ms
	uint16_t low_time_step; //at least = 1ms
	uint8_t cur_ratio; //PWM duty ratio at present , which is the LED brightness
	APP_BREATH_STAGE_T cur_stage;
	HWTIMER_ID timer; //hardware timer
}APP_BREATH_T;

#endif

#ifdef CMT_008_UART_USBAUDIO_SWITCH
typedef enum {
	UARTUSB_SWITCH_UART,
	UARTUSB_SWITCH_USB,
	UARTUSB_SWITCH_INVALID
}UARTUSB_SWITCH;
#endif

#ifdef CMT_008_CHARGE_CURRENT
typedef enum {
	CHARGING_SPEED_LOW,
	CHARGING_SPEED_HIGH,
	CHARGING_SPEED_INVALID
}CHARGING_SPEED;
#endif

bool user_custom_is_touch_locked(void);
void user_custom_lock_unlock_touch(bool isEn, bool isSave);
bool user_custom_is_prompt_en(void);
void user_custom_en_dis_prompt(bool isEn, bool isSave);
uint8_t user_custom_get_prompt_volume_level(void);
void user_custom_set_prompt_volume_level(uint8_t vol, bool isSave);
void user_custom_nvrecord_rebuild_user_info(uint8_t *pUserInfo, bool isRebuildAll);
void user_custom_nvrecord_user_info_get(void);

#ifdef CMT_008_3_5JACK_CTR
APP_JACK_STATUS_T app_3_5jack_status_get(void);
bool app_is_3_5jack_inplug(void);
#endif
#ifdef CMT_008_EN_LED_BREATH
int app_breath_led_setup(APP_BREATH_ID_T id, APP_BREATH_CFG_T *cfg);
int app_breath_led_start(APP_BREATH_ID_T id);
int app_breath_led_stop(APP_BREATH_ID_T id);
uint8_t app_breath_led_init(void);
#endif
void app_user_event_open_module(void);
void app_user_event_close_module(void);
void en_dis_charging(bool isEn);
#ifdef CMT_008_UART_USBAUDIO_SWITCH
void uart_usbaudio_start_switch_to(UARTUSB_SWITCH switcher);
#endif
#ifdef CMT_008_CHARGE_CURRENT
void charge_current_switch_to(CHARGING_SPEED speed);
#endif


#ifdef __cplusplus
}
#endif

#endif
