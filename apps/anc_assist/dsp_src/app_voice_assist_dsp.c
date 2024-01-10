/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#include <string.h>
#include "plat_types.h"
#include "hal_trace.h"
#include "app_voice_assist_dsp.h"
#include "app_voice_assist_fir_lms.h"

int32_t anc_assist_dsp_init()
{
#ifdef ANC_ALGO_DSP
    assist_algo_dsp_init();
#endif

#ifdef FIR_ADAPT_ANC_M55
    assist_fir_lms_dsp_init();
#endif
    return 0;
}