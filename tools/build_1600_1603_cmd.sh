#!/bin/bash
[ -z "$1" ] &&{
    echo The Chip is Null !!!
    exit
}

[ -z "$2" ] &&{
    echo The Target is Null !!!
    exit
}

[ -z "$3" ] &&{
    echo "The Target_Customer is Null !!! \n"
    echo "hearing_aid_customer or audio_customer "
    exit
}


CHIPID=$1
TARGET_LIST=$2
CUSTOMER_TARGET_LIST=""
TARGET_CUSTOMER=$3
RELEASE=$4

SECURITY_ENABLE=$5
INCLUDE_SPAUDIO=$6
INCLUDE_SE=$7

echo "param opt:"
echo "CHIPID=$CHIPID TARGET_LIST=$TARGET_LIST TARGET_CUSTOMER=$TARGET_CUSTOMER RELEASE=$RELEASE SECURITY_ENABLE=$SECURITY_ENABLE INCLUDE_SPAUDIO=$INCLUDE_SPAUDIO INCLUDE_SE=$INCLUDE_SE"


TARGET_DIR="config/"$TARGET_LIST
TARGET_NAME="$TARGET_LIST"

#git clean -d -fx
DATE=`date +%F | sed 's/-//g'`
commitid=`git rev-parse --short HEAD`

export CROSS_COMPILE="ccache arm-none-eabi-"

if [[ $RELEASE == "clean" ]];
then
    echo -e "rm old lib"
    LIB_DIR=lib/bes/
    rm build_err.log
    rm -rf $LIB_DIR
    rm out -rf
    exit;
elif [[ $RELEASE == "release" ]];
then
    echo -e "release"
    # LIB_DIR=lib/bes/
    # rm build_err.log
    # rm -rf $LIB_DIR
    # rm out -rf
fi


################################### var define ###################################
M55_SYS_CFG="NOSTD=0 DSP_M55_TRC_TO_MCU=1 M55_ALL_TCM_TEST=1 BTH_AS_MAIN_MCU=1 ANC_ALGO_DSP=1 A2DP_AAC_ON=1 "
M55_PSAP_SW_CFG="PSAP_SW_APP=1 PSAP_SW_ALGO=bes"
M55_ALGO_MCPP="AUDIO_DSP_ACCEL_USE_M55=1 SPEECH_ALGO_DSP=thirdparty "

BTH_SYS_CFG="DSP_M55_TRC_TO_MCU=1 M55_ALL_TCM_TEST=1 BTH_AS_MAIN_MCU=1 BTH_USE_SYS_PERIPH=1 HOST_GEN_ECDH_KEY=1 BT_RAMRUN_NEW=1 BT_RF_PREFER=2M A2DP_AAC_ON=1 OS_TASKCNT=27"

BTH_PSAP_SW_CFG="DSP_M55=1 PSAP_SW_APP=1 "
BTH_ANC_CFG="DSP_M55=1 ANC_ASSIST_ENABLED=1 ANC_ASSIST_ON_DSP=M55 ANC_ASSIST_ALGO_ON_DSP=1 ANC_ASSIST_LE_AUDIO_SUPPORT=1 "

##algo start ##
## m55 ##
BTH_ALGO_MCPP="AUDIO_DSP_ACCEL=M55 SPEECH_ALGO_DSP=M55 "
## hifi ## pls note need M55_ALL_TCM_TEST=0(bth/m55 sides)
#BTH_ALGO_MCPP="AUDIO_DSP_ACCEL=HIFI SPEECH_ALGO_DSP=HIFI DSP_HIFI4=1 APP_RPC_ENABLE=1 APP_RPC_BTH_DSP_EN=1 "
## algo end ##

BTH_LEA_ENABLE_CFG="BLE=1 BLE_AUDIO_ENABLED=1 BLE_CONNECTION_MAX=2 BT_RAMRUN_NEW=1 BT_DEBUG_TPORTS=0xB8B8 AOB_LOW_LATENCY_MODE=0 \
                REPORT_EVENT_TO_CUSTOMIZED_UX=1 CTKD_ENABLE=1 IS_CTKD_OVER_BR_EDR_ENABLED=1 IO_CAPABILITY_NO_INPUT_NO_OUTPUT_MITM_FALSE=1 \
                BLE_ADV_RPA_ENABLED=1 IS_BLE_FLAGS_ADV_DATA_CONFIGURED_BY_APP_LAYER=1 BLE_AOB_VOLUME_SYNC_ENABLED=0 \
                BLE_AUDIO_DOLPHIN_COMPATIBLE_SUPPORT=1 BLE_AUDIO_STARLORD_COMPATIBLE_SUPPORT=0 \
                 "

BTH_CUSTOMER_CFG="BES_OTA=1 BT_DIP_SUPPORT=1 IGNORE_POWER_ON_KEY_DURING_BOOT_UP=1 \
                IS_AUTOPOWEROFF_ENABLED=0 POWER_ON_ENTER_TWS_PAIRING_ENABLED=1 FREE_TWS_PAIRING_ENABLED=1 \
                AOB_MOBILE_ENABLED=0 IGNORE_POWER_ON_KEY_DURING_BOOT_UP=1 IS_AUTOPOWEROFF_ENABLED=0 APP_TRACE_RX_ENABLE=1 AUDIO_DEBUG_CMD=1 \
                BT_FA_ECC=0 APP_UI_TEST_MODE_ENABLE=1 \
                TRACE_BUF_SIZE=30*1024 "
OTA_COPY_OFFSET_CFG="OTA_CODE_OFFSET=0x18000 OTA_BOOT_SIZE=0x10000 OTA_BOOT_INFO_OFFSET=0x10000"

if [[ $CHIPID == "best1600" ]];
then
M55_DECODE_DECODER_CFG="DSP_M55=1 GAF_CODEC_CROSS_CORE=1 GAF_DECODER_CROSS_CORE_USE_M55=1 GAF_ENCODER_CROSS_CORE_USE_M55=1 A2DP_DECODER_CROSS_CORE_USE_M55=1 A2DP_DECODER_CROSS_CORE=1 "

BTH_CAPSENSOR_CFG=""

BTH_DECODE_M55_DECODER_CFG="DSP_M55=1 A2DP_CP_ACCEL=0 SCO_CP_ACCEL=0 CHIP_HAS_CP=0 GAF_CODEC_CROSS_CORE=1 GAF_DECODER_CROSS_CORE_USE_M55=1 GAF_ENCODER_CROSS_CORE_USE_M55=1 A2DP_DECODER_CROSS_CORE_USE_M55=1 A2DP_DECODER_CROSS_CORE=1 "
BTH_DECODE_CP_DECODER_CFG=""
LOW_PWR_CFG=""
BTH_CUSTOMER_CFG="${BTH_CUSTOMER_CFG} FAST_XRAM_SECTION_SIZE=0x19000"

elif [[ $CHIPID == "best1603" ]];
then
M55_DECODE_DECODER_CFG="DSP_M55=1 GAF_CODEC_CROSS_CORE=1 GAF_DECODER_CROSS_CORE_USE_M55=1 GAF_ENCODER_CROSS_CORE_USE_M55=1 "

BTH_CAPSENSOR_CFG="CAPSENSOR_ENABLE=1 CAPSENSOR_HAL_SPI=1  SPI_IOMUX_4WIRE=1 SPI_IOMUX_DI0_INDEX=1 CAPSENSOR_TOUCH=1 CAPSENSOR_WEAR=1 CAPSENSOR_SPP_SERVER=1 CAPSENSOR_AT_MCU=1 CHIP_CAPSENSOR_VER=2 CAPSENSOR_FP_MODE=1 CAPSENSOR_TRACE_DEBUG=0 SENSOR_HUB=1 SENS_TRC_TO_MCU=1 CORE_BRIDGE_DEMO_MSG=1 SENSOR_HUB_BSP_TEST=1"
BTH_DECODE_M55_DECODER_CFG="DSP_M55=1 GAF_CODEC_CROSS_CORE=1 GAF_DECODER_CROSS_CORE_USE_M55=1 GAF_ENCODER_CROSS_CORE_USE_M55=1 "
BTH_DECODE_CP_DECODER_CFG="A2DP_CP_ACCEL=1 CHIP_HAS_CP=1 AAC_REDUCE_SIZE=1"
# LOW_PWR_CFG="VCODEC_VOLT=1.35V CHIP_HAS_CP=1 A2DP_CP_ACCEL=1 AFH_ASSESS=0 "
LOW_PWR_CFG="CHIP_HAS_CP=1 A2DP_CP_ACCEL=1 AFH_ASSESS=0 "
BTH_CUSTOMER_CFG="${BTH_CUSTOMER_CFG} AUDIO_BUFFER_SIZE=90*1024 FAST_XRAM_SECTION_SIZE=0xea60 "
fi

################################### end of var define ###################################



################################### build  start ###################################

if [[ $RELEASE == "release" ]];
then

make T=prod_test/ota_copy GEN_LIB=1 DEBUG=1 -j8 CHIP=$CHIPID  $OTA_COPY_OFFSET_CFG
make T=prod_test/ota_copy GEN_LIB=1 DEBUG=1 -j8 CHIP=$CHIPID OTA_BIN_COMPRESSED=1 $OTA_COPY_OFFSET_CFG
make T=sensor_hub GEN_LIB=1 CHIP=$CHIPID BTH_AS_MAIN_MCU=1 SENS_TRC_TO_MCU=1 DEBUG=1 VOICE_DETECTOR_EN=0 -j8
make T=dsp_m55 GEN_LIB=1 CHIP=$CHIPID $M55_SYS_CFG $M55_DECODE_DECODER_CFG $M55_PSAP_SW_CFG -j8

CUSTOMER_TARGET_LIST=$TARGET_LIST

if [[ $CHIPID == "best1600" ]];
then
TARGET_LIST="best1600_ibrt"
elif [[ $CHIPID == "best1603" ]];
then
TARGET_LIST="best1603_ibrt"
fi

### basic ###
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG BLE=0 
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG BLE=1 
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG BLE=0 
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG BLE=1

make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG BLE=0 
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG BLE=1 
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG BLE=1 GFPS_ENABLE=1

make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG BLE=0 
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG BLE=1 
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG BLE=1 GFPS_ENABLE=1
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG BLE=1 GFPS_ENABLE=1 SASS_ENABLE=1

make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_LEA_ENABLE_CFG BLE=1 
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_LEA_ENABLE_CFG BLE=1 GFPS_ENABLE=1
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_LEA_ENABLE_CFG BLE=1 GFPS_ENABLE=1 SASS_ENABLE=1

make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_PSAP_SW_CFG BLE=0 
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_PSAP_SW_CFG BLE=1 
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_PSAP_SW_CFG BLE=1 GFPS_ENABLE=1
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_PSAP_SW_CFG BLE=1 GFPS_ENABLE=1 SASS_ENABLE=1

make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_PSAP_SW_CFG BLE=0 
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_PSAP_SW_CFG BLE=1 
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_PSAP_SW_CFG BLE=1 GFPS_ENABLE=1
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_PSAP_SW_CFG BLE=1 GFPS_ENABLE=1 SASS_ENABLE=1

make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_LEA_ENABLE_CFG $BTH_PSAP_SW_CFG BLE=1 
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_LEA_ENABLE_CFG $BTH_PSAP_SW_CFG BLE=1 GFPS_ENABLE=1
make T=$TARGET_LIST GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_LEA_ENABLE_CFG $BTH_PSAP_SW_CFG BLE=1 GFPS_ENABLE=1 SASS_ENABLE=1

### anc ###

make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG BLE=0 
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG BLE=1 
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG BLE=0 
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG BLE=1 

make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG BLE=0 
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG BLE=1 
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG BLE=1 GFPS_ENABLE=1
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG BLE=1 GFPS_ENABLE=1 SASS_ENABLE=1

make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_DECODE_M55_DECODER_CFG BLE=0 
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_DECODE_M55_DECODER_CFG BLE=1 
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_DECODE_M55_DECODER_CFG BLE=1 GFPS_ENABLE=1
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_DECODE_M55_DECODER_CFG BLE=1 GFPS_ENABLE=1 SASS_ENABLE=1

make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_LEA_ENABLE_CFG BLE=1 
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_LEA_ENABLE_CFG BLE=1 GFPS_ENABLE=1
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_LEA_ENABLE_CFG BLE=1 GFPS_ENABLE=1 SASS_ENABLE=1

make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_PSAP_SW_CFG BLE=0 
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_PSAP_SW_CFG BLE=1 
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_PSAP_SW_CFG BLE=1 GFPS_ENABLE=1
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_PSAP_SW_CFG BLE=1 GFPS_ENABLE=1 SASS_ENABLE=1

make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_PSAP_SW_CFG BLE=0 
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_PSAP_SW_CFG BLE=1 
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_PSAP_SW_CFG BLE=1 GFPS_ENABLE=1
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_PSAP_SW_CFG BLE=1 GFPS_ENABLE=1 SASS_ENABLE=1

make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_LEA_ENABLE_CFG $BTH_PSAP_SW_CFG  BLE=1 
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_LEA_ENABLE_CFG $BTH_PSAP_SW_CFG  BLE=1 GFPS_ENABLE=1
make T=${TARGET_LIST}_anc GEN_LIB=1 -s -j8 $BTH_SYS_CFG $BTH_CAPSENSOR_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_ANC_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_LEA_ENABLE_CFG $BTH_PSAP_SW_CFG  BLE=1 GFPS_ENABLE=1 SASS_ENABLE=1

#################  customer feature  ###############
TARGET_LIST=$CUSTOMER_TARGET_LIST
make T=$TARGET_LIST GEN_LIB=1  -j8 



[[ $SECURITY_ENABLE == security_enable ]] &&{
# SECURE_BOOT_FLAGS="SECURE_BOOT=1 USER_SECURE_BOOT=1"
# CRYPT_LOAD_FLAGS="LARGE_SE_RAM=1"
PROJECT_FUNC_DEMO="SE_OTP_DEMO_TEST=1 CMSE_CRYPT_TEST_DEMO=1 FLASH_SECURITY_REGISTER=1 MBEDTLS_CONFIG_FILE="config-rsa.h""

CUSTOMER_LOAD_SECTION_FLAGS="CUSTOMER_LOAD_SRAM_TEXT_RAMX_SECTION_SIZE=0x1000 CUSTOMER_LOAD_RAM_DATA_SECTION_SIZE=0x1000 CUSTOMER_LOAD_ENC_DEC_RECORD_SECTION_SIZE=0x4000"
PROJECT_CFG_FLAGS="FLASH_SIZE=0x800000 SPA_AUDIO_ENABLE=1 SPA_AUDIO_SEC=1 "
# CP_DISABLE_FLAGS="CHIP_HAS_CP=0 A2DP_CP_ACCEL=0 SCO_CP_ACCEL=0 NO_OVERLAY=1"

#EXTRA_PROJECT_FEATURE_CFG_FLAGS="SPEECH_TX_2MIC_NS7=1 SPEECH_TX_MIC_FIR_CALIBRATION=1 TOTA_v2=1 CUSTOMER_APP_BOAT=1 GFPS_ENABLE=1"

CHIP_TYPES="CHIP=best1600 "
CHIP_BASIC_CFG=""

#########################   arm_cmse    #########################
CHIP_BASIC_CFG="DEBUG_PORT=1 CHIP=best1600 USE_MEM_CFG=1 FLASH_SIZE=0x800000 TRACE_BAUD_RATE=1152000"
make T=arm_cmse DEBUG=1 -j8 FLASH_SECURITY_REGISTER=1 CRC32_ROM=1 GEN_LIB=1 \
    $CHIP_TYPES $CHIP_BASIC_CFG\
    $PROJECT_CFG_FLAGS $CRYPT_LOAD_FLAGS $PROJECT_FUNC_DEMO $SECURE_BOOT_FLAGS 

#########################   arm_cmns    #########################
CHIP_BASIC_CFG="CHIP_SUBSYS= BTH_AS_MAIN_MCU=0 BTH_USE_SYS_PERIPH=0 DPD_ONCHIP_CAL=0 NO_MPU_DEFAULT_MAP=0 FLASH_SIZE=0x800000 TRACE_BAUD_RATE=1152000"
make T=best1600_ibrt $CHIP_BASIC_CFG DEBUG=1 -j8 ARM_CMNS=1 GEN_LIB=1 \
    POWER_ON_ENTER_TWS_PAIRING_ENABLED=1 IGNORE_POWER_ON_KEY_DURING_BOOT_UP=1 OTA_CODE_OFFSET=0x80000 \
    $CP_DISABLE_FLAGS \
    $PROJECT_CFG_FLAGS $CRYPT_LOAD_FLAGS $SECURE_BOOT_FLAGS USER_SECURE_BOOT=0
}

else

hearing_aid_customer=" \
                    $LOW_PWR_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG \
                    $BTH_ALGO_MCPP $BTH_LEA_ENABLE_CFG \
                    $BTH_PSAP_SW_CFG "
audio_customer=" \
                    $LOW_PWR_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG \
                    $BTH_ALGO_MCPP $BTH_LEA_ENABLE_CFG \
                    $BTH_CAPSENSOR_CFG $BTH_ANC_CFG "

make T=prod_test/ota_copy  DEBUG=1 -j8 CHIP=$CHIPID $OTA_COPY_OFFSET_CFG
make T=sensor_hub  CHIP=$CHIPID BTH_AS_MAIN_MCU=1 SENS_TRC_TO_MCU=1 DEBUG=1 VOICE_DETECTOR_EN=0 CORE_BRIDGE_DEMO_MSG=1 SENSOR_HUB_BSP_TEST=1 -j8

# build_cmd="make T=$TARGET_LIST CHIP=$CHIPID $BTH_SYS_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_M55_DECODER_CFG $BTH_CAPSENSOR_CFG \
#     $BTH_PSAP_SW_CFG \
#     $LOW_PWR_CFG \
#     $BTH_DECODE_CP_DECODER_CFG \
#     $BTH_ANC_CFG \
#     $BTH_LEA_ENABLE_CFG -j8"

if [[ $TARGET_CUSTOMER == "audio_customer" ]];
then
m55_build_cmd="make T=dsp_m55  CHIP=$CHIPID $M55_DECODE_DECODER_CFG  $M55_SYS_CFG  $M55_ALGO_MCPP -j8 "

bth_build_cmd="make T=$TARGET_LIST CHIP=$CHIPID $BTH_SYS_CFG $BTH_CUSTOMER_CFG  -j8 \
            $audio_customer"
elif [[ $TARGET_CUSTOMER == "hearing_aid_customer" ]];
then
m55_build_cmd="make T=dsp_m55  CHIP=$CHIPID $M55_DECODE_DECODER_CFG  $M55_SYS_CFG $M55_ALGO_MCPP $M55_PSAP_SW_CFG -j8 "

bth_build_cmd="make T=$TARGET_LIST CHIP=$CHIPID $BTH_SYS_CFG $BTH_CUSTOMER_CFG  -j8 \
            $hearing_aid_customer"
fi

$m55_build_cmd
$bth_build_cmd
echo "m55_build_cmd: "
echo "$m55_build_cmd"
echo "bth_build_cmd: "
echo "$bth_build_cmd"
fi

################################### end of build  start ###################################

################################### release script ###################################
if [[ $RELEASE == "release" ]];
then


. `dirname $0`/relsw_ibrt_common.sh

fi
################################### end of release script ###################################
