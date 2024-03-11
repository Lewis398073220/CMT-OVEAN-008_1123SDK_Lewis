#### Add by Jay #####
#### HARDWARE CONFIG ######
KBUILD_CPPFLAGS += \
    -DCMT_008_LDO_3V0_ENABLE \
    -DCMT_008_LDO_1V8_ENABLE \
    -DCMT_008_AC107_ADC \
    -DCMT_008_CST820_TOUCH \
    -DCMT_008_NTC_DETECT \
    -DCMT_008_3_5JACK_CTR \
    -DCMT_008_UART_USBAUDIO_SWITCH \
    -DCMT_008_CHARGE_CURRENT \
    -DCMT_008_MIC_CONFIG \
    -DCMT_008_UI_LED_INDICATION \
    -DCMT_008_EN_LED_BREATH
##### HARDWARE CONFIG END ######

export BQB_TEST ?= 1
ifeq ($(BQB_TEST),1)
KBUILD_CPPFLAGS += -DBQB_TEST
export APP_TRACE_RX_ENABLE ?= 1
export APP_RX_API_ENABLE ?= 1
export BQB_PROFILE_TEST ?= 1
endif

#VIO supply of CST820 and ac107 should be 1.8v, to see pmu_vio_3v3.
export PMU_VIO_3V3_ENABLE ?= 1
ifeq ($(PMU_VIO_3V3_ENABLE),1)
KBUILD_CPPFLAGS += -DPMU_VIO_3V3_ENABLE
endif

#If enable, it will cause I2C function of CST820 abnormal.
export i2c_mode_task ?= 0
ifeq ($(i2c_mode_task),1)
KBUILD_CPPFLAGS += -Di2c_mode_task
endif

export AUDIO_LINEIN ?= 1
ifeq ($(AUDIO_LINEIN),1)
KBUILD_CPPFLAGS += -DAUDIO_LINEIN
export AF_DEVICE_I2S ?= 1
export I2S_MCLK_PIN ?= 1
export I2S_MCLK_FROM_SPDIF ?= 1
endif

export SPEECH_SIDETONE ?= 1

export CMT_008_UI ?= 1
ifeq ($(CMT_008_UI),1)
KBUILD_CPPFLAGS += -DCMT_008_UI
endif

export CMT_008_SPP_TOTA_V2 ?= 1
ifeq ($(CMT_008_SPP_TOTA_V2),1)
KBUILD_CPPFLAGS += -DCMT_008_SPP_TOTA_V2
endif

export CMT_008_BLE_ENABLE ?= 1
ifeq ($(CMT_008_BLE_ENABLE),1)
KBUILD_CPPFLAGS += -DCMT_008_BLE_ENABLE
endif

export AF_STREAM_PLAYBACK_FADEINOUT ?= 1
ifeq ($(AF_STREAM_PLAYBACK_FADEINOUT),1)
KBUILD_CPPFLAGS += -DAF_STREAM_PLAYBACK_FADEINOUT
endif

export EN_LR_BALANCE ?= 1
ifeq ($(EN_LR_BALANCE),1)
KBUILD_CPPFLAGS += -DEN_LR_BALANCE
endif

export CODEC_DAC_PROMPT_ALONE_VOLUME_TABLE ?= 1
ifeq ($(CODEC_DAC_PROMPT_ALONE_VOLUME_TABLE),1)
KBUILD_CPPFLAGS += -DCODEC_DAC_PROMPT_ALONE_VOLUME_TABLE
endif

#### Add by Jay, end. #####

#### ANC DEFINE START ######
export ANC_APP ?= 1

#### ANC CONFIG ######
export ANC_FB_CHECK         ?= 0
export ANC_FF_CHECK         ?= 0
export ANC_TT_CHECK         ?= 0
export ANC_FF_ENABLED	    ?= 1
export ANC_FB_ENABLED	    ?= 1
export ANC_ASSIST_ENABLED   ?= 0
export AUDIO_ANC_FB_MC      ?= 0
export AUDIO_ANC_FB_MC_HW   ?= 1
export AUDIO_ANC_FB_ADJ_MC  ?= 0
export AUDIO_SECTION_SUPPT  ?= 1
export AUDIO_ANC_SPKCALIB_HW ?= 0
export AUDIO_ANC_FIR_HW     ?= 0
export AUDIO_ANC_TT_HW      ?= 0
##### ANC DEFINE END ######

export PSAP_APP  ?= 0

AUDIO_HW_LIMITER ?= 0
ifeq ($(AUDIO_HEARING_COMPSATN),1)
AUDIO_HW_LIMITER := 1
export PSAP_APP_ONLY_MUSIC := 1
endif
ifeq ($(AUDIO_HW_LIMITER),1)
export PSAP_APP := 1
endif

ifeq ($(AUDIO_ANC_TT_HW),1)
export AUDIO_ANC_FB_MC_HW := 1
endif

export AUDIO_OUTPUT_DAC2 ?= 0

APP_ANC_TEST ?= 1
ifeq ($(APP_ANC_TEST),1)
export TOTA_v2 := 1
endif

ifeq ($(ANC_APP),1)
KBUILD_CPPFLAGS += \
    -DANC_APP \
    -D__BT_ANC_KEY__
endif

ifeq ($(USE_CYBERON),1)

export THIRDPARTY_LIB ?= cyberon
KBUILD_CPPFLAGS += -D__CYBERON

export KWS_IN_RAM := 0
ifeq ($(KWS_IN_RAM),1)
CPPFLAGS_${LDS_FILE} += -DKWS_IN_RAM
endif #KWS_IN_RAM

endif #USE_CYBERON

export HAS_BT_SYNC = 1
include config/best1502x_ibrt/target.mk
