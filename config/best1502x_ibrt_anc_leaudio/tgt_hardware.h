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
#ifndef __TGT_HARDWARE__
#define __TGT_HARDWARE__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_key.h"
#include "hal_aud.h"

//config hwardware codec iir.
#define EQ_HW_DAC_IIR_LIST_NUM              2
#define EQ_HW_ADC_IIR_LIST_NUM              1
#define EQ_HW_IIR_LIST_NUM                  1
#define EQ_SW_IIR_LIST_NUM                  1
#define EQ_HW_FIR_LIST_NUM                  3

#ifdef __TENCENT_VOICE__
extern const char *BT_FIRMWARE_VERSION;
#endif

//pwl

#ifdef __BT_DEBUG_TPORTS__
#define CFG_HW_PWL_NUM (0)
#else
#ifdef __APP_USE_LED_INDICATE_IBRT_STATUS__
#define CFG_HW_PWL_NUM (0)
#else
#define CFG_HW_PWL_NUM (0)
#endif
#endif

extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pinmux_pwl[CFG_HW_PWL_NUM];
#ifdef __APP_USE_LED_INDICATE_IBRT_STATUS__
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_ibrt_indication_pinmux_pwl[3];
#endif

#ifdef __KNOWLES
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_pinmux_uart[2];
#endif

//adckey define
#define CFG_HW_ADCKEY_NUMBER 0
#define CFG_HW_ADCKEY_BASE 0
#define CFG_HW_ADCKEY_ADC_MAXVOLT 1000
#define CFG_HW_ADCKEY_ADC_MINVOLT 0
#define CFG_HW_ADCKEY_ADC_KEYVOLT_BASE 130
extern const uint16_t CFG_HW_ADCKEY_MAP_TABLE[CFG_HW_ADCKEY_NUMBER];

#define BTA_AV_CO_SBC_MAX_BITPOOL  52

#ifdef __BT_DEBUG_TPORTS__
#ifdef TPORTS_KEY_COEXIST
#define CFG_HW_GPIOKEY_NUM (3)
#else
#define CFG_HW_GPIOKEY_NUM (0)
#endif
#else
//gpiokey define
#ifdef IS_MULTI_AI_ENABLED
#define CFG_HW_GPIOKEY_NUM (3)
#elif BES_AUDIO_DEV_Main_Board_9v0
#define CFG_HW_GPIOKEY_NUM (6)
#else
#define CFG_HW_GPIOKEY_NUM (3)
#endif
#endif

extern const struct HAL_KEY_GPIOKEY_CFG_T cfg_hw_gpio_key_cfg[CFG_HW_GPIOKEY_NUM];

// DC CALIB define
#define DAC_DC_CALIB_MIC_CHAN_MAP           (AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2)

// Notes: the real CFG_ADC_DC_CALIB_CH_NUM need be equal to the real calibrate mic number!!!
#define CFG_ADC_DC_CALIB_MIC_DEV                (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2 | AUD_CHANNEL_MAP_CH3 | AUD_CHANNEL_MAP_CH4)
#define CFG_ADC_DC_CALIB_CH_NUM                 (AUD_CHANNEL_NUM_5)

// ANC function key
#define ANC_FUNCTION_KEY                    HAL_KEY_CODE_PWR

// ANC coefficient curve number
#define ANC_COEF_NUM                        (1)

#define PSAP_COEF_LIST_NUM                  (1)

#define HWLIMITER_PARA_LIST_NUM             (1)
//#define ANC_TALK_THROUGH

#ifdef ANC_TALK_THROUGH
#define ANC_COEF_LIST_NUM                   (ANC_COEF_NUM + 1)
#else
#define ANC_COEF_LIST_NUM                   (ANC_COEF_NUM)
#endif

#define ANC_FF_MIC_CH_L                     AUD_CHANNEL_MAP_CH0
#define ANC_FB_MIC_CH_L                     AUD_CHANNEL_MAP_CH2
#if defined(FREEMAN_ENABLED_STERO)
#define ANC_FF_MIC_CH_R                     AUD_CHANNEL_MAP_CH1
#define ANC_FB_MIC_CH_R                     AUD_CHANNEL_MAP_CH4
#else
#define ANC_FF_MIC_CH_R                     (0)
#define ANC_FB_MIC_CH_R                     (0)
#endif


/**
 * NOTE:
 *  1. TT can work with FF, which means two FF channels.
 *  2. PSAP use TT channel, which means PSAP can not work with TT
 **/
#define ANC_TT_MIC_CH_L                     AUD_CHANNEL_MAP_CH0
#if defined(FREEMAN_ENABLED_STERO)
#define ANC_TT_MIC_CH_R                     AUD_CHANNEL_MAP_CH1  //5 ADC, need reuse, same with ff r
#else
#define ANC_TT_MIC_CH_R                     (0)  //5 ADC, need reuse, same with ff r
#endif
#define ANC_TALK_MIC_CH_L                   AUD_CHANNEL_MAP_CH1
#define ANC_TALK_MIC_CH_R                   0

#define ANC_REF_MIC_CH_L                    AUD_CHANNEL_MAP_ECMIC_CH0
#define ANC_REF_MIC_CH_R                    0

#define ANC_VMIC_CFG                        (AUD_VMIC_MAP_VMIC2)

// audio codec
#define CFG_HW_AUD_INPUT_PATH_NUM           (6)
extern const struct AUD_IO_PATH_CFG_T cfg_audio_input_path_cfg[CFG_HW_AUD_INPUT_PATH_NUM];

#if defined(BLE_AUDIO_STEREO_CHAN_OVER_CIS_CNT)
#define CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV  (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
#else
#if defined(FREEMAN_ENABLED_STERO)
#define CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV  (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
#else
#define CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV  (AUD_CHANNEL_MAP_CH0)
#endif
#endif

#define CFG_HW_AUD_SIDETONE_MIC_DEV         (AUD_CHANNEL_MAP_CH0)
#define CFG_HW_AUD_SIDETONE_GAIN_DBVAL      (-20)

//bt config
extern const char *BT_LOCAL_NAME;
extern const char *BLE_DEFAULT_NAME;
extern uint8_t ble_global_addr[6];
extern uint8_t bt_global_addr[6];

#define CODEC_SADC_VOL (1)

extern const struct CODEC_DAC_VOL_T codec_dac_vol[TGT_VOLUME_LEVEL_QTY];
extern const struct CODEC_DAC_VOL_T codec_dac_a2dp_vol[TGT_VOLUME_LEVEL_QTY];
extern const struct CODEC_DAC_VOL_T codec_dac_hfp_vol[TGT_VOLUME_LEVEL_QTY];

#define CFG_AUD_EQ_IIR_NUM_BANDS (4)

//battery info
#define APP_BATTERY_MIN_MV (3200)
#define APP_BATTERY_PD_MV   (3100)

#define APP_BATTERY_MAX_MV (4200)

extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_enable_cfg;
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_detecter_cfg;
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_indicator_cfg;

#ifdef __cplusplus
}
#endif

#endif