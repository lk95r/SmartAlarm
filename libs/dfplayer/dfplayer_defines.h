#pragma once


/**Protocol Bytes*/
#define DFP_START_BYTE 0x7E
#define DFP_VERSION 0xFF
#define DFP_END_BYTE 0xEF
#define DFP_DEFAULT_MSG_LEN 10

#define DFP_ACK 0x00
#define DEFAULT_CMD_LEN 0x06

/**Commands to Send, no params returned**/
#define CMD_NEXT 0x01
#define CMD_PREVIOS 0x02
#define CMD_TRACK_NO 0x03 // Specify tracking(num) Par: 0-2999
#define CMD_VOLUME_INC 0x04
#define CMD_VOLUME_DEC 0x05
#define CMD_VOLUME_SET 0x06 //Par: 0-30
#define CMD_SET_EQ 0x07 // 0-Normal/1-Pop/2-Rock/3-Jazz/4-Classic/5-Base
#define CMD_SET_PB_MODE 0x08 //Speficy playback mode 0-repeat/1-folder repeat/2-single repeat/3-random
#define CMD_SET_SOURCE 0x09 // Playback source: 0U/1-TF/2-AUX/3-SLEEP/4-FLASH
#define CMD_ENTER_STBY 0x0A // Enter Standby
#define CMD_NORMAL 0x0B // Normal working
#define CMD_RESET_MODULE 0x0C // Reset Module
#define CMD_PLAYBACK 0x0D // Playback
#define CMD_PAUSE 0x0E // Pause
#define CMD_SET_FOLDER 0x0F // Choose Folder Par: 0~10
#define CMD_ADJ_VOLUME 0x10 // DH=1: OPEN Volume Adjust, DL: Set volume gaim 0~31
#define CMD_REPEAT 0x11// Par: 1 repeat play, 0: no repeat

/**Query System Parameters**/
#define QCMD_STAY0 0x3C
#define QCMD_STAY1 0x3D
#define QCMD_STAY2 0x3E
#define QCMD_SEND_INIT_PARAMS 0x3F //0-0x0F (each bit represents one device of the low-for bits)
#define QCMD_REQ_RETRANS 0x40 // Returns an error, request retransmission
#define QCMD_REPLY 0x41
#define QCMD_STATUS_CUR 0x42 // Query current status
#define QCMD_VOLUME_CUR 0x43 // Query current Volume
#define QCMD_EQ_CUR 0x45 // Query current EQ
#define QCMD_PB_MODE_CUR 0x45 // Query current playback mode
#define QCMD_SW_VERS_CUR 0x46 // Query current software version
#define QCMD_NOF_FILES_TF 0x47 // Query number of files on tf-card
#define QCMD_NOF_FILES_U 0x48 // Query number of files on u-disk
#define QCMD_NOF_FILES_flash 0x49 // Query number of files in flash
#define QCMD_KEEP_ON 0x4A
#define QCMD_TRACK_CUR_TF 0x4B //Query current track of tf card
#define QCMD_TRACK_CUR_U 0x4C //Query current track of u-disk
#define QCMD_TRACK_CUR_FLASH 0x4D // Query current track of flash
