cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y +=  \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    $(EPLAYER_INCLUDES) \
    $(ECOMM_INCLUDES) \
    -I$(BLE_GAF_CORE_DIR_PATH)/inc \
    -I$(BLE_AOB_APP_DIR_PATH)/inc \
    -Iservices/audio_bt \
    -Iservices/audioflinger \
    -Iservices/norflash_api \
    -Iservices/nv_section/factory_section \
    -Iservices/nv_section/log_section \
    -Iservices/overlay \
    -Iservices/resources \
    -Iservices/audio_process \
    -Iservices/lea_player/inc \
    -Iservices/ux/common/ble_audio_core/inc \
    -Iapps/apptester \
    -Iapps/factory \
    -Iapps/app_rpc \
    -Iapps/ble_audio_test \
    -Iapps/voice_detector \
    -Iservices/ux/common/sound \
    -Iutils/crc32 \
    -Iutils/list \
    -Iplatform/drivers/bt \
    -Iplatform/drivers/hifi4 \
    -Iplatform/drivers/ana \
    -Iapps/audioplayers/rbplay \
    -Itests/anc_usb \
    -Iapps/anc/inc \
    -Ithirdparty/userapi \
    -Iservices/communication \
    -Iutils/cqueue \
    -Iutils/cfifo \
    -Iservices/ai_voice/audio \
    -Iservices/ai_voice/manager \
    -Iservices/ai_voice/transport \
    -Iservices/app_ai/inc \
    -Iservices/interconnection/red \
    -Iservices/interconnection/green \
    -Iservices/bridge/ \
    -Iservices/bt_app/a2dp_codecs/include \
    -Iservices/aob_bes_ux/inc \
    -Iservices/aob_custom_ux \
    -Ithirdparty/tile \
    -Iservices/ux/common/ibrt/ibrt_middleware/inc \
    -Iservices/ux/common/ibrt/custom_api \
    -Iservices/ux/common/ibrt/app_ibrt/inc \
    -Iservices/ux/common/ibrt/ibrt_core/inc \
    -Iservices/ux/product/earbud_tws/inc \
    -Iapps/earbuds/conn/ \
    -Iservices/rpc/inc \
    -Ithirdparty/tile/tile_target \
    -Iinclude/ \
    -Iapps/btwatch \
    -Iapps/sensorhub \
    -Iapps/dsp_hifi4 \
    -Iapps/dsp_m55 \
    -Iplatform/drivers/cp_accel \
    -Iservices/audio_manager \
    -Iplatform/drivers/ac107 \
    -Iplatform/drivers/cst820
    
ifeq ($(IBRT),1)

ifeq ($(IBRT_UI),1)
ccflags-y += \
    -Iservices/ux/common/ibrt/app_ibrt/inc  \
    -Iservices/ux/common/ibrt/ibrt_core/inc \
    -Iservices/ux/common/ibrt/custom_api \
    -Iservices/ux/common/ibrt/ibrt_ui/inc \
    -Iapps/earbuds/conn
endif
endif