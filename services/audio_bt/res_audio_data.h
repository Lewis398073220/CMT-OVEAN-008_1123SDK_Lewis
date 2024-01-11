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
#ifndef __RES_AUDIO_DATA_H__
#define __RES_AUDIO_DATA_H__

#ifdef MEDIA_PLAYER_SUPPORT

const uint8_t EN_POWER_ON [] = {
/* Modify by lewis */
#ifdef CMT_008_UI
#include "res/en/SOUND_POWER_ON_16000.txt"
#else
#include "res/en/SOUND_POWER_ON.txt"
#endif
/* End Modify by lewis */
};

const uint8_t EN_POWER_OFF [] = {
/* Modify by lewis */
#ifdef CMT_008_UI
#include "res/en/SOUND_POWER_OFF_16000.txt"
#else
#include "res/en/SOUND_POWER_OFF.txt"
#endif
/* End Modify by lewis */
};

const uint8_t EN_SOUND_ZERO[] = {
#include "res/en/SOUND_ZERO.txt"
};

const uint8_t EN_SOUND_ONE[] = {
#include "res/en/SOUND_ONE.txt"
};

const uint8_t EN_SOUND_TWO[] = {
#include "res/en/SOUND_TWO.txt"
};

const uint8_t EN_SOUND_THREE[] = {
#include "res/en/SOUND_THREE.txt"
};

const uint8_t EN_SOUND_FOUR[] = {
#include "res/en/SOUND_FOUR.txt"
};

const uint8_t EN_SOUND_FIVE[] = {
#include "res/en/SOUND_FIVE.txt"
};

const uint8_t EN_SOUND_SIX[] = {
#include "res/en/SOUND_SIX.txt"
};

const uint8_t EN_SOUND_SEVEN [] = {
#include "res/en/SOUND_SEVEN.txt"
};

const uint8_t EN_SOUND_EIGHT [] = {
#include "res/en/SOUND_EIGHT.txt"
};

const uint8_t EN_SOUND_PROMPT_ADAPTIVE_ANC [] = {
#include "res/adapt_anc/SOUND_PROMPT_ADAPTIVE_ANC.txt"
};

const uint8_t EN_OPENEAR_PROMPT_ADAPTIVE_ANC [] = {
#include "res/adapt_anc/OPENEAR_PROMPT_ADAPTIVE_ANC.txt"
};

const uint8_t EN_SOUND_CUSTOM_LEAK_DETECT [] = {
#include "res/ld/SOUND_PROMPT_CUSTOM_LEAK.txt"
};

const uint8_t EN_SOUND_NINE [] = {
#include "res/en/SOUND_NINE.txt"
};

const uint8_t EN_BT_PAIR_ENABLE[] = {
/* Modify by lewis */
#ifdef CMT_008_UI
#include "res/en/SOUND_PAIRING_16000.txt"
#else
#include "res/en/SOUND_PAIR_ENABLE.txt"
#endif
/* End Modify by lewis */
};

const uint8_t EN_BT_PAIRING[] = {
#include "res/en/SOUND_PAIRING.txt"
};

const uint8_t EN_BT_PAIRING_FAIL[] = {
#include "res/en/SOUND_PAIRING_FAIL.txt"
};

const uint8_t EN_BT_PAIRING_SUCCESS[] = {
#include "res/en/SOUND_PAIRING_SUCCESS.txt"
};

const uint8_t EN_BT_REFUSE[] = {
#include "res/en/SOUND_REFUSE.txt"
};

const uint8_t EN_BT_OVER[] = {
#include "res/en/SOUND_OVER.txt"
};

const uint8_t EN_BT_ANSWER[] = {
#include "res/en/SOUND_ANSWER.txt"
};

const uint8_t EN_BT_HUNG_UP[] = {
#include "res/en/SOUND_HUNG_UP.txt"
};

const uint8_t EN_BT_CONNECTED [] = {
/* Modify by lewis */
#ifdef CMT_008_UI
#include "res/en/SOUND_CONNECTED_16000.txt"
#else
#include "res/en/SOUND_CONNECTED.txt"
#endif
/* End Modify by lewis */
};

const uint8_t EN_BT_DIS_CONNECT [] = {
/* Modify by lewis */
#ifdef CMT_008_UI
#include "res/en/SOUND_DIS_CONNECT_16000.txt"
#else
#include "res/en/SOUND_DIS_CONNECT.txt"
#endif
/* End Modify by lewis */
};

const uint8_t EN_BT_INCOMING_CALL [] = {
#include "res/en/SOUND_INCOMING_CALL.txt"
};

const uint8_t EN_CHARGE_PLEASE[] = {
/* Modify by lewis */
#ifdef CMT_008_UI
#include "res/en/SOUND_BATTERY_LOW_16000.txt"
#else
#include "res/en/SOUND_CHARGE_PLEASE.txt"
#endif
/* End Modify by lewis */
};

const uint8_t EN_CHARGE_FINISH[] = {
#include "res/en/SOUND_CHARGE_FINISH.txt"
};

const uint8_t EN_LANGUAGE_SWITCH[] = {
#include "res/en/SOUND_LANGUAGE_SWITCH.txt"
};

const uint8_t EN_BT_WARNING[] = {
#include "res/en/SOUND_WARNING.txt"
};

const uint8_t EN_BT_ALEXA_START[] = {
#include "res/en/SOUND_ALEXA_START.txt"
};

const uint8_t EN_BT_ALEXA_STOP[] = {
#include "res/en/SOUND_ALEXA_STOP.txt"
};

const uint8_t EN_BT_GSOUND_MIC_OPEN[] = {
#include "res/en/SOUND_GSOUND_MIC_OPEN.txt"
};

const uint8_t EN_BT_GSOUND_MIC_CLOSE[] = {
#include "res/en/SOUND_GSOUND_MIC_CLOSE.txt"
};

const uint8_t EN_BT_GSOUND_NC[] = {
#include "res/en/SOUND_GSOUND_NC.txt"
};

#ifdef __INTERACTION__
const uint8_t EN_BT_FINDME[] = {
#include "res/en/SOUND_FINDME.txt"
};
#endif

const uint8_t EN_BT_MUTE[] = {
#include "res/SOUND_MUTE.txt"
};

/* Add by lewis */
#ifdef CMT_008_UI
const uint8_t EN_BT_SINGLE_BEEP_TONE_16000[] = {
#include "res/en/SOUND_SINGLE_BEEP_TONE_16000.txt"
};

const uint8_t EN_BT_DOUBLE_BEEP_TONE_16000[] = {
#include "res/en/SOUND_DOUBLE_BEEP_TONE_16000.txt"
};

const uint8_t EN_BT_VOLMIN_TONE_16000[] = {
#include "res/en/SOUND_VOLMIN_TONE_16000.txt"
};

const uint8_t EN_BT_VOLMAX_TONE_16000[] = {
#include "res/en/SOUND_VOLMAX_TONE_16000.txt"
};

const uint8_t EN_BT_ANC_ON_16000[] = {
#include "res/en/SOUND_ANC_ON_16000.txt"
};

const uint8_t EN_BT_ANC_OFF_16000[] = {
#include "res/en/SOUND_ANC_OFF_16000.txt"
};

const uint8_t EN_BT_AWARENESS_ON_16000[] = {
#include "res/en/SOUND_AWARENESS_ON_16000.txt"
};

const uint8_t EN_BT_MIC_MUTE_16000[] = {
#include "res/en/SOUND_MIC_MUTE_16000.txt"
};

const uint8_t EN_BT_MIC_UNMUTE_16000[] = {
#include "res/en/SOUND_MIC_UNMUTE_16000.txt"
};

const uint8_t EN_BT_FACTORY_RESET_16000[] = {
#include "res/en/SOUND_FACTORY_RESET_16000.txt"
};

const uint8_t EN_BT_BATTERY_LOW_POWER_OFF_16000[] = {
#include "res/en/SOUND_BATTERY_LOW_POWER_OFF_16000.txt"
};
#endif
/* End Add by lewis */

#endif

#endif /* __RES_AUDIO_DATA_H__ */
