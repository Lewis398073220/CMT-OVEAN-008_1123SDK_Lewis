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
#ifndef __APP_ANC_FADE_H__
#define __APP_ANC_FADE_H__

#include "plat_types.h"
#include "hal_aud.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Add by lewis */
#define ANC_FADE_MS         (200) //Modify by lewis from 10 to 200
#define ANC_FADE_CNT        (512)
/* End Add by lewis */

int32_t app_anc_fade_init(void);
int32_t app_anc_fade_deinit(void);
int32_t app_anc_fadein(uint32_t types);
/* Modify by lewis */
#if 0
int32_t app_anc_fadeout(uint32_t types);
#else
int32_t app_anc_fadeout(uint32_t types, uint32_t fade_ms);
#endif
/* End Modify by lewis */

#ifdef __cplusplus
}
#endif

#endif
