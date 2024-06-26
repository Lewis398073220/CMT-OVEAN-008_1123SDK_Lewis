
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

all_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
all_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
all_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))

filtout_c :=
filtout_cpp :=
filtout_s :=

ifneq ($(BISTO_ENABLE),1)
filtout_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*gsound.c))
filtout_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*gsound.cpp))
endif

ifneq ($(AI_OTA), 1)
filtout_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*ota.c))
filtout_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*ota.cpp))
endif

ifneq ($(PROMPT_IN_FLASH),1)
filtout_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*prompt.c))
filtout_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*prompt.cpp))
filtout_s += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*prompt.S))
else
ifneq ($(PROMPT_EMBEDED),1)
filtout_s += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*prompt.S))
endif
endif

ifneq ($(COMBO_CUSBIN_IN_FLASH),1)
filtout_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*combo.c))
filtout_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*combo.cpp))
endif

obj_c := $(filter-out $(filtout_c),$(all_c))
obj_cpp := $(filter-out $(filtout_cpp),$(all_cpp))
obj_s := $(filter-out $(filtout_s),$(all_s))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
    -I$(obj)/../../nv_section/userdata_section \
    -I$(obj)/../../nv_section/include \
    -I$(obj)/../platform/cmsis/inc \
    -Iutils/crc32 \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    -Iutils/hexdump \
    -Iapps/key \
    -Iservices/bt_app \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/bt \
    -Iservices/nv_section/customparam_section \
    -Iservices/nv_section/factory_section \
    -Iutils/heap \
    -Iservices/norflash_api \
    -Iservices/nv_setion/userdata_section \
    -Iservices/voicepath/gsound/gsound_target_api_read_only \
    -Iservices/voicepath/gsound/gsound_custom/inc \
    -Iplatform/drivers/norflash \
    -Iplatform/hal \
    -Iplatform/cmsis/inc \
    -Iservices/osif/ \
    -Iservices/ota/ai_ota \
    -Iutils/cqueue \
    -Iservices/nv_section \
    -Iservices/bt_app/a2dp_codecs/include \
    -Iutils/aes_cmac \
    -Iutils/hsm \
    -Iservices/audio_manager \
	-Iapps/user
	
ifeq ($(CUSTOM_BITRATE),1)
ccflags-y += -Iapps/earbuds/conn/
endif

ifeq ($(FLASH_SUSPEND),1)
ccflags-y += -DFLASH_SUSPEND
endif

export NV_RECORD_FLUSH_EXECUTION_INTERVAL_MS ?= 1200000
ccflags-y += -DNV_RECORD_FLUSH_EXECUTION_INTERVAL_MS=$(NV_RECORD_FLUSH_EXECUTION_INTERVAL_MS)

export NV_RECORD_ASYNC_FLUSH_CONFIRM_TIME_MS ?= 5000
ccflags-y += -DNV_RECORD_ASYNC_FLUSH_CONFIRM_TIME_MS=$(NV_RECORD_ASYNC_FLUSH_CONFIRM_TIME_MS)

ifeq ($(NORFLASH_API_FLUSH_IN_SLEEP_HOOK),1)
ccflags-y += -DNORFLASH_API_FLUSH_IN_SLEEP_HOOK
endif

ifeq ($(TX_IQ_CAL),1)
subdir-ccflags-y += -DTX_IQ_CAL
endif

ifeq ($(BLE_AUDIO_ENABLED),1)
ccflags-y += \
    -Iservices/audio_manager \
    -Iservices/ux/common/ble_audio_core/inc
endif

ifeq ($(PROMPT_IN_FLASH),1)
AFLAGS_prompt.o += -Wa,-Iconfig/_default_cfg_src_/res/prompt_package
endif

ifeq ($(DMA_AUDIO_APP),1)
CFLAGS_nvrecord_extension.o += -DDMA_AUDIO_APP
endif