cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))


obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y +=  \
    $(BLUETOOTH_ADAPTER_INCLUDES) \
    -Iservices/audioflinger \
    -Iservices/overlay \
    -Iservices/resources \
    -Iservices/audio_bt \
    -Iservices/audio_process \
    -Iapps/apptester \
    -Iapps/factory \
    -Iutils/crc32 \
    -Iplatform/drivers/bt \
    -Iplatform/drivers/ana \
    -Iapps/audioplayers/rbplay \
    -Itests/anc_usb \
    -Iapps/anc/inc \
    -Iapps/ota \
    -Ithirdparty/userapi \
    -Iservices/voicepath \
    -Iservices/voicepath/gsound/gsound_service \
    -Iservices/voicepath/gsound/gsound_target \
    -Iservices/communication \
    -Iutils/cqueue \
    -Iservices/ai_voice/ama/ama_manager \
    -Iservices/ai_voice/manager \
    -Imultimedia/inc/audio/codec/sbc/inc \
    -Iservices/interconnection \
    -Imultimedia/inc/audio/codec/lc3/inc \
    -Iservices/audio_manager \
	-Iapps/user