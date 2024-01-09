/**
 ****************************************************************************************
 *
 * @file acc_ot.h
 *
 * @brief Audio Content Control - Object Transfer - Definitions
 *
 * Copyright (C) RivieraWaves 2019-2021
 *
 ****************************************************************************************
 */

#ifndef ACC_OT_H_
#define ACC_OT_H_

/**
 ****************************************************************************************
 * @defgroup ACC_OT Object Transfer Service
 * @ingroup ACC
 * @brief Description of Object Transfer Service module
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "acc.h"            // Audio Content Control Definitions
#if (GAF_ACC_OTS || GAF_ACC_OTC)
#include "otp.h"            // Object Transfer Profile Definitions
#endif //(GAF_ACC_OTS || GAF_ACC_OTC)

#endif // ACC_OT_H_
