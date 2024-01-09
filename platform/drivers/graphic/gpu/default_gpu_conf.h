
/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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

#ifndef _GPU_DEFAULT_CFG_H
#define _GPU_DEFAULT_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "default_graphic_conf.h"

#define    GPU_FB_WIDTH    BOARD_LCDC_WIDTH
#define    GPU_FB_HEIGHT   BOARD_LCDC_HEIGHT
#define    GPU_FB_FORMAT   VG_LITE_RGBA8888
#define    GPU_FB_BPP      32

#ifndef LVGL_GPU_EN
#define    GPU_USE_PSRAM
#endif

#ifdef __cplusplus
}
#endif

#endif //_GPU_DEFAULT_CFG_H
