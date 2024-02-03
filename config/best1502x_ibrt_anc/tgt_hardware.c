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
#include "tgt_hardware.h"
#include "aud_section.h"
#include "iir_process.h"
#include "fir_process.h"
#include "drc.h"
#include "limiter.h"
#include "spectrum_fix.h"
#include "dynamic_boost.h"
#include "../../apps/key/app_key.h" //Add by lewis

#ifdef CMT_008_LDO_3V0_ENABLE
const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_ldo_3v0_enable = {
    HAL_IOMUX_PIN_P1_7, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
};
#endif /*CMT_008_LDO_3V0_ENABLE*/

#ifdef CMT_008_LDO_1V8_ENABLE
const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_ldo_1v8_enable = {
    HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
};
#endif /*CMT_008_LDO_1V8_ENABLE*/

#ifdef CMT_008_AC107_ADC
const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_ac107_ldo_enable = {
    HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL
};
#endif /*CMT_008_AC107_ADC*/

#ifdef CMT_008_3_5JACK_CTR
const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pio_3p5_jack_detecter = {
    HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
};
#endif

#ifdef CMT_008_UART_USBAUDIO_SWITCH
const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_uart_usbaudio_switch = {
    HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL
};
#endif /*CMT_008_UART_USBAUDIO_SWITCH*/

#ifdef CMT_008_CHARGE_CURRENT
const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_charge_current_control = {
    HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL
};
#endif /*CMT_008_CHARGE_CURRENT*/

#ifdef CMT_008_CST820_TOUCH
const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_cst820_touch_intr_det = {
    HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_cst820_touch_rst = {
    HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL
};
#endif /*CMT_008_CST820_TOUCH*/

const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pinmux_pwl[CFG_HW_PWL_NUM] = {
#if (CFG_HW_PWL_NUM > 0)
#ifdef CMT_008_UI_LED_INDICATION

    //white led
    {HAL_IOMUX_PIN_P3_2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    //Red led
    {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},

#else /*CMT_008_UI_LED_INDICATION*/
    {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
    {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
#endif /*CMT_008_UI_LED_INDICATION*/
#endif
};

#ifdef __APP_USE_LED_INDICATE_IBRT_STATUS__
const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_ibrt_indication_pinmux_pwl[3] = {
    {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
    {HAL_IOMUX_PIN_LED1, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VBAT, HAL_IOMUX_PIN_PULLUP_ENABLE},
    {HAL_IOMUX_PIN_LED2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VBAT, HAL_IOMUX_PIN_PULLUP_ENABLE},
};
#endif

#ifdef __KNOWLES
const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_pinmux_uart[2] = {
    {HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_UART2_RX, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_UART2_TX,  HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
};
#endif

//adckey define
const uint16_t CFG_HW_ADCKEY_MAP_TABLE[CFG_HW_ADCKEY_NUMBER] = {
#if (CFG_HW_ADCKEY_NUMBER > 0)
    HAL_KEY_CODE_FN9,HAL_KEY_CODE_FN8,HAL_KEY_CODE_FN7,
    HAL_KEY_CODE_FN6,HAL_KEY_CODE_FN5,HAL_KEY_CODE_FN4,
    HAL_KEY_CODE_FN3,HAL_KEY_CODE_FN2,HAL_KEY_CODE_FN1,
#endif
};

//gpiokey define
#define CFG_HW_GPIOKEY_DOWN_LEVEL          (0)
#define CFG_HW_GPIOKEY_UP_LEVEL            (1)
const struct HAL_KEY_GPIOKEY_CFG_T cfg_hw_gpio_key_cfg[CFG_HW_GPIOKEY_NUM] = {
#if (CFG_HW_GPIOKEY_NUM > 0)
#ifdef CMT_008_UI
    //ANC_Button
    {APP_KEY_CODE_ANC,{HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},0},
    //Voice_Assistant_Button
    {APP_KEY_CODE_VOICE_ASSISTANT,{HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},0},

#else /* CMT_008_UI */
#ifdef BES_AUDIO_DEV_Main_Board_9v0
    {HAL_KEY_CODE_FN1,{HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
    {HAL_KEY_CODE_FN2,{HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
    {HAL_KEY_CODE_FN3,{HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
    {HAL_KEY_CODE_FN4,{HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
    {HAL_KEY_CODE_FN5,{HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
    {HAL_KEY_CODE_FN6,{HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
#else
    {HAL_KEY_CODE_FN1,{HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},//left
    {HAL_KEY_CODE_FN2,{HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},//right
    {HAL_KEY_CODE_FN3,{HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},//middle
#ifndef TPORTS_KEY_COEXIST
    // {HAL_KEY_CODE_FN3,{HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
    //{HAL_KEY_CODE_FN15,{HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
#endif
#endif
#ifdef IS_MULTI_AI_ENABLED
    //{HAL_KEY_CODE_FN13,{HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
    //{HAL_KEY_CODE_FN14,{HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
#endif
#endif
#endif /* CMT_008_UI */
};

//bt config
const char *BT_LOCAL_NAME = TO_STRING(BT_DEV_NAME) "\0";
const char *BLE_DEFAULT_NAME = "BES_BLE";
uint8_t ble_global_addr[6] = {
#ifdef BLE_DEV_ADDR
	BLE_DEV_ADDR
#else
	0xBE,0x99,0x34,0x45,0x56,0x67
#endif
};
uint8_t bt_global_addr[6] = {
#ifdef BT_DEV_ADDR
	BT_DEV_ADDR
#else
	0x1e,0x57,0x34,0x45,0x56,0x67
#endif
};

#ifdef __TENCENT_VOICE__
#define REVISION_INFO ("0.1.0\0")
const char *BT_FIRMWARE_VERSION = REVISION_INFO;
#endif

#define TX_PA_GAIN                          CODEC_TX_PA_GAIN_DEFAULT

//lewis: USBaudio & audio linein vol table
const struct CODEC_DAC_VOL_T codec_dac_vol[TGT_VOLUME_LEVEL_QTY] = {
    {TX_PA_GAIN,0x03,-99},
    {TX_PA_GAIN,0x03,-60},
    {TX_PA_GAIN,0x03,-55},
    {TX_PA_GAIN,0x03,-52},
    {TX_PA_GAIN,0x03,-47},
    {TX_PA_GAIN,0x03,-42},
    {TX_PA_GAIN,0x03,-38},
    {TX_PA_GAIN,0x03,-34},
    {TX_PA_GAIN,0x03,-30},
    {TX_PA_GAIN,0x03,-26},
    {TX_PA_GAIN,0x03,-22},
    {TX_PA_GAIN,0x03,-18},
    {TX_PA_GAIN,0x03,-15},
    {TX_PA_GAIN,0x03,-12},
    {TX_PA_GAIN,0x03, -9},
    {TX_PA_GAIN,0x03, -6},
    {TX_PA_GAIN,0x03, -3},  //0dBm
};

/* Add by lewis: prompt vol table */
#ifdef CODEC_DAC_PROMPT_ALONE_VOLUME_TABLE
const struct CODEC_DAC_VOL_T codec_dac_prompt_vol[TGT_PROMPT_VOL_LEVEL_QTY] = {
    {TX_PA_GAIN,0x03,-99},
    {TX_PA_GAIN,0x03,-60},
    {TX_PA_GAIN,0x03,-55},
    {TX_PA_GAIN,0x03,-52},
    {TX_PA_GAIN,0x03,-47},
    {TX_PA_GAIN,0x03,-42},
    {TX_PA_GAIN,0x03,-38},
    {TX_PA_GAIN,0x03,-34},
    {TX_PA_GAIN,0x03,-30},
    {TX_PA_GAIN,0x03,-26},
    {TX_PA_GAIN,0x03,-22},
    {TX_PA_GAIN,0x03,-18},
    {TX_PA_GAIN,0x03,-15},
    {TX_PA_GAIN,0x03,-12},
    {TX_PA_GAIN,0x03, -9},
    {TX_PA_GAIN,0x03, -6},
    {TX_PA_GAIN,0x03, -3},  //0dBm
};
#endif
/* End add by lewis */

const struct CODEC_DAC_VOL_T codec_dac_a2dp_vol[TGT_VOLUME_LEVEL_QTY] = {
    {TX_PA_GAIN,0x03,-99},
    {TX_PA_GAIN,0x03,-60},
    {TX_PA_GAIN,0x03,-55},
    {TX_PA_GAIN,0x03,-52},
    {TX_PA_GAIN,0x03,-47},
    {TX_PA_GAIN,0x03,-42},
    {TX_PA_GAIN,0x03,-38},
    {TX_PA_GAIN,0x03,-34},
    {TX_PA_GAIN,0x03,-30},
    {TX_PA_GAIN,0x03,-26},
    {TX_PA_GAIN,0x03,-22},
    {TX_PA_GAIN,0x03,-18},
    {TX_PA_GAIN,0x03,-15},
    {TX_PA_GAIN,0x03,-12},
    {TX_PA_GAIN,0x03, -9},
    {TX_PA_GAIN,0x03, -6},
    {TX_PA_GAIN,0x03, -3},  //0dBm
};

const struct CODEC_DAC_VOL_T codec_dac_hfp_vol[TGT_VOLUME_LEVEL_QTY] = {
    {TX_PA_GAIN,0x03,-99},
    {TX_PA_GAIN,0x03,-51},
    {TX_PA_GAIN,0x03,-48},
    {TX_PA_GAIN,0x03,-45},
    {TX_PA_GAIN,0x03,-42},
    {TX_PA_GAIN,0x03,-39},
    {TX_PA_GAIN,0x03,-36},
    {TX_PA_GAIN,0x03,-33},
    {TX_PA_GAIN,0x03,-30},
    {TX_PA_GAIN,0x03,-27},
    {TX_PA_GAIN,0x03,-24},
    {TX_PA_GAIN,0x03,-21},
    {TX_PA_GAIN,0x03,-18},
    {TX_PA_GAIN,0x03,-15},
    {TX_PA_GAIN,0x03,-12},
    {TX_PA_GAIN,0x03, -9},
    {TX_PA_GAIN,0x03, -6},  //0dBm
};

// Dev mother board VMIC1 <---> CHIP VMIC2
// Dev mother board VMIC2 <---> CHIP VMIC1
#ifndef VMIC_MAP_CFG
#ifdef CMT_008_MIC_CONFIG
#define VMIC_MAP_CFG                        AUD_VMIC_MAP_VMIC2
#else
#define VMIC_MAP_CFG                        AUD_VMIC_MAP_VMIC1
#endif
#endif

#if SPEECH_CODEC_CAPTURE_CHANNEL_NUM == 2
#define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV   (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | VMIC_MAP_CFG)
#elif SPEECH_CODEC_CAPTURE_CHANNEL_NUM == 3
#define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV   (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH4 | VMIC_MAP_CFG)
#else

#ifdef CMT_008_MIC_CONFIG
/* Talk mic config. */
#define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV   (AUD_CHANNEL_MAP_CH3 | VMIC_MAP_CFG)
#else /*CMT_008_MIC_CONFIG*/
#define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV   (AUD_CHANNEL_MAP_CH0 | VMIC_MAP_CFG)
#endif /*CMT_008_MIC_CONFIG*/

#endif

#ifdef CMT_008_SPP_TOTA_V2
#define CFG_HW_AUD_INPUT_PATH_LFFMIC_DEV    (ANC_FF_MIC_CH_L | AUD_VMIC_MAP_VMIC1)
#define CFG_HW_AUD_INPUT_PATH_RFFMIC_DEV    (ANC_FF_MIC_CH_R | AUD_VMIC_MAP_VMIC1)
#define CFG_HW_AUD_INPUT_PATH_LFBMIC_DEV    (ANC_FB_MIC_CH_L | AUD_VMIC_MAP_VMIC1)
#define CFG_HW_AUD_INPUT_PATH_RFBMIC_DEV    (ANC_FB_MIC_CH_R | AUD_VMIC_MAP_VMIC1)
#endif /*CMT_008_SPP_TOTA_V2*/

#define CFG_HW_AUD_INPUT_PATH_LINEIN_DEV    (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)

#ifdef VOICE_DETECTOR_SENS_EN
#define CFG_HW_AUD_INPUT_PATH_VADMIC_DEV    (AUD_CHANNEL_MAP_CH4 | VMIC_MAP_CFG)
#else
#define CFG_HW_AUD_INPUT_PATH_ASRMIC_DEV    (AUD_CHANNEL_MAP_CH0 | VMIC_MAP_CFG)
//#define CFG_HW_AUD_INPUT_PATH_ASRMIC_DEV    (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_ECMIC_CH0 | VMIC_MAP_CFG)
#endif

#define CFG_HW_AUD_INPUT_PATH_ANC_ASSIST_DEV   (ANC_FF_MIC_CH_L | ANC_FB_MIC_CH_L | ANC_TALK_MIC_CH_L | ANC_REF_MIC_CH_L | VMIC_MAP_CFG)

#define CFG_HW_AUD_INPUT_PATH_HEARING_DEV   (AUD_CHANNEL_MAP_CH0 | VMIC_MAP_CFG)

#define CFG_HW_AUD_INPUT_PATH_DC_CALIB      (CFG_ADC_DC_CALIB_MIC_DEV | VMIC_MAP_CFG)

const struct AUD_IO_PATH_CFG_T cfg_audio_input_path_cfg[CFG_HW_AUD_INPUT_PATH_NUM] = {
#if defined(SPEECH_TX_AEC_CODEC_REF)
    // NOTE: If enable Ch5 and CH6, need to add channel_num when setup audioflinger stream
    { AUD_INPUT_PATH_MAINMIC, CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV | AUD_CHANNEL_MAP_ECMIC_CH0, },
#else
    { AUD_INPUT_PATH_MAINMIC, CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV, },
#endif
    { AUD_INPUT_PATH_LINEIN,  CFG_HW_AUD_INPUT_PATH_LINEIN_DEV, },
#ifdef VOICE_DETECTOR_SENS_EN
    { AUD_INPUT_PATH_VADMIC,  CFG_HW_AUD_INPUT_PATH_VADMIC_DEV, },
#else
    { AUD_INPUT_PATH_ASRMIC,  CFG_HW_AUD_INPUT_PATH_ASRMIC_DEV, },
#endif
    { AUD_INPUT_PATH_ANC_ASSIST,    CFG_HW_AUD_INPUT_PATH_ANC_ASSIST_DEV, },
#if defined(SPEECH_TX_AEC_CODEC_REF)
    { AUD_INPUT_PATH_HEARING,   CFG_HW_AUD_INPUT_PATH_HEARING_DEV | AUD_CHANNEL_MAP_ECMIC_CH0, },
#else
    { AUD_INPUT_PATH_HEARING,   CFG_HW_AUD_INPUT_PATH_HEARING_DEV, },
#endif
    { AUD_INPUT_PATH_DC_CALIB,  CFG_HW_AUD_INPUT_PATH_DC_CALIB, },

#ifdef CMT_008_SPP_TOTA_V2
    { AUD_INPUT_PATH_LFFMIC_SPP,  CFG_HW_AUD_INPUT_PATH_LFFMIC_DEV | AUD_CHANNEL_MAP_ECMIC_CH0},
    { AUD_INPUT_PATH_RFFMIC_SPP,  CFG_HW_AUD_INPUT_PATH_RFFMIC_DEV | AUD_CHANNEL_MAP_ECMIC_CH0},
    { AUD_INPUT_PATH_LFBMIC_SPP,  CFG_HW_AUD_INPUT_PATH_LFBMIC_DEV | AUD_CHANNEL_MAP_ECMIC_CH0},
    { AUD_INPUT_PATH_RFBMIC_SPP,  CFG_HW_AUD_INPUT_PATH_RFBMIC_DEV | AUD_CHANNEL_MAP_ECMIC_CH0},
#endif /*CMT_008_SPP_TOTA_V2*/

};

#ifdef CMT_008_NTC_DETECT
const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_ntc_volt_ctr = {
    HAL_GPIO_PIN_P1_3, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_NOPULL,
};
#endif /*CMT_008_NTC_DETECT*/

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_enable_cfg = {
    HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLDOWN_ENABLE
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_detecter_cfg = {
    HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_indicator_cfg = {
    HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
};

#define IIR_COUNTER_FF_L (6)
#define IIR_COUNTER_FF_R (6)
#define IIR_COUNTER_FB_L (5)
#define IIR_COUNTER_FB_R (5)

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_50p7k_mode0 = {
    .anc_cfg_ff_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6,
		
		.iir_coef[0].coef_b={0xffffae3d,0xffff5c7b,0xffffae3d}, .iir_coef[0].coef_a={0x08000000,0xf0341af4,0x07cd2c16},
		.iir_coef[1].coef_b={0x08018c4a,0xf001b43d,0x07fcbfd7}, .iir_coef[1].coef_a={0x08000000,0xf001b43d,0x07fe4c21},
		.iir_coef[2].coef_b={0x08032692,0xf007276c,0x07f5b7e2}, .iir_coef[2].coef_a={0x08000000,0xf007276c,0x07f8de74},
		.iir_coef[3].coef_b={0x0801c869,0xf008b1e9,0x07f592e7}, .iir_coef[3].coef_a={0x08000000,0xf008b1e9,0x07f75b4f},
		.iir_coef[4].coef_b={0x08032e15,0xf00af72d,0x07f1f23b}, .iir_coef[4].coef_a={0x08000000,0xf00af72d,0x07f5204f},
		.iir_coef[5].coef_b={0x07f8476e,0xf0238ffa,0x07e47018}, .iir_coef[5].coef_a={0x08000000,0xf0238ffa,0x07dcb786},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_ff_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffae3d,0xffff5c7b,0xffffae3d}, .iir_coef[0].coef_a={0x08000000,0xf0341af4,0x07cd2c16},
		.iir_coef[1].coef_b={0x08018c4a,0xf001b43d,0x07fcbfd7}, .iir_coef[1].coef_a={0x08000000,0xf001b43d,0x07fe4c21},
		.iir_coef[2].coef_b={0x08032692,0xf007276c,0x07f5b7e2}, .iir_coef[2].coef_a={0x08000000,0xf007276c,0x07f8de74},
		.iir_coef[3].coef_b={0x0801c869,0xf008b1e9,0x07f592e7}, .iir_coef[3].coef_a={0x08000000,0xf008b1e9,0x07f75b4f},
		.iir_coef[4].coef_b={0x08032e15,0xf00af72d,0x07f1f23b}, .iir_coef[4].coef_a={0x08000000,0xf00af72d,0x07f5204f},
		.iir_coef[5].coef_b={0x07f8476e,0xf0238ffa,0x07e47018}, .iir_coef[5].coef_a={0x08000000,0xf0238ffa,0x07dcb786},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_fb_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7, 
		
		.iir_coef[0].coef_b={0xf80699c1,0x0fd2e635,0xf8260354}, .iir_coef[0].coef_a={0x08000000,0xf02d57da,0x07d3a0f9},
		.iir_coef[1].coef_b={0x080153b2,0xf00175fe,0x07fd36ae}, .iir_coef[1].coef_a={0x08000000,0xf00175fe,0x07fe8a60},
		.iir_coef[2].coef_b={0x0802a727,0xf002ec74,0x07fa6dde}, .iir_coef[2].coef_a={0x08000000,0xf002ec74,0x07fd1504},
		.iir_coef[3].coef_b={0x080a96ca,0xf00bbd06,0x07e9c3ac}, .iir_coef[3].coef_a={0x08000000,0xf00bbd06,0x07f45a76},
		.iir_coef[4].coef_b={0x08059119,0xf00625ea,0x07f44f78}, .iir_coef[4].coef_a={0x08000000,0xf00625ea,0x07f9e091},
		.iir_coef[5].coef_b={0x07995584,0xf1515363,0x072869fc}, .iir_coef[5].coef_a={0x08000000,0xf1515363,0x06c1bf80},
		.iir_coef[6].coef_b={0x08010c74,0xf0031734,0x07fbdd8e}, .iir_coef[6].coef_a={0x08000000,0xf00316d0,0x07fce99e},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_fb_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7, 
		
		.iir_coef[0].coef_b={0xf80699c1,0x0fd2e635,0xf8260354}, .iir_coef[0].coef_a={0x08000000,0xf02d57da,0x07d3a0f9},
		.iir_coef[1].coef_b={0x080153b2,0xf00175fe,0x07fd36ae}, .iir_coef[1].coef_a={0x08000000,0xf00175fe,0x07fe8a60},
		.iir_coef[2].coef_b={0x0802a727,0xf002ec74,0x07fa6dde}, .iir_coef[2].coef_a={0x08000000,0xf002ec74,0x07fd1504},
		.iir_coef[3].coef_b={0x080a96ca,0xf00bbd06,0x07e9c3ac}, .iir_coef[3].coef_a={0x08000000,0xf00bbd06,0x07f45a76},
		.iir_coef[4].coef_b={0x08059119,0xf00625ea,0x07f44f78}, .iir_coef[4].coef_a={0x08000000,0xf00625ea,0x07f9e091},
		.iir_coef[5].coef_b={0x07995584,0xf1515363,0x072869fc}, .iir_coef[5].coef_a={0x08000000,0xf1515363,0x06c1bf80},
		.iir_coef[6].coef_b={0x08010c74,0xf0031734,0x07fbdd8e}, .iir_coef[6].coef_a={0x08000000,0xf00316d0,0x07fce99e},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1,
		
		.iir_coef[0].coef_b={0xfe9385a0,0x02d7ce9b,0xfe94ab9a}, .iir_coef[0].coef_a={0x08000000,0xf0033d7b,0x07fcc376},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={131723630,    -260565855,    130644731},  //7K
		// .iir_coef[2].coef_a={134217728,    -260565855,    128150634},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 512,

        .iir_bypass_flag = 0,
		.iir_counter=1,
		
		.iir_coef[0].coef_b={0xfe9385a0,0x02d7ce9b,0xfe94ab9a}, .iir_coef[0].coef_a={0x08000000,0xf0033d7b,0x07fcc376},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
};

/* Add by lewis */
static struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_50p7k_mode1 = {
    .anc_cfg_ff_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6,
		
		.iir_coef[0].coef_b={0xf016ffac,0x1fcc3c0c,0xf01cc012}, .iir_coef[0].coef_a={0x08000000,0xf01037e7,0x07efca36},
		.iir_coef[1].coef_b={0x07ecb5e1,0xf02efe33,0x07e45dbf}, .iir_coef[1].coef_a={0x08000000,0xf02efe33,0x07d113a0},
		.iir_coef[2].coef_b={0x080fdc60,0xf011a6ae,0x07deb1b5}, .iir_coef[2].coef_a={0x08000000,0xf011a6ae,0x07ee8e15},
		.iir_coef[3].coef_b={0x083a25fa,0xf0226fbb,0x07a44e60}, .iir_coef[3].coef_a={0x08000000,0xf0226fbb,0x07de745b},
		.iir_coef[4].coef_b={0x0832d509,0xf024623b,0x07ab1086}, .iir_coef[4].coef_a={0x08000000,0xf024623b,0x07dde58f},
		.iir_coef[5].coef_b={0x06e0373d,0xf297445a,0x06a04383}, .iir_coef[5].coef_a={0x08000000,0xf297445a,0x05807ac0},

        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_ff_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6,
		
		.iir_coef[0].coef_b={0xf016ffac,0x1fcc3c0c,0xf01cc012}, .iir_coef[0].coef_a={0x08000000,0xf01037e7,0x07efca36},
		.iir_coef[1].coef_b={0x07ecb5e1,0xf02efe33,0x07e45dbf}, .iir_coef[1].coef_a={0x08000000,0xf02efe33,0x07d113a0},
		.iir_coef[2].coef_b={0x080fdc60,0xf011a6ae,0x07deb1b5}, .iir_coef[2].coef_a={0x08000000,0xf011a6ae,0x07ee8e15},
		.iir_coef[3].coef_b={0x083a25fa,0xf0226fbb,0x07a44e60}, .iir_coef[3].coef_a={0x08000000,0xf0226fbb,0x07de745b},
		.iir_coef[4].coef_b={0x0832d509,0xf024623b,0x07ab1086}, .iir_coef[4].coef_a={0x08000000,0xf024623b,0x07dde58f},
		.iir_coef[5].coef_b={0x06e0373d,0xf297445a,0x06a04383}, .iir_coef[5].coef_a={0x08000000,0xf297445a,0x05807ac0},

        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_fb_l = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
		.iir_counter = 1,
		
        .iir_coef[0].coef_b = {0x08000000,0xf022991b,0x07ddf8db}, .iir_coef[0].coef_a = {0x08000000,0xf022991b,0x07ddf8db},

        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_fb_r = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
		.iir_counter = 1,
		
        .iir_coef[0].coef_b = {0x08000000,0xf022991b,0x07ddf8db}, .iir_coef[0].coef_a = {0x08000000,0xf022991b,0x07ddf8db},

        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
		.iir_counter = 1,
		
        .iir_coef[0].coef_b = {0x08000000,0xf022991b,0x07ddf8db}, .iir_coef[0].coef_a = {0x08000000,0xf022991b,0x07ddf8db},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
		.iir_counter = 1,
		
        .iir_coef[0].coef_b = {0x08000000,0xf022991b,0x07ddf8db}, .iir_coef[0].coef_a = {0x08000000,0xf022991b,0x07ddf8db},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
};

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_50p7k_mode2 = {
    .anc_cfg_ff_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xf016ffac,0x1fcc3c0c,0xf01cc012}, .iir_coef[0].coef_a={0x08000000,0xf01037e7,0x07efca36},
		.iir_coef[1].coef_b={0x07ecb5e1,0xf02efe33,0x07e45dbf}, .iir_coef[1].coef_a={0x08000000,0xf02efe33,0x07d113a0},
		.iir_coef[2].coef_b={0x080a4f2f,0xf014ec37,0x07e0f952}, .iir_coef[2].coef_a={0x08000000,0xf014ec37,0x07eb4881},
		.iir_coef[3].coef_b={0x08376c4b,0xf0188be8,0x07b09a1f}, .iir_coef[3].coef_a={0x08000000,0xf0188be8,0x07e8066b},
		.iir_coef[4].coef_b={0x081baa69,0xf01fb67b,0x07c5e7cd}, .iir_coef[4].coef_a={0x08000000,0xf01fb67b,0x07e19236},
		.iir_coef[5].coef_b={0x06e0373d,0xf297445a,0x06a04383}, .iir_coef[5].coef_a={0x08000000,0xf297445a,0x05807ac0},

		.dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_ff_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xf016ffac,0x1fcc3c0c,0xf01cc012}, .iir_coef[0].coef_a={0x08000000,0xf01037e7,0x07efca36},
		.iir_coef[1].coef_b={0x07ecb5e1,0xf02efe33,0x07e45dbf}, .iir_coef[1].coef_a={0x08000000,0xf02efe33,0x07d113a0},
		.iir_coef[2].coef_b={0x080a4f2f,0xf014ec37,0x07e0f952}, .iir_coef[2].coef_a={0x08000000,0xf014ec37,0x07eb4881},
		.iir_coef[3].coef_b={0x08376c4b,0xf0188be8,0x07b09a1f}, .iir_coef[3].coef_a={0x08000000,0xf0188be8,0x07e8066b},
		.iir_coef[4].coef_b={0x081baa69,0xf01fb67b,0x07c5e7cd}, .iir_coef[4].coef_a={0x08000000,0xf01fb67b,0x07e19236},
		.iir_coef[5].coef_b={0x06e0373d,0xf297445a,0x06a04383}, .iir_coef[5].coef_a={0x08000000,0xf297445a,0x05807ac0},

		.dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_fb_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=2, 
		
		.iir_coef[0].coef_b={0xf1c15dba,0x1c6ea227,0xf1cffd82}, .iir_coef[0].coef_a={0x08000000,0xf002ec74,0x07fd1504},
		.iir_coef[1].coef_b={0x08066dfd,0xf012fe1a,0x07e6d167}, .iir_coef[1].coef_a={0x08000000,0xf012ea44,0x07ed2b8d},

        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_fb_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=2, 
		
		.iir_coef[0].coef_b={0xf1c15dba,0x1c6ea227,0xf1cffd82}, .iir_coef[0].coef_a={0x08000000,0xf002ec74,0x07fd1504},
		.iir_coef[1].coef_b={0x08066dfd,0xf012fe1a,0x07e6d167}, .iir_coef[1].coef_a={0x08000000,0xf012ea44,0x07ed2b8d},

        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1, 
		
		.iir_coef[0].coef_b={0x016c6fe8,0xfd281c5c,0x016b73e6}, .iir_coef[0].coef_a={0x08000000,0xf002c733,0x07fd39be},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1, 
		
		.iir_coef[0].coef_b={0x016c6fe8,0xfd281c5c,0x016b73e6}, .iir_coef[0].coef_a={0x08000000,0xf002c733,0x07fd39be},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
};

static struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_50p7k_mode3 = {
    .anc_cfg_ff_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6,
		
		.iir_coef[0].coef_b={0xffffae3d,0xffff5c7b,0xffffae3d}, .iir_coef[0].coef_a={0x08000000,0xf0341af4,0x07cd2c16},
		.iir_coef[1].coef_b={0x08018c4a,0xf001b43d,0x07fcbfd7}, .iir_coef[1].coef_a={0x08000000,0xf001b43d,0x07fe4c21},
		.iir_coef[2].coef_b={0x08032692,0xf007276c,0x07f5b7e2}, .iir_coef[2].coef_a={0x08000000,0xf007276c,0x07f8de74},
		.iir_coef[3].coef_b={0x0801c869,0xf008b1e9,0x07f592e7}, .iir_coef[3].coef_a={0x08000000,0xf008b1e9,0x07f75b4f},
		.iir_coef[4].coef_b={0x08032e15,0xf00af72d,0x07f1f23b}, .iir_coef[4].coef_a={0x08000000,0xf00af72d,0x07f5204f},
		.iir_coef[5].coef_b={0x07f8476e,0xf0238ffa,0x07e47018}, .iir_coef[5].coef_a={0x08000000,0xf0238ffa,0x07dcb786},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_ff_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffae3d,0xffff5c7b,0xffffae3d}, .iir_coef[0].coef_a={0x08000000,0xf0341af4,0x07cd2c16},
		.iir_coef[1].coef_b={0x08018c4a,0xf001b43d,0x07fcbfd7}, .iir_coef[1].coef_a={0x08000000,0xf001b43d,0x07fe4c21},
		.iir_coef[2].coef_b={0x08032692,0xf007276c,0x07f5b7e2}, .iir_coef[2].coef_a={0x08000000,0xf007276c,0x07f8de74},
		.iir_coef[3].coef_b={0x0801c869,0xf008b1e9,0x07f592e7}, .iir_coef[3].coef_a={0x08000000,0xf008b1e9,0x07f75b4f},
		.iir_coef[4].coef_b={0x08032e15,0xf00af72d,0x07f1f23b}, .iir_coef[4].coef_a={0x08000000,0xf00af72d,0x07f5204f},
		.iir_coef[5].coef_b={0x07f8476e,0xf0238ffa,0x07e47018}, .iir_coef[5].coef_a={0x08000000,0xf0238ffa,0x07dcb786},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_fb_l = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
		.iir_counter=7, 
		
		.iir_coef[0].coef_b={0xf80699c1,0x0fd2e635,0xf8260354}, .iir_coef[0].coef_a={0x08000000,0xf02d57da,0x07d3a0f9},
		.iir_coef[1].coef_b={0x080153b2,0xf00175fe,0x07fd36ae}, .iir_coef[1].coef_a={0x08000000,0xf00175fe,0x07fe8a60},
		.iir_coef[2].coef_b={0x0802a727,0xf002ec74,0x07fa6dde}, .iir_coef[2].coef_a={0x08000000,0xf002ec74,0x07fd1504},
		.iir_coef[3].coef_b={0x080a96ca,0xf00bbd06,0x07e9c3ac}, .iir_coef[3].coef_a={0x08000000,0xf00bbd06,0x07f45a76},
		.iir_coef[4].coef_b={0x08059119,0xf00625ea,0x07f44f78}, .iir_coef[4].coef_a={0x08000000,0xf00625ea,0x07f9e091},
		.iir_coef[5].coef_b={0x07995584,0xf1515363,0x072869fc}, .iir_coef[5].coef_a={0x08000000,0xf1515363,0x06c1bf80},
		.iir_coef[6].coef_b={0x08010c74,0xf0031734,0x07fbdd8e}, .iir_coef[6].coef_a={0x08000000,0xf00316d0,0x07fce99e},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_fb_r = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
		.iir_counter=7, 
		
		.iir_coef[0].coef_b={0xf80699c1,0x0fd2e635,0xf8260354}, .iir_coef[0].coef_a={0x08000000,0xf02d57da,0x07d3a0f9},
		.iir_coef[1].coef_b={0x080153b2,0xf00175fe,0x07fd36ae}, .iir_coef[1].coef_a={0x08000000,0xf00175fe,0x07fe8a60},
		.iir_coef[2].coef_b={0x0802a727,0xf002ec74,0x07fa6dde}, .iir_coef[2].coef_a={0x08000000,0xf002ec74,0x07fd1504},
		.iir_coef[3].coef_b={0x080a96ca,0xf00bbd06,0x07e9c3ac}, .iir_coef[3].coef_a={0x08000000,0xf00bbd06,0x07f45a76},
		.iir_coef[4].coef_b={0x08059119,0xf00625ea,0x07f44f78}, .iir_coef[4].coef_a={0x08000000,0xf00625ea,0x07f9e091},
		.iir_coef[5].coef_b={0x07995584,0xf1515363,0x072869fc}, .iir_coef[5].coef_a={0x08000000,0xf1515363,0x06c1bf80},
		.iir_coef[6].coef_b={0x08010c74,0xf0031734,0x07fbdd8e}, .iir_coef[6].coef_a={0x08000000,0xf00316d0,0x07fce99e},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
		.iir_counter=1,
		
		.iir_coef[0].coef_b={0xfe9385a0,0x02d7ce9b,0xfe94ab9a}, .iir_coef[0].coef_a={0x08000000,0xf0033d7b,0x07fcc376},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={131723630,    -260565855,    130644731},  //7K
		// .iir_coef[2].coef_a={134217728,    -260565855,    128150634},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 0,

        .iir_bypass_flag = 0,
		.iir_counter=1,
		
		.iir_coef[0].coef_b={0xfe9385a0,0x02d7ce9b,0xfe94ab9a}, .iir_coef[0].coef_a={0x08000000,0xf0033d7b,0x07fcc376},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
};

static struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_50p7k_mode4 = {
    .anc_cfg_ff_l = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
		.iir_counter=6,
		
		.iir_coef[0].coef_b={0xffffae3d,0xffff5c7b,0xffffae3d}, .iir_coef[0].coef_a={0x08000000,0xf0341af4,0x07cd2c16},
		.iir_coef[1].coef_b={0x08018c4a,0xf001b43d,0x07fcbfd7}, .iir_coef[1].coef_a={0x08000000,0xf001b43d,0x07fe4c21},
		.iir_coef[2].coef_b={0x08032692,0xf007276c,0x07f5b7e2}, .iir_coef[2].coef_a={0x08000000,0xf007276c,0x07f8de74},
		.iir_coef[3].coef_b={0x0801c869,0xf008b1e9,0x07f592e7}, .iir_coef[3].coef_a={0x08000000,0xf008b1e9,0x07f75b4f},
		.iir_coef[4].coef_b={0x08032e15,0xf00af72d,0x07f1f23b}, .iir_coef[4].coef_a={0x08000000,0xf00af72d,0x07f5204f},
		.iir_coef[5].coef_b={0x07f8476e,0xf0238ffa,0x07e47018}, .iir_coef[5].coef_a={0x08000000,0xf0238ffa,0x07dcb786},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_ff_r = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffae3d,0xffff5c7b,0xffffae3d}, .iir_coef[0].coef_a={0x08000000,0xf0341af4,0x07cd2c16},
		.iir_coef[1].coef_b={0x08018c4a,0xf001b43d,0x07fcbfd7}, .iir_coef[1].coef_a={0x08000000,0xf001b43d,0x07fe4c21},
		.iir_coef[2].coef_b={0x08032692,0xf007276c,0x07f5b7e2}, .iir_coef[2].coef_a={0x08000000,0xf007276c,0x07f8de74},
		.iir_coef[3].coef_b={0x0801c869,0xf008b1e9,0x07f592e7}, .iir_coef[3].coef_a={0x08000000,0xf008b1e9,0x07f75b4f},
		.iir_coef[4].coef_b={0x08032e15,0xf00af72d,0x07f1f23b}, .iir_coef[4].coef_a={0x08000000,0xf00af72d,0x07f5204f},
		.iir_coef[5].coef_b={0x07f8476e,0xf0238ffa,0x07e47018}, .iir_coef[5].coef_a={0x08000000,0xf0238ffa,0x07dcb786},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_fb_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7, 
		
		.iir_coef[0].coef_b={0xf80699c1,0x0fd2e635,0xf8260354}, .iir_coef[0].coef_a={0x08000000,0xf02d57da,0x07d3a0f9},
		.iir_coef[1].coef_b={0x080153b2,0xf00175fe,0x07fd36ae}, .iir_coef[1].coef_a={0x08000000,0xf00175fe,0x07fe8a60},
		.iir_coef[2].coef_b={0x0802a727,0xf002ec74,0x07fa6dde}, .iir_coef[2].coef_a={0x08000000,0xf002ec74,0x07fd1504},
		.iir_coef[3].coef_b={0x080a96ca,0xf00bbd06,0x07e9c3ac}, .iir_coef[3].coef_a={0x08000000,0xf00bbd06,0x07f45a76},
		.iir_coef[4].coef_b={0x08059119,0xf00625ea,0x07f44f78}, .iir_coef[4].coef_a={0x08000000,0xf00625ea,0x07f9e091},
		.iir_coef[5].coef_b={0x07995584,0xf1515363,0x072869fc}, .iir_coef[5].coef_a={0x08000000,0xf1515363,0x06c1bf80},
		.iir_coef[6].coef_b={0x08010c74,0xf0031734,0x07fbdd8e}, .iir_coef[6].coef_a={0x08000000,0xf00316d0,0x07fce99e},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_fb_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7, 
		
		.iir_coef[0].coef_b={0xf80699c1,0x0fd2e635,0xf8260354}, .iir_coef[0].coef_a={0x08000000,0xf02d57da,0x07d3a0f9},
		.iir_coef[1].coef_b={0x080153b2,0xf00175fe,0x07fd36ae}, .iir_coef[1].coef_a={0x08000000,0xf00175fe,0x07fe8a60},
		.iir_coef[2].coef_b={0x0802a727,0xf002ec74,0x07fa6dde}, .iir_coef[2].coef_a={0x08000000,0xf002ec74,0x07fd1504},
		.iir_coef[3].coef_b={0x080a96ca,0xf00bbd06,0x07e9c3ac}, .iir_coef[3].coef_a={0x08000000,0xf00bbd06,0x07f45a76},
		.iir_coef[4].coef_b={0x08059119,0xf00625ea,0x07f44f78}, .iir_coef[4].coef_a={0x08000000,0xf00625ea,0x07f9e091},
		.iir_coef[5].coef_b={0x07995584,0xf1515363,0x072869fc}, .iir_coef[5].coef_a={0x08000000,0xf1515363,0x06c1bf80},
		.iir_coef[6].coef_b={0x08010c74,0xf0031734,0x07fbdd8e}, .iir_coef[6].coef_a={0x08000000,0xf00316d0,0x07fce99e},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1,
		
		.iir_coef[0].coef_b={0xfe9385a0,0x02d7ce9b,0xfe94ab9a}, .iir_coef[0].coef_a={0x08000000,0xf0033d7b,0x07fcc376},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={131723630,    -260565855,    130644731},  //7K
		// .iir_coef[2].coef_a={134217728,    -260565855,    128150634},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 512,

        .iir_bypass_flag = 0,
		.iir_counter=1,
		
		.iir_coef[0].coef_b={0xfe9385a0,0x02d7ce9b,0xfe94ab9a}, .iir_coef[0].coef_a={0x08000000,0xf0033d7b,0x07fcc376},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
};
/* End Add by lewis */

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_48k_mode0 = {
    .anc_cfg_ff_l = {
		.total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffb201,0xffff6402,0xffffb201}, .iir_coef[0].coef_a={0x08000000,0xf032deb0,0x07ce594c},
		.iir_coef[1].coef_b={0x08018301,0xf001aa02,0x07fcd356}, .iir_coef[1].coef_a={0x08000000,0xf001aa02,0x07fe5658},
		.iir_coef[2].coef_b={0x080313b3,0xf006fc70,0x07f5f578}, .iir_coef[2].coef_a={0x08000000,0xf006fc70,0x07f9092b},
		.iir_coef[3].coef_b={0x0801bdbc,0xf0087d8b,0x07f5d154}, .iir_coef[3].coef_a={0x08000000,0xf0087d8b,0x07f78f10},
		.iir_coef[4].coef_b={0x08031b0d,0xf00ab504,0x07f24655}, .iir_coef[4].coef_a={0x08000000,0xf00ab504,0x07f56162},
		.iir_coef[5].coef_b={0x07f8755d,0xf022bac5,0x07e51411}, .iir_coef[5].coef_a={0x08000000,0xf022bac5,0x07dd896e},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_ff_r = {
		.total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffb201,0xffff6402,0xffffb201}, .iir_coef[0].coef_a={0x08000000,0xf032deb0,0x07ce594c},
		.iir_coef[1].coef_b={0x08018301,0xf001aa02,0x07fcd356}, .iir_coef[1].coef_a={0x08000000,0xf001aa02,0x07fe5658},
		.iir_coef[2].coef_b={0x080313b3,0xf006fc70,0x07f5f578}, .iir_coef[2].coef_a={0x08000000,0xf006fc70,0x07f9092b},
		.iir_coef[3].coef_b={0x0801bdbc,0xf0087d8b,0x07f5d154}, .iir_coef[3].coef_a={0x08000000,0xf0087d8b,0x07f78f10},
		.iir_coef[4].coef_b={0x08031b0d,0xf00ab504,0x07f24655}, .iir_coef[4].coef_a={0x08000000,0xf00ab504,0x07f56162},
		.iir_coef[5].coef_b={0x07f8755d,0xf022bac5,0x07e51411}, .iir_coef[5].coef_a={0x08000000,0xf022bac5,0x07dd896e},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_fb_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7,
		
		.iir_coef[0].coef_b={0xf80671de,0x0fd3f62d,0xf82520ff}, .iir_coef[0].coef_a={0x08000000,0xf02c4506,0x07d4a856},
		.iir_coef[1].coef_b={0x08014bbd,0xf0016d39,0x07fd4764}, .iir_coef[1].coef_a={0x08000000,0xf0016d39,0x07fe9321},
		.iir_coef[2].coef_b={0x0802973f,0xf002dae4,0x07fa8f45}, .iir_coef[2].coef_a={0x08000000,0xf002dae4,0x07fd2683},
		.iir_coef[3].coef_b={0x080a576f,0xf00b7640,0x07ea48b6}, .iir_coef[3].coef_a={0x08000000,0xf00b7640,0x07f4a025},
		.iir_coef[4].coef_b={0x08056fbf,0xf00600ef,0x07f49581}, .iir_coef[4].coef_a={0x08000000,0xf00600ef,0x07fa0540},
		.iir_coef[5].coef_b={0x079b8cd5,0xf1499b3a,0x072d1147}, .iir_coef[5].coef_a={0x08000000,0xf1499b3a,0x06c89e1c},
		.iir_coef[6].coef_b={0x08010629,0xf00304a7,0x07fbf657}, .iir_coef[6].coef_a={0x08000000,0xf0030448,0x07fcfc21},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_fb_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7,
		
		.iir_coef[0].coef_b={0xf80671de,0x0fd3f62d,0xf82520ff}, .iir_coef[0].coef_a={0x08000000,0xf02c4506,0x07d4a856},
		.iir_coef[1].coef_b={0x08014bbd,0xf0016d39,0x07fd4764}, .iir_coef[1].coef_a={0x08000000,0xf0016d39,0x07fe9321},
		.iir_coef[2].coef_b={0x0802973f,0xf002dae4,0x07fa8f45}, .iir_coef[2].coef_a={0x08000000,0xf002dae4,0x07fd2683},
		.iir_coef[3].coef_b={0x080a576f,0xf00b7640,0x07ea48b6}, .iir_coef[3].coef_a={0x08000000,0xf00b7640,0x07f4a025},
		.iir_coef[4].coef_b={0x08056fbf,0xf00600ef,0x07f49581}, .iir_coef[4].coef_a={0x08000000,0xf00600ef,0x07fa0540},
		.iir_coef[5].coef_b={0x079b8cd5,0xf1499b3a,0x072d1147}, .iir_coef[5].coef_a={0x08000000,0xf1499b3a,0x06c89e1c},
		.iir_coef[6].coef_b={0x08010629,0xf00304a7,0x07fbf657}, .iir_coef[6].coef_a={0x08000000,0xf0030448,0x07fcfc21},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1,
		
		.iir_coef[0].coef_b={0xfe938757,0x02d7d210,0xfe94a66f}, .iir_coef[0].coef_a={0x08000000,0xf0032a09,0x07fcd6dd},

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1,
		
		.iir_coef[0].coef_b={0xfe938757,0x02d7d210,0xfe94a66f}, .iir_coef[0].coef_a={0x08000000,0xf0032a09,0x07fcd6dd},

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
};

/* Add by lewis */
static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_48k_mode1 = {
    .anc_cfg_ff_l = {
		.total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6,
		
		.iir_coef[0].coef_b={0xf016b034,0x1fccfd8d,0xf01c4e3a}, .iir_coef[0].coef_a={0x08000000,0xf00fd6eb,0x07f02b18},
		.iir_coef[1].coef_b={0x07ed2852,0xf02de6fe,0x07e501b1}, .iir_coef[1].coef_a={0x08000000,0xf02de6fe,0x07d22a03},
		.iir_coef[2].coef_b={0x080f7d9c,0xf0113c02,0x07df78b4}, .iir_coef[2].coef_a={0x08000000,0xf0113c02,0x07eef650},
		.iir_coef[3].coef_b={0x0838cbee,0xf0219d93,0x07a67010}, .iir_coef[3].coef_a={0x08000000,0xf0219d93,0x07df3bfd},
		.iir_coef[4].coef_b={0x0831a6a2,0xf0237c6a,0x07ad09cf}, .iir_coef[4].coef_a={0x08000000,0xf0237c6a,0x07deb071},
		.iir_coef[5].coef_b={0x06e5e732,0xf2899cc6,0x06a73705}, .iir_coef[5].coef_a={0x08000000,0xf2899cc6,0x058d1e37},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/

		.dac_gain_offset=0,
		.adc_gain_offset=-24,
    },
    .anc_cfg_ff_r = {
		.total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6,
		
		.iir_coef[0].coef_b={0xf016b034,0x1fccfd8d,0xf01c4e3a}, .iir_coef[0].coef_a={0x08000000,0xf00fd6eb,0x07f02b18},
		.iir_coef[1].coef_b={0x07ed2852,0xf02de6fe,0x07e501b1}, .iir_coef[1].coef_a={0x08000000,0xf02de6fe,0x07d22a03},
		.iir_coef[2].coef_b={0x080f7d9c,0xf0113c02,0x07df78b4}, .iir_coef[2].coef_a={0x08000000,0xf0113c02,0x07eef650},
		.iir_coef[3].coef_b={0x0838cbee,0xf0219d93,0x07a67010}, .iir_coef[3].coef_a={0x08000000,0xf0219d93,0x07df3bfd},
		.iir_coef[4].coef_b={0x0831a6a2,0xf0237c6a,0x07ad09cf}, .iir_coef[4].coef_a={0x08000000,0xf0237c6a,0x07deb071},
		.iir_coef[5].coef_b={0x06e5e732,0xf2899cc6,0x06a73705}, .iir_coef[5].coef_a={0x08000000,0xf2899cc6,0x058d1e37},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/

		.dac_gain_offset=0,
		.adc_gain_offset=-24,
    },
    .anc_cfg_fb_l = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
        .iir_counter = 1,
        
        .iir_coef[0].coef_b = {0x08000000,0xf021c7e1,0x07dec359}, .iir_coef[0].coef_a = {0x08000000,0xf021c7e1,0x07dec359},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
		.dac_gain_offset=0,
		.adc_gain_offset=-24,
    },
    .anc_cfg_fb_r = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
        .iir_counter = 1,
        
        .iir_coef[0].coef_b = {0x08000000,0xf021c7e1,0x07dec359}, .iir_coef[0].coef_a = {0x08000000,0xf021c7e1,0x07dec359},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
		.dac_gain_offset=0,
		.adc_gain_offset=-24,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
        .iir_counter = 1,
        
        .iir_coef[0].coef_b = {0x08000000,0xf021c7e1,0x07dec359}, .iir_coef[0].coef_a = {0x08000000,0xf021c7e1,0x07dec359},

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
        .iir_counter = 1,
        
        .iir_coef[0].coef_b = {0x08000000,0xf021c7e1,0x07dec359}, .iir_coef[0].coef_a = {0x08000000,0xf021c7e1,0x07dec359},

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
};

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_48k_mode2 = {
    .anc_cfg_ff_l = {
		.total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xf016b034,0x1fccfd8d,0xf01c4e3a}, .iir_coef[0].coef_a={0x08000000,0xf00fd6eb,0x07f02b18},
		.iir_coef[1].coef_b={0x07ed2852,0xf02de6fe,0x07e501b1}, .iir_coef[1].coef_a={0x08000000,0xf02de6fe,0x07d22a03},
		.iir_coef[2].coef_b={0x080a11a3,0xf0146e19,0x07e1b28d}, .iir_coef[2].coef_a={0x08000000,0xf0146e19,0x07ebc430},
		.iir_coef[3].coef_b={0x083621b0,0xf017f622,0x07b273bf}, .iir_coef[3].coef_a={0x08000000,0xf017f622,0x07e8956e},
		.iir_coef[4].coef_b={0x081b05a7,0xf01ef219,0x07c741c5}, .iir_coef[4].coef_a={0x08000000,0xf01ef219,0x07e2476c},
		.iir_coef[5].coef_b={0x06e5e732,0xf2899cc6,0x06a73705}, .iir_coef[5].coef_a={0x08000000,0xf2899cc6,0x058d1e37},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/

		.dac_gain_offset=0,
		.adc_gain_offset=-24,
    },
    .anc_cfg_ff_r = {
		.total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xf016b034,0x1fccfd8d,0xf01c4e3a}, .iir_coef[0].coef_a={0x08000000,0xf00fd6eb,0x07f02b18},
		.iir_coef[1].coef_b={0x07ed2852,0xf02de6fe,0x07e501b1}, .iir_coef[1].coef_a={0x08000000,0xf02de6fe,0x07d22a03},
		.iir_coef[2].coef_b={0x080a11a3,0xf0146e19,0x07e1b28d}, .iir_coef[2].coef_a={0x08000000,0xf0146e19,0x07ebc430},
		.iir_coef[3].coef_b={0x083621b0,0xf017f622,0x07b273bf}, .iir_coef[3].coef_a={0x08000000,0xf017f622,0x07e8956e},
		.iir_coef[4].coef_b={0x081b05a7,0xf01ef219,0x07c741c5}, .iir_coef[4].coef_a={0x08000000,0xf01ef219,0x07e2476c},
		.iir_coef[5].coef_b={0x06e5e732,0xf2899cc6,0x06a73705}, .iir_coef[5].coef_a={0x08000000,0xf2899cc6,0x058d1e37},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/

		.dac_gain_offset=0,
		.adc_gain_offset=-24,
    },
    .anc_cfg_fb_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=2, 
		
		.iir_coef[0].coef_b={0xf1c17a03,0x1c6ec162,0xf1cfc21d}, .iir_coef[0].coef_a={0x08000000,0xf002dae4,0x07fd2683},
		.iir_coef[1].coef_b={0x0806475b,0xf0128bb5,0x07e76796}, .iir_coef[1].coef_a={0x08000000,0xf01278c9,0x07ed9c06},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
		.dac_gain_offset=0,
		.adc_gain_offset=-24,
    },
    .anc_cfg_fb_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=2, 
		
		.iir_coef[0].coef_b={0xf1c17a03,0x1c6ec162,0xf1cfc21d}, .iir_coef[0].coef_a={0x08000000,0xf002dae4,0x07fd2683},
		.iir_coef[1].coef_b={0x0806475b,0xf0128bb5,0x07e76796}, .iir_coef[1].coef_a={0x08000000,0xf01278c9,0x07ed9c06},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
		.dac_gain_offset=0,
		.adc_gain_offset=-24,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1, 
		
		.iir_coef[0].coef_b={0x016c6e70,0xfd281965,0x016b7854}, .iir_coef[0].coef_a={0x08000000,0xf002b685,0x07fd4a61},

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1, 
		
		.iir_coef[0].coef_b={0x016c6e70,0xfd281965,0x016b7854}, .iir_coef[0].coef_a={0x08000000,0xf002b685,0x07fd4a61},

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
};

static struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_48k_mode3 = {
    .anc_cfg_ff_l = {
		.total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffb201,0xffff6402,0xffffb201}, .iir_coef[0].coef_a={0x08000000,0xf032deb0,0x07ce594c},
		.iir_coef[1].coef_b={0x08018301,0xf001aa02,0x07fcd356}, .iir_coef[1].coef_a={0x08000000,0xf001aa02,0x07fe5658},
		.iir_coef[2].coef_b={0x080313b3,0xf006fc70,0x07f5f578}, .iir_coef[2].coef_a={0x08000000,0xf006fc70,0x07f9092b},
		.iir_coef[3].coef_b={0x0801bdbc,0xf0087d8b,0x07f5d154}, .iir_coef[3].coef_a={0x08000000,0xf0087d8b,0x07f78f10},
		.iir_coef[4].coef_b={0x08031b0d,0xf00ab504,0x07f24655}, .iir_coef[4].coef_a={0x08000000,0xf00ab504,0x07f56162},
		.iir_coef[5].coef_b={0x07f8755d,0xf022bac5,0x07e51411}, .iir_coef[5].coef_a={0x08000000,0xf022bac5,0x07dd896e},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_ff_r = {
		.total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffb201,0xffff6402,0xffffb201}, .iir_coef[0].coef_a={0x08000000,0xf032deb0,0x07ce594c},
		.iir_coef[1].coef_b={0x08018301,0xf001aa02,0x07fcd356}, .iir_coef[1].coef_a={0x08000000,0xf001aa02,0x07fe5658},
		.iir_coef[2].coef_b={0x080313b3,0xf006fc70,0x07f5f578}, .iir_coef[2].coef_a={0x08000000,0xf006fc70,0x07f9092b},
		.iir_coef[3].coef_b={0x0801bdbc,0xf0087d8b,0x07f5d154}, .iir_coef[3].coef_a={0x08000000,0xf0087d8b,0x07f78f10},
		.iir_coef[4].coef_b={0x08031b0d,0xf00ab504,0x07f24655}, .iir_coef[4].coef_a={0x08000000,0xf00ab504,0x07f56162},
		.iir_coef[5].coef_b={0x07f8755d,0xf022bac5,0x07e51411}, .iir_coef[5].coef_a={0x08000000,0xf022bac5,0x07dd896e},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_fb_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7,
		
		.iir_coef[0].coef_b={0xf80671de,0x0fd3f62d,0xf82520ff}, .iir_coef[0].coef_a={0x08000000,0xf02c4506,0x07d4a856},
		.iir_coef[1].coef_b={0x08014bbd,0xf0016d39,0x07fd4764}, .iir_coef[1].coef_a={0x08000000,0xf0016d39,0x07fe9321},
		.iir_coef[2].coef_b={0x0802973f,0xf002dae4,0x07fa8f45}, .iir_coef[2].coef_a={0x08000000,0xf002dae4,0x07fd2683},
		.iir_coef[3].coef_b={0x080a576f,0xf00b7640,0x07ea48b6}, .iir_coef[3].coef_a={0x08000000,0xf00b7640,0x07f4a025},
		.iir_coef[4].coef_b={0x08056fbf,0xf00600ef,0x07f49581}, .iir_coef[4].coef_a={0x08000000,0xf00600ef,0x07fa0540},
		.iir_coef[5].coef_b={0x079b8cd5,0xf1499b3a,0x072d1147}, .iir_coef[5].coef_a={0x08000000,0xf1499b3a,0x06c89e1c},
		.iir_coef[6].coef_b={0x08010629,0xf00304a7,0x07fbf657}, .iir_coef[6].coef_a={0x08000000,0xf0030448,0x07fcfc21},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_fb_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7,
		
		.iir_coef[0].coef_b={0xf80671de,0x0fd3f62d,0xf82520ff}, .iir_coef[0].coef_a={0x08000000,0xf02c4506,0x07d4a856},
		.iir_coef[1].coef_b={0x08014bbd,0xf0016d39,0x07fd4764}, .iir_coef[1].coef_a={0x08000000,0xf0016d39,0x07fe9321},
		.iir_coef[2].coef_b={0x0802973f,0xf002dae4,0x07fa8f45}, .iir_coef[2].coef_a={0x08000000,0xf002dae4,0x07fd2683},
		.iir_coef[3].coef_b={0x080a576f,0xf00b7640,0x07ea48b6}, .iir_coef[3].coef_a={0x08000000,0xf00b7640,0x07f4a025},
		.iir_coef[4].coef_b={0x08056fbf,0xf00600ef,0x07f49581}, .iir_coef[4].coef_a={0x08000000,0xf00600ef,0x07fa0540},
		.iir_coef[5].coef_b={0x079b8cd5,0xf1499b3a,0x072d1147}, .iir_coef[5].coef_a={0x08000000,0xf1499b3a,0x06c89e1c},
		.iir_coef[6].coef_b={0x08010629,0xf00304a7,0x07fbf657}, .iir_coef[6].coef_a={0x08000000,0xf0030448,0x07fcfc21},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1,
		
		.iir_coef[0].coef_b={0xfe938757,0x02d7d210,0xfe94a66f}, .iir_coef[0].coef_a={0x08000000,0xf0032a09,0x07fcd6dd},

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1,
		
		.iir_coef[0].coef_b={0xfe938757,0x02d7d210,0xfe94a66f}, .iir_coef[0].coef_a={0x08000000,0xf0032a09,0x07fcd6dd},

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
};

static struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_48k_mode4 = {
    .anc_cfg_ff_l = {
		.total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffb201,0xffff6402,0xffffb201}, .iir_coef[0].coef_a={0x08000000,0xf032deb0,0x07ce594c},
		.iir_coef[1].coef_b={0x08018301,0xf001aa02,0x07fcd356}, .iir_coef[1].coef_a={0x08000000,0xf001aa02,0x07fe5658},
		.iir_coef[2].coef_b={0x080313b3,0xf006fc70,0x07f5f578}, .iir_coef[2].coef_a={0x08000000,0xf006fc70,0x07f9092b},
		.iir_coef[3].coef_b={0x0801bdbc,0xf0087d8b,0x07f5d154}, .iir_coef[3].coef_a={0x08000000,0xf0087d8b,0x07f78f10},
		.iir_coef[4].coef_b={0x08031b0d,0xf00ab504,0x07f24655}, .iir_coef[4].coef_a={0x08000000,0xf00ab504,0x07f56162},
		.iir_coef[5].coef_b={0x07f8755d,0xf022bac5,0x07e51411}, .iir_coef[5].coef_a={0x08000000,0xf022bac5,0x07dd896e},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_ff_r = {
		.total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffb201,0xffff6402,0xffffb201}, .iir_coef[0].coef_a={0x08000000,0xf032deb0,0x07ce594c},
		.iir_coef[1].coef_b={0x08018301,0xf001aa02,0x07fcd356}, .iir_coef[1].coef_a={0x08000000,0xf001aa02,0x07fe5658},
		.iir_coef[2].coef_b={0x080313b3,0xf006fc70,0x07f5f578}, .iir_coef[2].coef_a={0x08000000,0xf006fc70,0x07f9092b},
		.iir_coef[3].coef_b={0x0801bdbc,0xf0087d8b,0x07f5d154}, .iir_coef[3].coef_a={0x08000000,0xf0087d8b,0x07f78f10},
		.iir_coef[4].coef_b={0x08031b0d,0xf00ab504,0x07f24655}, .iir_coef[4].coef_a={0x08000000,0xf00ab504,0x07f56162},
		.iir_coef[5].coef_b={0x07f8755d,0xf022bac5,0x07e51411}, .iir_coef[5].coef_a={0x08000000,0xf022bac5,0x07dd896e},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_fb_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7,
		
		.iir_coef[0].coef_b={0xf80671de,0x0fd3f62d,0xf82520ff}, .iir_coef[0].coef_a={0x08000000,0xf02c4506,0x07d4a856},
		.iir_coef[1].coef_b={0x08014bbd,0xf0016d39,0x07fd4764}, .iir_coef[1].coef_a={0x08000000,0xf0016d39,0x07fe9321},
		.iir_coef[2].coef_b={0x0802973f,0xf002dae4,0x07fa8f45}, .iir_coef[2].coef_a={0x08000000,0xf002dae4,0x07fd2683},
		.iir_coef[3].coef_b={0x080a576f,0xf00b7640,0x07ea48b6}, .iir_coef[3].coef_a={0x08000000,0xf00b7640,0x07f4a025},
		.iir_coef[4].coef_b={0x08056fbf,0xf00600ef,0x07f49581}, .iir_coef[4].coef_a={0x08000000,0xf00600ef,0x07fa0540},
		.iir_coef[5].coef_b={0x079b8cd5,0xf1499b3a,0x072d1147}, .iir_coef[5].coef_a={0x08000000,0xf1499b3a,0x06c89e1c},
		.iir_coef[6].coef_b={0x08010629,0xf00304a7,0x07fbf657}, .iir_coef[6].coef_a={0x08000000,0xf0030448,0x07fcfc21},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_fb_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7,
		
		.iir_coef[0].coef_b={0xf80671de,0x0fd3f62d,0xf82520ff}, .iir_coef[0].coef_a={0x08000000,0xf02c4506,0x07d4a856},
		.iir_coef[1].coef_b={0x08014bbd,0xf0016d39,0x07fd4764}, .iir_coef[1].coef_a={0x08000000,0xf0016d39,0x07fe9321},
		.iir_coef[2].coef_b={0x0802973f,0xf002dae4,0x07fa8f45}, .iir_coef[2].coef_a={0x08000000,0xf002dae4,0x07fd2683},
		.iir_coef[3].coef_b={0x080a576f,0xf00b7640,0x07ea48b6}, .iir_coef[3].coef_a={0x08000000,0xf00b7640,0x07f4a025},
		.iir_coef[4].coef_b={0x08056fbf,0xf00600ef,0x07f49581}, .iir_coef[4].coef_a={0x08000000,0xf00600ef,0x07fa0540},
		.iir_coef[5].coef_b={0x079b8cd5,0xf1499b3a,0x072d1147}, .iir_coef[5].coef_a={0x08000000,0xf1499b3a,0x06c89e1c},
		.iir_coef[6].coef_b={0x08010629,0xf00304a7,0x07fbf657}, .iir_coef[6].coef_a={0x08000000,0xf0030448,0x07fcfc21},

/*		.fir_bypass_flag=1,
        .fir_len = AUD_COEF_LEN,
        .fir_coef =
        {
            32767,
        },
*/
		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1,
		
		.iir_coef[0].coef_b={0xfe938757,0x02d7d210,0xfe94a66f}, .iir_coef[0].coef_a={0x08000000,0xf0032a09,0x07fcd6dd},

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1,
		
		.iir_coef[0].coef_b={0xfe938757,0x02d7d210,0xfe94a66f}, .iir_coef[0].coef_a={0x08000000,0xf0032a09,0x07fcd6dd},

		.dac_gain_offset=0,
		.adc_gain_offset=0,
    },
};
/* End Add by lewis */

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_44p1k_mode0 = {
    .anc_cfg_ff_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffa3b3,0xffff4766,0xffffa3b3}, .iir_coef[0].coef_a={0x08000000,0xf0376ceb,0x07ca044a},
		.iir_coef[1].coef_b={0x0801a537,0xf001cfb3,0x07fc8b80}, .iir_coef[1].coef_a={0x08000000,0xf001cfb3,0x07fe30b7},
		.iir_coef[2].coef_b={0x0803593b,0xf0079ad9,0x07f51290}, .iir_coef[2].coef_a={0x08000000,0xf0079ad9,0x07f86bcb},
		.iir_coef[3].coef_b={0x0801e510,0xf0093e8b,0x07f4eb53}, .iir_coef[3].coef_a={0x08000000,0xf0093e8b,0x07f6d064},
		.iir_coef[4].coef_b={0x08036129,0xf00ba8e3,0x07f1107b}, .iir_coef[4].coef_a={0x08000000,0xf00ba8e3,0x07f471a5},
		.iir_coef[5].coef_b={0x07f7cc33,0xf025cc5b,0x07e2b82f}, .iir_coef[5].coef_a={0x08000000,0xf025cc5b,0x07da8462},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_ff_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffa3b3,0xffff4766,0xffffa3b3}, .iir_coef[0].coef_a={0x08000000,0xf0376ceb,0x07ca044a},
		.iir_coef[1].coef_b={0x0801a537,0xf001cfb3,0x07fc8b80}, .iir_coef[1].coef_a={0x08000000,0xf001cfb3,0x07fe30b7},
		.iir_coef[2].coef_b={0x0803593b,0xf0079ad9,0x07f51290}, .iir_coef[2].coef_a={0x08000000,0xf0079ad9,0x07f86bcb},
		.iir_coef[3].coef_b={0x0801e510,0xf0093e8b,0x07f4eb53}, .iir_coef[3].coef_a={0x08000000,0xf0093e8b,0x07f6d064},
		.iir_coef[4].coef_b={0x08036129,0xf00ba8e3,0x07f1107b}, .iir_coef[4].coef_a={0x08000000,0xf00ba8e3,0x07f471a5},
		.iir_coef[5].coef_b={0x07f7cc33,0xf025cc5b,0x07e2b82f}, .iir_coef[5].coef_a={0x08000000,0xf025cc5b,0x07da8462},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_fb_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7, 
		
		.iir_coef[0].coef_b={0xf80704ea,0x0fd00bb5,0xf8286295}, .iir_coef[0].coef_a={0x08000000,0xf0303a5c,0x07d0de92},
		.iir_coef[1].coef_b={0x08016910,0xf0018d8b,0x07fd09cf}, .iir_coef[1].coef_a={0x08000000,0xf0018d8b,0x07fe72e0},
		.iir_coef[2].coef_b={0x0802d1db,0xf0031b9d,0x07fa1432}, .iir_coef[2].coef_a={0x08000000,0xf0031b9d,0x07fce60c},
		.iir_coef[3].coef_b={0x080b40d9,0xf00c7b1c,0x07e85e91}, .iir_coef[3].coef_a={0x08000000,0xf00c7b1c,0x07f39f6b},
		.iir_coef[4].coef_b={0x0805eaa1,0xf006893b,0x07f39377}, .iir_coef[4].coef_a={0x08000000,0xf006893b,0x07f97e18},
		.iir_coef[5].coef_b={0x07936d31,0xf16601eb,0x071c0249}, .iir_coef[5].coef_a={0x08000000,0xf16601eb,0x06af6f7a},
		.iir_coef[6].coef_b={0x08011d58,0xf0034901,0x07fb9b05}, .iir_coef[6].coef_a={0x08000000,0xf0034890,0x07fcb7ec},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_fb_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7, 
		
		.iir_coef[0].coef_b={0xf80704ea,0x0fd00bb5,0xf8286295}, .iir_coef[0].coef_a={0x08000000,0xf0303a5c,0x07d0de92},
		.iir_coef[1].coef_b={0x08016910,0xf0018d8b,0x07fd09cf}, .iir_coef[1].coef_a={0x08000000,0xf0018d8b,0x07fe72e0},
		.iir_coef[2].coef_b={0x0802d1db,0xf0031b9d,0x07fa1432}, .iir_coef[2].coef_a={0x08000000,0xf0031b9d,0x07fce60c},
		.iir_coef[3].coef_b={0x080b40d9,0xf00c7b1c,0x07e85e91}, .iir_coef[3].coef_a={0x08000000,0xf00c7b1c,0x07f39f6b},
		.iir_coef[4].coef_b={0x0805eaa1,0xf006893b,0x07f39377}, .iir_coef[4].coef_a={0x08000000,0xf006893b,0x07f97e18},
		.iir_coef[5].coef_b={0x07936d31,0xf16601eb,0x071c0249}, .iir_coef[5].coef_a={0x08000000,0xf16601eb,0x06af6f7a},
		.iir_coef[6].coef_b={0x08011d58,0xf0034901,0x07fb9b05}, .iir_coef[6].coef_a={0x08000000,0xf0034890,0x07fcb7ec},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1, 
		
		.iir_coef[0].coef_b={0xfe938104,0x02d7c552,0xfe94b97a}, .iir_coef[0].coef_a={0x08000000,0xf00371b2,0x07fc8f5e},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1, 
		
		.iir_coef[0].coef_b={0xfe938104,0x02d7c552,0xfe94b97a}, .iir_coef[0].coef_a={0x08000000,0xf00371b2,0x07fc8f5e},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
};

/* Add by lewis */
static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_44p1k_mode1 = {
    .anc_cfg_ff_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xf017d4f4,0x1fca34ad,0xf01df19d}, .iir_coef[0].coef_a={0x08000000,0xf0113c34,0x07eec62f},
		.iir_coef[1].coef_b={0x07eb82ef,0xf031eb2c,0x07e2a605}, .iir_coef[1].coef_a={0x08000000,0xf031eb2c,0x07ce28f4},
		.iir_coef[2].coef_b={0x0810dab6,0xf012c540,0x07dc9ba3}, .iir_coef[2].coef_a={0x08000000,0xf012c540,0x07ed7658},
		.iir_coef[3].coef_b={0x083dc662,0xf024a4cc,0x079e9661}, .iir_coef[3].coef_a={0x08000000,0xf024a4cc,0x07dc5cc3},
		.iir_coef[4].coef_b={0x0836004e,0xf026cdf0,0x07a5c4f9}, .iir_coef[4].coef_a={0x08000000,0xf026cdf0,0x07dbc547},
		.iir_coef[5].coef_b={0x06d1288b,0xf2bb8bf8,0x068ddc38}, .iir_coef[5].coef_a={0x08000000,0xf2bb8bf8,0x055f04c3},

        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_ff_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xf017d4f4,0x1fca34ad,0xf01df19d}, .iir_coef[0].coef_a={0x08000000,0xf0113c34,0x07eec62f},
		.iir_coef[1].coef_b={0x07eb82ef,0xf031eb2c,0x07e2a605}, .iir_coef[1].coef_a={0x08000000,0xf031eb2c,0x07ce28f4},
		.iir_coef[2].coef_b={0x0810dab6,0xf012c540,0x07dc9ba3}, .iir_coef[2].coef_a={0x08000000,0xf012c540,0x07ed7658},
		.iir_coef[3].coef_b={0x083dc662,0xf024a4cc,0x079e9661}, .iir_coef[3].coef_a={0x08000000,0xf024a4cc,0x07dc5cc3},
		.iir_coef[4].coef_b={0x0836004e,0xf026cdf0,0x07a5c4f9}, .iir_coef[4].coef_a={0x08000000,0xf026cdf0,0x07dbc547},
		.iir_coef[5].coef_b={0x06d1288b,0xf2bb8bf8,0x068ddc38}, .iir_coef[5].coef_a={0x08000000,0xf2bb8bf8,0x055f04c3},

        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_fb_l = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
        .iir_counter = 1,
        
        .iir_coef[0].coef_b = {0x08000000,0xf024cb39,0x07dbd999}, .iir_coef[0].coef_a = {0x08000000,0xf024cb39,0x07dbd999},

        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_fb_r = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
        .iir_counter = 1,
        
        .iir_coef[0].coef_b = {0x08000000,0xf024cb39,0x07dbd999}, .iir_coef[0].coef_a = {0x08000000,0xf024cb39,0x07dbd999},

        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
        .iir_counter = 1,
        
        .iir_coef[0].coef_b = {0x08000000,0xf024cb39,0x07dbd999}, .iir_coef[0].coef_a = {0x08000000,0xf024cb39,0x07dbd999},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 0,
			
        .iir_bypass_flag = 0,
        .iir_counter = 1,
        
        .iir_coef[0].coef_b = {0x08000000,0xf024cb39,0x07dbd999}, .iir_coef[0].coef_a = {0x08000000,0xf024cb39,0x07dbd999},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
};

static const struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_44p1k_mode2 = {
    .anc_cfg_ff_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xf017d4f4,0x1fca34ad,0xf01df19d}, .iir_coef[0].coef_a={0x08000000,0xf0113c34,0x07eec62f},
		.iir_coef[1].coef_b={0x07eb82ef,0xf031eb2c,0x07e2a605}, .iir_coef[1].coef_a={0x08000000,0xf031eb2c,0x07ce28f4},
		.iir_coef[2].coef_b={0x080af45c,0xf0163ef5,0x07df083b}, .iir_coef[2].coef_a={0x08000000,0xf0163ef5,0x07e9fc96},
		.iir_coef[3].coef_b={0x083ae373,0xf01a1e9b,0x07aba333}, .iir_coef[3].coef_a={0x08000000,0xf01a1e9b,0x07e686a6},
		.iir_coef[4].coef_b={0x081d6474,0xf021c72a,0x07c24791}, .iir_coef[4].coef_a={0x08000000,0xf021c72a,0x07dfac05},
		.iir_coef[5].coef_b={0x06d1288b,0xf2bb8bf8,0x068ddc38}, .iir_coef[5].coef_a={0x08000000,0xf2bb8bf8,0x055f04c3},

        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_ff_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xf017d4f4,0x1fca34ad,0xf01df19d}, .iir_coef[0].coef_a={0x08000000,0xf0113c34,0x07eec62f},
		.iir_coef[1].coef_b={0x07eb82ef,0xf031eb2c,0x07e2a605}, .iir_coef[1].coef_a={0x08000000,0xf031eb2c,0x07ce28f4},
		.iir_coef[2].coef_b={0x080af45c,0xf0163ef5,0x07df083b}, .iir_coef[2].coef_a={0x08000000,0xf0163ef5,0x07e9fc96},
		.iir_coef[3].coef_b={0x083ae373,0xf01a1e9b,0x07aba333}, .iir_coef[3].coef_a={0x08000000,0xf01a1e9b,0x07e686a6},
		.iir_coef[4].coef_b={0x081d6474,0xf021c72a,0x07c24791}, .iir_coef[4].coef_a={0x08000000,0xf021c72a,0x07dfac05},
		.iir_coef[5].coef_b={0x06d1288b,0xf2bb8bf8,0x068ddc38}, .iir_coef[5].coef_a={0x08000000,0xf2bb8bf8,0x055f04c3},

        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_fb_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=2, 
		
		.iir_coef[0].coef_b={0xf1c111c9,0x1c6e4e4a,0xf1d09cf8}, .iir_coef[0].coef_a={0x08000000,0xf0031b9d,0x07fce60c},
		.iir_coef[1].coef_b={0x0806d5b8,0xf0143156,0x07e53e65}, .iir_coef[1].coef_a={0x08000000,0xf0141aef,0x07ebfdb6},

        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_fb_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=2, 
		
		.iir_coef[0].coef_b={0xf1c111c9,0x1c6e4e4a,0xf1d09cf8}, .iir_coef[0].coef_a={0x08000000,0xf0031b9d,0x07fce60c},
		.iir_coef[1].coef_b={0x0806d5b8,0xf0143156,0x07e53e65}, .iir_coef[1].coef_a={0x08000000,0xf0141aef,0x07ebfdb6},

        .dac_gain_offset = 0,
        .adc_gain_offset = -24,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1, 
		
		.iir_coef[0].coef_b={0x016c73dc,0xfd282453,0x016b6801}, .iir_coef[0].coef_a={0x08000000,0xf002f3fb,0x07fd0d15},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1, 
		
		.iir_coef[0].coef_b={0x016c73dc,0xfd282453,0x016b6801}, .iir_coef[0].coef_a={0x08000000,0xf002f3fb,0x07fd0d15},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
};

static struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_44p1k_mode3 = {
    .anc_cfg_ff_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffa3b3,0xffff4766,0xffffa3b3}, .iir_coef[0].coef_a={0x08000000,0xf0376ceb,0x07ca044a},
		.iir_coef[1].coef_b={0x0801a537,0xf001cfb3,0x07fc8b80}, .iir_coef[1].coef_a={0x08000000,0xf001cfb3,0x07fe30b7},
		.iir_coef[2].coef_b={0x0803593b,0xf0079ad9,0x07f51290}, .iir_coef[2].coef_a={0x08000000,0xf0079ad9,0x07f86bcb},
		.iir_coef[3].coef_b={0x0801e510,0xf0093e8b,0x07f4eb53}, .iir_coef[3].coef_a={0x08000000,0xf0093e8b,0x07f6d064},
		.iir_coef[4].coef_b={0x08036129,0xf00ba8e3,0x07f1107b}, .iir_coef[4].coef_a={0x08000000,0xf00ba8e3,0x07f471a5},
		.iir_coef[5].coef_b={0x07f7cc33,0xf025cc5b,0x07e2b82f}, .iir_coef[5].coef_a={0x08000000,0xf025cc5b,0x07da8462},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_ff_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffa3b3,0xffff4766,0xffffa3b3}, .iir_coef[0].coef_a={0x08000000,0xf0376ceb,0x07ca044a},
		.iir_coef[1].coef_b={0x0801a537,0xf001cfb3,0x07fc8b80}, .iir_coef[1].coef_a={0x08000000,0xf001cfb3,0x07fe30b7},
		.iir_coef[2].coef_b={0x0803593b,0xf0079ad9,0x07f51290}, .iir_coef[2].coef_a={0x08000000,0xf0079ad9,0x07f86bcb},
		.iir_coef[3].coef_b={0x0801e510,0xf0093e8b,0x07f4eb53}, .iir_coef[3].coef_a={0x08000000,0xf0093e8b,0x07f6d064},
		.iir_coef[4].coef_b={0x08036129,0xf00ba8e3,0x07f1107b}, .iir_coef[4].coef_a={0x08000000,0xf00ba8e3,0x07f471a5},
		.iir_coef[5].coef_b={0x07f7cc33,0xf025cc5b,0x07e2b82f}, .iir_coef[5].coef_a={0x08000000,0xf025cc5b,0x07da8462},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_fb_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7, 
		
		.iir_coef[0].coef_b={0xf80704ea,0x0fd00bb5,0xf8286295}, .iir_coef[0].coef_a={0x08000000,0xf0303a5c,0x07d0de92},
		.iir_coef[1].coef_b={0x08016910,0xf0018d8b,0x07fd09cf}, .iir_coef[1].coef_a={0x08000000,0xf0018d8b,0x07fe72e0},
		.iir_coef[2].coef_b={0x0802d1db,0xf0031b9d,0x07fa1432}, .iir_coef[2].coef_a={0x08000000,0xf0031b9d,0x07fce60c},
		.iir_coef[3].coef_b={0x080b40d9,0xf00c7b1c,0x07e85e91}, .iir_coef[3].coef_a={0x08000000,0xf00c7b1c,0x07f39f6b},
		.iir_coef[4].coef_b={0x0805eaa1,0xf006893b,0x07f39377}, .iir_coef[4].coef_a={0x08000000,0xf006893b,0x07f97e18},
		.iir_coef[5].coef_b={0x07936d31,0xf16601eb,0x071c0249}, .iir_coef[5].coef_a={0x08000000,0xf16601eb,0x06af6f7a},
		.iir_coef[6].coef_b={0x08011d58,0xf0034901,0x07fb9b05}, .iir_coef[6].coef_a={0x08000000,0xf0034890,0x07fcb7ec},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_fb_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7, 
		
		.iir_coef[0].coef_b={0xf80704ea,0x0fd00bb5,0xf8286295}, .iir_coef[0].coef_a={0x08000000,0xf0303a5c,0x07d0de92},
		.iir_coef[1].coef_b={0x08016910,0xf0018d8b,0x07fd09cf}, .iir_coef[1].coef_a={0x08000000,0xf0018d8b,0x07fe72e0},
		.iir_coef[2].coef_b={0x0802d1db,0xf0031b9d,0x07fa1432}, .iir_coef[2].coef_a={0x08000000,0xf0031b9d,0x07fce60c},
		.iir_coef[3].coef_b={0x080b40d9,0xf00c7b1c,0x07e85e91}, .iir_coef[3].coef_a={0x08000000,0xf00c7b1c,0x07f39f6b},
		.iir_coef[4].coef_b={0x0805eaa1,0xf006893b,0x07f39377}, .iir_coef[4].coef_a={0x08000000,0xf006893b,0x07f97e18},
		.iir_coef[5].coef_b={0x07936d31,0xf16601eb,0x071c0249}, .iir_coef[5].coef_a={0x08000000,0xf16601eb,0x06af6f7a},
		.iir_coef[6].coef_b={0x08011d58,0xf0034901,0x07fb9b05}, .iir_coef[6].coef_a={0x08000000,0xf0034890,0x07fcb7ec},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1, 
		
		.iir_coef[0].coef_b={0xfe938104,0x02d7c552,0xfe94b97a}, .iir_coef[0].coef_a={0x08000000,0xf00371b2,0x07fc8f5e},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1, 
		
		.iir_coef[0].coef_b={0xfe938104,0x02d7c552,0xfe94b97a}, .iir_coef[0].coef_a={0x08000000,0xf00371b2,0x07fc8f5e},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
};

static struct_anc_cfg POSSIBLY_UNUSED AncFirCoef_44p1k_mode4 = {
    .anc_cfg_ff_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffa3b3,0xffff4766,0xffffa3b3}, .iir_coef[0].coef_a={0x08000000,0xf0376ceb,0x07ca044a},
		.iir_coef[1].coef_b={0x0801a537,0xf001cfb3,0x07fc8b80}, .iir_coef[1].coef_a={0x08000000,0xf001cfb3,0x07fe30b7},
		.iir_coef[2].coef_b={0x0803593b,0xf0079ad9,0x07f51290}, .iir_coef[2].coef_a={0x08000000,0xf0079ad9,0x07f86bcb},
		.iir_coef[3].coef_b={0x0801e510,0xf0093e8b,0x07f4eb53}, .iir_coef[3].coef_a={0x08000000,0xf0093e8b,0x07f6d064},
		.iir_coef[4].coef_b={0x08036129,0xf00ba8e3,0x07f1107b}, .iir_coef[4].coef_a={0x08000000,0xf00ba8e3,0x07f471a5},
		.iir_coef[5].coef_b={0x07f7cc33,0xf025cc5b,0x07e2b82f}, .iir_coef[5].coef_a={0x08000000,0xf025cc5b,0x07da8462},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_ff_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=6, 
		
		.iir_coef[0].coef_b={0xffffa3b3,0xffff4766,0xffffa3b3}, .iir_coef[0].coef_a={0x08000000,0xf0376ceb,0x07ca044a},
		.iir_coef[1].coef_b={0x0801a537,0xf001cfb3,0x07fc8b80}, .iir_coef[1].coef_a={0x08000000,0xf001cfb3,0x07fe30b7},
		.iir_coef[2].coef_b={0x0803593b,0xf0079ad9,0x07f51290}, .iir_coef[2].coef_a={0x08000000,0xf0079ad9,0x07f86bcb},
		.iir_coef[3].coef_b={0x0801e510,0xf0093e8b,0x07f4eb53}, .iir_coef[3].coef_a={0x08000000,0xf0093e8b,0x07f6d064},
		.iir_coef[4].coef_b={0x08036129,0xf00ba8e3,0x07f1107b}, .iir_coef[4].coef_a={0x08000000,0xf00ba8e3,0x07f471a5},
		.iir_coef[5].coef_b={0x07f7cc33,0xf025cc5b,0x07e2b82f}, .iir_coef[5].coef_a={0x08000000,0xf025cc5b,0x07da8462},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_fb_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7, 
		
		.iir_coef[0].coef_b={0xf80704ea,0x0fd00bb5,0xf8286295}, .iir_coef[0].coef_a={0x08000000,0xf0303a5c,0x07d0de92},
		.iir_coef[1].coef_b={0x08016910,0xf0018d8b,0x07fd09cf}, .iir_coef[1].coef_a={0x08000000,0xf0018d8b,0x07fe72e0},
		.iir_coef[2].coef_b={0x0802d1db,0xf0031b9d,0x07fa1432}, .iir_coef[2].coef_a={0x08000000,0xf0031b9d,0x07fce60c},
		.iir_coef[3].coef_b={0x080b40d9,0xf00c7b1c,0x07e85e91}, .iir_coef[3].coef_a={0x08000000,0xf00c7b1c,0x07f39f6b},
		.iir_coef[4].coef_b={0x0805eaa1,0xf006893b,0x07f39377}, .iir_coef[4].coef_a={0x08000000,0xf006893b,0x07f97e18},
		.iir_coef[5].coef_b={0x07936d31,0xf16601eb,0x071c0249}, .iir_coef[5].coef_a={0x08000000,0xf16601eb,0x06af6f7a},
		.iir_coef[6].coef_b={0x08011d58,0xf0034901,0x07fb9b05}, .iir_coef[6].coef_a={0x08000000,0xf0034890,0x07fcb7ec},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_fb_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=7, 
		
		.iir_coef[0].coef_b={0xf80704ea,0x0fd00bb5,0xf8286295}, .iir_coef[0].coef_a={0x08000000,0xf0303a5c,0x07d0de92},
		.iir_coef[1].coef_b={0x08016910,0xf0018d8b,0x07fd09cf}, .iir_coef[1].coef_a={0x08000000,0xf0018d8b,0x07fe72e0},
		.iir_coef[2].coef_b={0x0802d1db,0xf0031b9d,0x07fa1432}, .iir_coef[2].coef_a={0x08000000,0xf0031b9d,0x07fce60c},
		.iir_coef[3].coef_b={0x080b40d9,0xf00c7b1c,0x07e85e91}, .iir_coef[3].coef_a={0x08000000,0xf00c7b1c,0x07f39f6b},
		.iir_coef[4].coef_b={0x0805eaa1,0xf006893b,0x07f39377}, .iir_coef[4].coef_a={0x08000000,0xf006893b,0x07f97e18},
		.iir_coef[5].coef_b={0x07936d31,0xf16601eb,0x071c0249}, .iir_coef[5].coef_a={0x08000000,0xf16601eb,0x06af6f7a},
		.iir_coef[6].coef_b={0x08011d58,0xf0034901,0x07fb9b05}, .iir_coef[6].coef_a={0x08000000,0xf0034890,0x07fcb7ec},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_tt_l = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_L,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},
		// .iir_coef[2].coef_b={130003803,    -257605108,    128180940}, //4k test code
		// .iir_coef[2].coef_a={134217728,    -257605108,    123967016},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0}, 
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_tt_r = {
        .total_gain = 0,

		.iir_bypass_flag=0,
		.iir_counter=IIR_COUNTER_FF_R,

		.iir_coef[0].coef_b={0x8000000,0,0},
		.iir_coef[0].coef_a={0x8000000,0,0},

		.iir_coef[1].coef_b={0x8000000,0,0},
		.iir_coef[1].coef_a={0x8000000,0,0},

		.iir_coef[2].coef_b={0x8000000,0,0},
		.iir_coef[2].coef_a={0x8000000,0,0},

		.iir_coef[3].coef_b={0x8000000,0,0},
		.iir_coef[3].coef_a={0x8000000,0,0},

		.iir_coef[4].coef_b={0x8000000,0,0},  
		.iir_coef[4].coef_a={0x8000000,0,0},

		.iir_coef[5].coef_b={0x8000000,0,0},
		.iir_coef[5].coef_a={0x8000000,0,0},

		.dac_gain_offset=0,
		.adc_gain_offset=(0)*4,
    },
    .anc_cfg_mc_l = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1, 
		
		.iir_coef[0].coef_b={0xfe938104,0x02d7c552,0xfe94b97a}, .iir_coef[0].coef_a={0x08000000,0xf00371b2,0x07fc8f5e},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
    .anc_cfg_mc_r = {
        .total_gain = 512,
			
        .iir_bypass_flag = 0,
		.iir_counter=1, 
		
		.iir_coef[0].coef_b={0xfe938104,0x02d7c552,0xfe94b97a}, .iir_coef[0].coef_a={0x08000000,0xf00371b2,0x07fc8f5e},

        .dac_gain_offset = 0,
        .adc_gain_offset = 0,
    },
};

/* End Add by lewis */

const struct_anc_cfg * anc_coef_list_50p7k[ANC_COEF_LIST_NUM] = {
    &AncFirCoef_50p7k_mode0,
	/* Add by lewis */
    &AncFirCoef_50p7k_mode1,
    &AncFirCoef_50p7k_mode2,
    &AncFirCoef_50p7k_mode3,
    &AncFirCoef_50p7k_mode4,
    /* End Add by lewis */
};

const struct_anc_cfg * anc_coef_list_48k[ANC_COEF_LIST_NUM] = {
    &AncFirCoef_48k_mode0,
	/* Add by lewis */
    &AncFirCoef_48k_mode1,
    &AncFirCoef_48k_mode2,
    &AncFirCoef_48k_mode3,
    &AncFirCoef_48k_mode4,
    /* End Add by lewis */
};

const struct_anc_cfg * anc_coef_list_44p1k[ANC_COEF_LIST_NUM] = {
    &AncFirCoef_44p1k_mode0,
	/* Add by lewis */
    &AncFirCoef_44p1k_mode1,
    &AncFirCoef_44p1k_mode2,
    &AncFirCoef_44p1k_mode3,
    &AncFirCoef_44p1k_mode4,
    /* End Add by lewis */
};

static const struct_psap_cfg POSSIBLY_UNUSED hwlimiter_para_44p1k_mode0 = {
    .psap_cfg_l = {
        .psap_total_gain = 811,
        .psap_band_num = 2,
        .psap_band_gain={0x3f8bd79e, 0x08000000},
        .psap_iir_coef[0].iir0.coef_b={0x04fa1dfc, 0x0c6d6b7e, 0x08000000, 0x00000000},
        .psap_iir_coef[0].iir0.coef_a={0x08000000, 0x0c6d6b7e, 0x04fa1dfc, 0x00000000},
        .psap_iir_coef[0].iir1.coef_b={0x04f9f115, 0x11279bfc, 0x1404d0c8, 0x08000000},
        .psap_iir_coef[0].iir1.coef_a={0x08000000, 0x1404d0c8, 0x11279bfc, 0x04f9f115},
        .psap_cpd_cfg[0]={0x5740, 0x0000, 0x4e02, 0x0000, 0x3b85, 0x0000, 0x7cf7, 0x0309, 0x7ff0, 0x0010, 0x7333, 0x0ccd, 0},
        .psap_limiter_cfg={524287, 0x7cf7, 0x0309, 0x7ff0, 0x0010, 127},
        .psap_dehowling_cfg.dehowling_delay=0,
        .psap_dehowling_cfg.dehowling_l.total_gain=0,
        .psap_dehowling_cfg.dehowling_l.iir_bypass_flag=0,
        .psap_dehowling_cfg.dehowling_l.iir_counter=1,
        .psap_dehowling_cfg.dehowling_l.iir_coef[0].coef_b={0x08000000, 0xf096cb51, 0x07733cd7},
        .psap_dehowling_cfg.dehowling_l.iir_coef[0].coef_a={0x08000000, 0xf096cb51, 0x07733cd7},
        .psap_dehowling_cfg.dehowling_l.dac_gain_offset=0,
        .psap_dehowling_cfg.dehowling_l.adc_gain_offset=0,
        .psap_type = 3,
        .psap_dac_gain_offset=0,
        .psap_adc_gain_offset=0,
    }
};
static const struct_psap_cfg POSSIBLY_UNUSED hwlimiter_para_48k_mode0 = {
    .psap_cfg_l = {
        .psap_total_gain = 811,
        .psap_band_num = 2,
        .psap_band_gain={0x3f8bd79e, 0x08000000},
        .psap_iir_coef[0].iir0.coef_b={0x0364529c, 0x09dd9c0b, 0x08000000, 0x00000000},
        .psap_iir_coef[0].iir0.coef_a={0x08000000, 0x09dd9c0b, 0x0364529c, 0x00000000},
        .psap_iir_coef[0].iir1.coef_b={0x0361ed64, 0x0cc9bec2, 0x109eeca2, 0x08000000},
        .psap_iir_coef[0].iir1.coef_a={0x08000000, 0x109eeca2, 0x0cc9bec2, 0x0361ed64},
        .psap_cpd_cfg[0]={0x5740, 0x0000, 0x4e02, 0x0000, 0x3b85, 0x0000, 0x7cf7, 0x0309, 0x7ff0, 0x0010, 0x7333, 0x0ccd, 0},
        .psap_limiter_cfg={524287, 0x7cf7, 0x0309, 0x7ff0, 0x0010, 127},
        .psap_dehowling_cfg.dehowling_delay=0,
        .psap_dehowling_cfg.dehowling_l.total_gain=0,
        .psap_dehowling_cfg.dehowling_l.iir_bypass_flag=0,
        .psap_dehowling_cfg.dehowling_l.iir_counter=1,
        .psap_dehowling_cfg.dehowling_l.iir_coef[0].coef_b={0x08000000, 0xf08a323c, 0x077e4bc1},
        .psap_dehowling_cfg.dehowling_l.iir_coef[0].coef_a={0x08000000, 0xf08a323c, 0x077e4bc1},
        .psap_dehowling_cfg.dehowling_l.dac_gain_offset=0,
        .psap_dehowling_cfg.dehowling_l.adc_gain_offset=0,
        .psap_type = 3,
        .psap_dac_gain_offset=0,
        .psap_adc_gain_offset=0,
    }
};
static const struct_psap_cfg POSSIBLY_UNUSED hwlimiter_para_50p7k_mode0 = {
    .psap_cfg_l = {
        .psap_total_gain = 811,
        .psap_band_num = 2,
        .psap_band_gain={0x3f8bd79e, 0x08000000},
        .psap_iir_coef[0].iir0.coef_b={0x03c46400, 0x0a8a3bd5, 0x08000000, 0x00000000},
        .psap_iir_coef[0].iir0.coef_a={0x08000000, 0x0a8a3bd5, 0x03c46400, 0x00000000},
        .psap_iir_coef[0].iir1.coef_b={0x03c305c1, 0x0de12c8a, 0x118f7298, 0x08000000},
        .psap_iir_coef[0].iir1.coef_a={0x08000000, 0x118f7298, 0x0de12c8a, 0x03c305c1},
        .psap_cpd_cfg[0]={0x5740, 0x0000, 0x4e02, 0x0000, 0x3b85, 0x0000, 0x7cf7, 0x0309, 0x7ff0, 0x0010, 0x7333, 0x0ccd, 0},
        .psap_limiter_cfg={524287, 0x7cf7, 0x0309, 0x7ff0, 0x0010, 127},
        .psap_dehowling_cfg.dehowling_delay=0,
        .psap_dehowling_cfg.dehowling_l.total_gain=0,
        .psap_dehowling_cfg.dehowling_l.iir_bypass_flag=0,
        .psap_dehowling_cfg.dehowling_l.iir_counter=1,
        .psap_dehowling_cfg.dehowling_l.iir_coef[0].coef_b={0x08000000, 0xf08d9c03, 0x077b49d0},
        .psap_dehowling_cfg.dehowling_l.iir_coef[0].coef_a={0x08000000, 0xf08d9c03, 0x077b49d0},
        .psap_dehowling_cfg.dehowling_l.dac_gain_offset=0,
        .psap_dehowling_cfg.dehowling_l.adc_gain_offset=0,
        .psap_type = 3,
        .psap_dac_gain_offset=0,
        .psap_adc_gain_offset=0,
    }
};

const struct_psap_cfg * hwlimiter_para_list_50p7k[HWLIMITER_PARA_LIST_NUM] = {
    &hwlimiter_para_50p7k_mode0,
};

const struct_psap_cfg * hwlimiter_para_list_48k[HWLIMITER_PARA_LIST_NUM] = {
    &hwlimiter_para_48k_mode0,
};

const struct_psap_cfg * hwlimiter_para_list_44p1k[HWLIMITER_PARA_LIST_NUM] = {
    &hwlimiter_para_44p1k_mode0,
};

static const struct_psap_cfg POSSIBLY_UNUSED PsapFirCoef_50p7k_mode0 = {
    .psap_cfg_l = {
        .psap_total_gain = 723,
        .psap_band_num = 9,
        .psap_band_gain={0, 476222470, 672682118, 534330399, 950188747, 1066129310, 1066129310, 424433723, 0},
        .psap_iir_coef[0].iir0.coef_b={131557370, -265754754, 134217728, 0},
        .psap_iir_coef[0].iir0.coef_a={134217728, -265754754, 131557370, 0},
        .psap_iir_coef[0].iir1.coef_b={-131557370, 397299677, -399959783, 134217728},
        .psap_iir_coef[0].iir1.coef_a={134217728, -399959783, 397299677, -131557370},
        .psap_iir_coef[1].iir0.coef_b={129986488, -264152445, 134217728, 0},
        .psap_iir_coef[1].iir0.coef_a={134217728, -264152445, 129986488, 0},
        .psap_iir_coef[1].iir1.coef_b={-129986488, 394107454, -398337669, 134217728},
        .psap_iir_coef[1].iir1.coef_a={134217728, -398337669, 394107454, -129986488},
        .psap_iir_coef[2].iir0.coef_b={125134130, -259108702, 134217728, 0},
        .psap_iir_coef[2].iir0.coef_a={134217728, -259108702, 125134130, 0},
        .psap_iir_coef[2].iir1.coef_b={-125134129, 384097929, -393171000, 134217728},
        .psap_iir_coef[2].iir1.coef_a={134217728, -393171000, 384097929, -125134129},
        .psap_iir_coef[3].iir0.coef_b={120220377, -253849171, 134217728, 0},
        .psap_iir_coef[3].iir0.coef_a={134217728, -253849171, 120220377, 0},
        .psap_iir_coef[3].iir1.coef_b={-120220366, 373726258, -387683554, 134217728},
        .psap_iir_coef[3].iir1.coef_a={134217728, -387683554, 373726258, -120220366},
        .psap_iir_coef[4].iir0.coef_b={109848832, -242200690, 134217728, 0},
        .psap_iir_coef[4].iir0.coef_a={134217728, -242200690, 109848832, 0},
        .psap_iir_coef[4].iir1.coef_b={-109848632, 351018377, -375156857, 134217728},
        .psap_iir_coef[4].iir1.coef_a={134217728, -375156857, 351018377, -109848632},
        .psap_iir_coef[5].iir0.coef_b={81171724, -205166742, 134217728, 0},
        .psap_iir_coef[5].iir0.coef_a={134217728, -205166742, 81171724, 0},
        .psap_iir_coef[5].iir1.coef_b={-81156770, 281801460, -331721088, 134217728},
        .psap_iir_coef[5].iir1.coef_a={134217728, -331721088, 281801460, -81156770},
        .psap_iir_coef[6].iir0.coef_b={66181540, -181899702, 134217728, 0},
        .psap_iir_coef[6].iir0.coef_a={134217728, -181899702, 66181540, 0},
        .psap_iir_coef[6].iir1.coef_b={-66113394, 241306967, -301500735, 134217728},
        .psap_iir_coef[6].iir1.coef_a={134217728, -301500735, 241306967, -66113394},
        .psap_iir_coef[7].iir0.coef_b={62863906, -176244865, 134217728, 0},
        .psap_iir_coef[7].iir0.coef_a={134217728, -176244865, 62863906, 0},
        .psap_iir_coef[7].iir1.coef_b={-62771443, 231892680, -293809571, 134217728},
        .psap_iir_coef[7].iir1.coef_a={134217728, -293809571, 231892680, -62771443},
        .psap_cpd_cfg[0]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_cpd_cfg[1]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_cpd_cfg[2]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_cpd_cfg[3]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_cpd_cfg[4]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_cpd_cfg[5]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_cpd_cfg[6]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_cpd_cfg[7]={27069, 0, 27069, 0, 10505, -29491, 32689, 79, 32766, 2, 29491, 3277, 0},
        .psap_limiter_cfg={524287, 31991, 777, 32752, 16, 0},
        .psap_dac_gain_offset=0,
        .psap_adc_gain_offset=-12,
    }

};

const struct_psap_cfg * psap_coef_list_50p7k[PSAP_COEF_LIST_NUM] = {
    &PsapFirCoef_50p7k_mode0,
};

const struct_psap_cfg * psap_coef_list_48k[PSAP_COEF_LIST_NUM] = {
    &PsapFirCoef_50p7k_mode0,
};

const struct_psap_cfg * psap_coef_list_44p1k[PSAP_COEF_LIST_NUM] = {
    &PsapFirCoef_50p7k_mode0,
};

const IIR_CFG_T audio_eq_sw_iir_cfg = {
    .gain0 = 0,
    .gain1 = 0,
    .num = 5,
    .param = {
        {IIR_TYPE_PEAK, .0,   200,   2},
        {IIR_TYPE_PEAK, .0,   600,  2},
        {IIR_TYPE_PEAK, .0,   2000.0, 2},
        {IIR_TYPE_PEAK, .0,  6000.0, 2},
        {IIR_TYPE_PEAK, .0,  12000.0, 2}
    }
};

const IIR_CFG_T * const audio_eq_sw_iir_cfg_list[EQ_SW_IIR_LIST_NUM]={
    &audio_eq_sw_iir_cfg,
};

const FIR_CFG_T audio_eq_hw_fir_cfg_44p1k = {
    .gain = 0.0f,
    .len = 384,
    .coef =
    {
        (1<<23)-1,
    }
};

const FIR_CFG_T audio_eq_hw_fir_cfg_48k = {
    .gain = 0.0f,
    .len = 384,
    .coef =
    {
        (1<<23)-1,
    }
};


const FIR_CFG_T audio_eq_hw_fir_cfg_96k = {
    .gain = 0.0f,
    .len = 384,
    .coef =
    {
        (1<<23)-1,
    }
};

FIR_CFG_T audio_eq_hw_fir_adaptive_eq_cfg = {
    .gain = 0.0f,
    .len = 384,
    .coef =
    {
        (1<<23)-1,
    }
};

const FIR_CFG_T * const audio_eq_hw_fir_cfg_list[EQ_HW_FIR_LIST_NUM]={
    &audio_eq_hw_fir_cfg_44p1k,
    &audio_eq_hw_fir_cfg_48k,
    &audio_eq_hw_fir_cfg_96k,
};

//hardware dac iir eq
// jay
const IIR_CFG_T audio_eq_hw_dac_iir_cfg = {
#if defined(AUDIO_HEARING_COMPSATN)
    .gain0 = -22,
    .gain1 = -22,
#else
    .gain0 = -2,
    .gain1 = -2,
#endif
    .num = 5,
    .param = {
		{IIR_TYPE_HIGH_PASS,  0,   20.0,   0.9},
		{IIR_TYPE_PEAK,     -14,  190.0,   0.8},
		{IIR_TYPE_PEAK,    -2.5,  600.0,   1.3},
		{IIR_TYPE_PEAK,    -3.5,  800.0,   1.5},
		{IIR_TYPE_HIGH_SHELF,-3, 4800.0,   1.5},
    }
};

// jay
const IIR_CFG_T audio_eq_anc_hw_dac_iir_cfg = {
#if defined(AUDIO_HEARING_COMPSATN)
    .gain0 = -22,
    .gain1 = -22,
#else  // jay
    .gain0 = -2,
    .gain1 = -2,
#endif
    .num = 6,
    .param = {
        {IIR_TYPE_HIGH_PASS,  0,   20.0,   0.55},
        {IIR_TYPE_PEAK,     -13,  200.0,   0.6},
        {IIR_TYPE_PEAK,      -2,  600.0,   0.9},
        {IIR_TYPE_PEAK,      -4,  900.0,   2.2},
        {IIR_TYPE_HIGH_SHELF,-4, 4500.0,   1.0},
        {IIR_TYPE_PEAK,      -2, 9000.0,   2.0},
    }
};

/* Add by lewis */
const IIR_CFG_T audio_eq_bass_hw_dac_iir_cfg = {
    .gain0 = -2,
    .gain1 = -2,
    .num = 6,
    .param = {
    	{IIR_TYPE_HIGH_PASS,  0,   20.0,   0.9},
        {IIR_TYPE_PEAK,     -14,  190.0,   0.8},
        {IIR_TYPE_PEAK,    -2.5,  600.0,   1.3},
        {IIR_TYPE_PEAK,    -3.5,  800.0,   1.5},
        {IIR_TYPE_HIGH_SHELF,-8, 4800.0,   1.1},
        {IIR_TYPE_LOW_SHELF,  2,  200.0,   1.0},
    }
};

const IIR_CFG_T audio_eq_bass_anc_hw_dac_iir_cfg = {
    .gain0 = -2,
    .gain1 = -2,
    .num = 7,
    .param = {
        {IIR_TYPE_HIGH_PASS,  0,   20.0,   0.55},
        {IIR_TYPE_PEAK,     -13,  200.0,   0.6},
        {IIR_TYPE_PEAK,      -2,  600.0,   0.9},
        {IIR_TYPE_PEAK,      -4,  900.0,   2.2},
        {IIR_TYPE_HIGH_SHELF,-8, 4500.0,   1.0},
        {IIR_TYPE_PEAK,      -2, 9000.0,   2.0},
        {IIR_TYPE_LOW_SHELF,  2,  210.0,   1.0},
    }
};

const IIR_CFG_T audio_eq_jazz_hw_dac_iir_cfg = {
    .gain0 = -1,
    .gain1 = -1,
    .num = 5,
    .param = {
    	{IIR_TYPE_HIGH_PASS,  0,   20.0,   0.9},
        {IIR_TYPE_PEAK,     -17,  200.0,   0.7},
        {IIR_TYPE_PEAK,      -4,  600.0,   0.8},
        {IIR_TYPE_PEAK,    -3.5,  800.0,   1.5},
        {IIR_TYPE_HIGH_SHELF,-1, 4800.0,   2.0},
    }
};

const IIR_CFG_T audio_eq_jazz_anc_hw_dac_iir_cfg = {
    .gain0 = -1,
    .gain1 = -1,
    .num = 6,
    .param = {
        {IIR_TYPE_HIGH_PASS,  0,   20.0,  0.55},
        {IIR_TYPE_PEAK,     -16,  210.0,   0.6},
        {IIR_TYPE_PEAK,      -5,  600.0,   0.7},
        {IIR_TYPE_PEAK,    -4.5,  900.0,   2.0},
        {IIR_TYPE_HIGH_SHELF,-2, 4500.0,   1.0},
        {IIR_TYPE_PEAK,      -3, 9000.0,   2.0},
    }
};

const IIR_CFG_T audio_eq_pop_hw_dac_iir_cfg = {
    .gain0 = -2,
    .gain1 = -2,
    .num = 5,
    .param = {
        {IIR_TYPE_PEAK,      -1,   25.0,   0.8},
        {IIR_TYPE_PEAK,   -11.5,  180.0,   0.8},
        {IIR_TYPE_PEAK,    -1.5,  550.0,   1.0},
        {IIR_TYPE_PEAK,    -3.5,  800.0,   1.7},
        {IIR_TYPE_HIGH_SHELF,-5, 4800.0,   1.3},
    }
};

const IIR_CFG_T audio_eq_pop_anc_hw_dac_iir_cfg = {
    .gain0 = -2,
    .gain1 = -2,
    .num = 7,
    .param = {
        {IIR_TYPE_HIGH_PASS,  0,   20.0,  0.55},
        {IIR_TYPE_PEAK,    -9.5,  200.0,   0.6},
        {IIR_TYPE_PEAK,      -2,  600.0,   0.7},
        {IIR_TYPE_PEAK,      -4,  900.0,   2.2},
        {IIR_TYPE_HIGH_SHELF,-6, 4500.0,   1.0},
        {IIR_TYPE_PEAK,      -2, 9000.0,   2.0},
        {IIR_TYPE_LOW_SHELF, -2,  190.0,   0.8},
    }
};

IIR_CFG_T audio_eq_user_hw_dac_iir_cfg = {
    .gain0 = -2,
    .gain1 = -2,
    .num = USER_EQ_BANDS + 5,
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

		{IIR_TYPE_HIGH_PASS,  0,   20.0,   0.9},
		{IIR_TYPE_PEAK,     -14,  190.0,   0.8},
		{IIR_TYPE_PEAK,    -2.5,  600.0,   1.3},
		{IIR_TYPE_PEAK,    -3.5,  800.0,   1.5},
		{IIR_TYPE_HIGH_SHELF,-3, 4800.0,   1.5},
    }
};

IIR_CFG_T audio_eq_user_anc_hw_dac_iir_cfg = {
    .gain0 = -2,
    .gain1 = -2,
    .num = USER_EQ_BANDS + 6,
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

		{IIR_TYPE_HIGH_PASS,  0,   20.0,   0.55},
        {IIR_TYPE_PEAK,     -13,  200.0,   0.6},
        {IIR_TYPE_PEAK,      -2,  600.0,   0.9},
        {IIR_TYPE_PEAK,      -4,  900.0,   2.2},
        {IIR_TYPE_HIGH_SHELF,-4, 4500.0,   1.0},
        {IIR_TYPE_PEAK,      -2, 9000.0,   2.0},
    }
};
/* End Add by lewis */

IIR_CFG_T audio_eq_hw_dac_iir_adaptive_eq_cfg = {
    .gain0 = 0,
    .gain1 = 0,
    .num = 1,
    .param = {
        {IIR_TYPE_PEAK, 0,    1000,    0.7},
    }
};

const IIR_CFG_T * const POSSIBLY_UNUSED audio_eq_hw_dac_iir_cfg_list[EQ_HW_DAC_IIR_LIST_NUM]={
    &audio_eq_hw_dac_iir_cfg,
	&audio_eq_anc_hw_dac_iir_cfg,
	/* Add by lewis */
	&audio_eq_bass_hw_dac_iir_cfg,
	&audio_eq_bass_anc_hw_dac_iir_cfg,
	&audio_eq_jazz_hw_dac_iir_cfg,
	&audio_eq_jazz_anc_hw_dac_iir_cfg,
	&audio_eq_pop_hw_dac_iir_cfg,
	&audio_eq_pop_anc_hw_dac_iir_cfg,
	&audio_eq_user_hw_dac_iir_cfg,
	&audio_eq_user_anc_hw_dac_iir_cfg,
	/* End Add by lewis */
};

//hardware dac iir eq
const IIR_CFG_T audio_eq_hw_adc_iir_adc_cfg = {
    .gain0 = 0,
    .gain1 = 0,
    .num = 1,
    .param = {
        {IIR_TYPE_PEAK, 0.0,   1000.0,   0.7},
    }
};

const IIR_CFG_T * const POSSIBLY_UNUSED audio_eq_hw_adc_iir_cfg_list[EQ_HW_ADC_IIR_LIST_NUM]={
    &audio_eq_hw_adc_iir_adc_cfg,
};



//hardware iir eq
const IIR_CFG_T audio_eq_hw_iir_cfg = {
    .gain0 = 0,
    .gain1 = 0,
    .num = 8,
    .param = {
        {IIR_TYPE_PEAK, -10.1,   100.0,   7},
        {IIR_TYPE_PEAK, -10.1,   400.0,   7},
        {IIR_TYPE_PEAK, -10.1,   700.0,   7},
        {IIR_TYPE_PEAK, -10.1,   1000.0,   7},
        {IIR_TYPE_PEAK, -10.1,   3000.0,   7},
        {IIR_TYPE_PEAK, -10.1,   5000.0,   7},
        {IIR_TYPE_PEAK, -10.1,   7000.0,   7},
        {IIR_TYPE_PEAK, -10.1,   9000.0,   7},

    }
};

const IIR_CFG_T * const POSSIBLY_UNUSED audio_eq_hw_iir_cfg_list[EQ_HW_IIR_LIST_NUM]={
    &audio_eq_hw_iir_cfg,
};

const DrcConfig audio_drc_cfg = {
     .knee = 3,
     .filter_type = {14, -1},
     .band_num = 2,
     .look_ahead_time = 10,
     .band_settings = {
         {-20, 0, 2, 3, 3000, 1},
         {-20, 0, 2, 3, 3000, 1},
     }
 };

const LimiterConfig audio_limiter_cfg = {
    .knee = 2,
    .look_ahead_time = 10,
    .threshold = -20,
    .makeup_gain = 19,
    .ratio = 1000,
    .attack_time = 3,
    .release_time = 3000,
};

const SpectrumFixConfig audio_spectrum_cfg = {
    .freq_num = 9,
    .freq_list = {200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800},
};

const DynamicBoostConfig audio_dynamic_boost_cfg = {
    .debug = 0,
    .xover_freq = {200},
    .order = 4,
    .CT = -40,
    .CS = 0.18,
    .WT = -40,
    .WS = 0.3,
    .ET = -60,
    .ES = 0,
    .attack_time        = 0.0001f,
    .release_time       = 0.0001f,
    .makeup_gain        = -6,
    .delay              = 128,
    .tav                = 1.0f,
    .eq_num = 2,
    .boost_eq = {
        {
            .gain = 10,
            .freq = 33,
            .Q = 0.5,
        },
        {
            .gain = -1,
            .freq = 240,
            .Q = 1.1,
        },
        {
            .gain = 10,
            .freq = 1000, // -1 for unused eq
            .Q = 0.7,
        },
        {
            .gain = 10,
            .freq = 2000, // -1 for unused eq
            .Q = 0.7,
        }
    }
};

IIR_CFG_T audio_iir_running_set_cfg = {
    .gain0 = 0,
    .gain1 = 0,
    .num = 2,
    .param = {
        {IIR_TYPE_PEAK, 10,   33,   0.5},
        {IIR_TYPE_PEAK, -1,   240,   1.1},
    }
};

IIR_CFG_T audio_iir_running_set_cfg_average = {
    .gain0 = 0,
    .gain1 = 0,
    .num = 2,
    .param = {
        {IIR_TYPE_PEAK, 0,   33,   0.5},
        {IIR_TYPE_PEAK, 0,   240,   1.1},
    }
};
