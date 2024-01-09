/***************************************************************************
 *
 * Copyright 2023 Add by lewis
 * Handle custom application
 * All rights reserved. All unpublished rights reserved.
 *
 ****************************************************************************/

#ifdef CMT_008_SPP_TOTA_V2

#ifndef __TOTA_SPP_CUSTOM_H__
#define __TOTA_SPP_CUSTOM_H__

#include "hal_aud.h"

#define MIC_SELECT_TEST   0x50
#define ANC_MODE_TEST     0x51
#define FUNCTION_TEST     0x52
#define LEN_OF_ARRAY      512 /* Copy from 'LEN_OF_IMAGE_TAIL_TO_FINDKEY_WORD'*/

typedef enum 
{
    TALK_MIC = 1,
    L_FF_MIC,
    R_FF_MIC,
    L_FB_MIC,
    R_FB_MIC,
    MIC_RESET,
    ALL_SELECT_MIC = 0xFF
}MIC_SELECT_E;

typedef enum 
{
    ANC_MODE_CMD = 1,
    AWARENESS_MODE_CMD,
    ANC_OFF_CMD,
    ANC_NONE = 0xFF
}ANC_MODE_E;

typedef enum 
{
    POWER_OFF_CMD = 1,
    DISCONNECT_BT_CMD,
    ANC_WIRELESS_DEBUG_EN,
    ANC_WIRELESS_DEBUG_DIS,
    GET_FIRMWARE_VER,
    GET_FIRMWARE_VER_BUILD_DATE,
    FUNCTION_TEST_NONE = 0xFF
}FUNCTION_TEST_E;

enum AUD_IO_PATH_T current_select_mic(void);


#endif

#endif
