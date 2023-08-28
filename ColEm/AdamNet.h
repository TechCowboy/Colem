/** ColEm: portable Coleco emulator **************************/
/**                                                         **/
/**                       AdamNet.h                         **/
/**                                                         **/
/** This file contains declarations for the AdamNet I/O     **/
/** interface found in Coleco Adam home computer.           **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2021                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
#ifndef ADAMNET_H
#define ADAMNET_H
#ifdef __cplusplus
extern "C" {
#endif

/** Adam Key Codes *******************************************/
#define KEY_CONTROL    CON_CONTROL
#define KEY_SHIFT      CON_SHIFT
#define KEY_CAPS       CON_CAPS
#define KEY_ESC        27
#define KEY_BS         8   // + SHIFT = 184
#define KEY_TAB        9   // + SHIFT = 185
#define KEY_ENTER      13 
#define KEY_QUOTE      '\''
#define KEY_BQUOTE     '`'
#define KEY_BSLASH     '\\'
#define KEY_COMMA      ','
#define KEY_DOT        '.'
#define KEY_SLASH      '/'
#define KEY_ASTERISK   '*'
#define KEY_HOME       128
#define KEY_F1         129 // + SHIFT = 137 
#define KEY_F2         130 // + SHIFT = 138
#define KEY_F3         131 // + SHIFT = 139
#define KEY_F4         132 // + SHIFT = 140
#define KEY_F5         133 // + SHIFT = 141
#define KEY_F6         134 // + SHIFT = 142
#define KEY_WILDCARD   144 // + SHIFT = 152
#define KEY_UNDO       145 // + SHIFT = 153
#define KEY_MOVE       146 // + SHIFT = 154 (COPY)
#define KEY_STORE      147 // + SHIFT = 155 (FETCH)
#define KEY_INS        148 // + SHIFT = 156
#define KEY_PRINT      149 // + SHIFT = 157
#define KEY_CLEAR      150 // + SHIFT = 158
#define KEY_DEL        151 // + SHIFT = 159, + CTRL = 127
#define KEY_UP         160 // + CTRL = 164, + HOME = 172
#define KEY_RIGHT      161 // + CTRL = 165, + HOME = 173
#define KEY_DOWN       162 // + CTRL = 166, + HOME = 174
#define KEY_LEFT       163 // + CTRL = 167, + HOME = 175
#define KEY_DIAG_NE    168
#define KEY_DIAG_SE    169
#define KEY_DIAG_SW    170
#define KEY_DIAG_NW    171

#ifndef BYTE_TYPE_DEFINED
#define BYTE_TYPE_DEFINED
typedef unsigned char byte;
#endif

#ifndef WORD_TYPE_DEFINED
#define WORD_TYPE_DEFINED
typedef unsigned short word;
#endif

/** ReadPCB() ************************************************/
/** Read value from a given PCB or DCB address.             **/
/*************************************************************/
void ReadPCB(word A);

/** WritePCB() ***********************************************/
/** Write value to a given PCB or DCB address.              **/
/*************************************************************/
void WritePCB(word A,byte V);

/** ResetPCB() ***********************************************/
/** Reset PCB and attached hardware.                        **/
/*************************************************************/
void ResetPCB(void);

/** PutKBD() *************************************************/
/** Add a new key to the keyboard buffer.                   **/
/*************************************************************/
void PutKBD(unsigned int Key);

void ReportDevice(byte Dev, word MsgSize, byte IsBlock);
void SetDCB(byte Dev, byte Offset, byte Value);

/** PCB Field Offsets ****************************************/
#define PCB_CMD_STAT 0
#define PCB_BA_LO 1
#define PCB_BA_HI 2
#define PCB_MAX_DCB 3
#define PCB_SIZE 4

/** DCB Field Offsets ****************************************/
#define DCB_CMD_STAT 0
#define DCB_BA_LO 1
#define DCB_BA_HI 2
#define DCB_BUF_LEN_LO 3
#define DCB_BUF_LEN_HI 4
#define DCB_SEC_NUM_0 5
#define DCB_SEC_NUM_1 6
#define DCB_SEC_NUM_2 7
#define DCB_SEC_NUM_3 8
#define DCB_DEV_NUM 9
#define DCB_RETRY_LO 14
#define DCB_RETRY_HI 15
#define DCB_ADD_CODE 16
#define DCB_MAXL_LO 17
#define DCB_MAXL_HI 18
#define DCB_DEV_TYPE 19
#define DCB_NODE_TYPE 20
#define DCB_SIZE 21

/** PCB Commands *********************************************/
#define CMD_PCB_IDLE 0x00
#define CMD_PCB_SYNC1 0x01
#define CMD_PCB_SYNC2 0x02
#define CMD_PCB_SNA 0x03
#define CMD_PCB_RESET 0x04
#define CMD_PCB_WAIT 0x05

/** DCB Commands *********************************************/
#define CMD_RESET 0x00
#define CMD_STATUS 0x01
#define CMD_ACK 0x02
#define CMD_CLEAR 0x03
#define CMD_RECEIVE 0x04
#define CMD_CANCEL 0x05
#define CMD_SEND 0x06 /* + SIZE_HI + SIZE_LO + DATA + CRC */
#define CMD_NACK 0x07

#define CMD_SOFT_RESET 0x02
#define CMD_WRITE 0x03
#define CMD_READ 0x04

/** Response Codes *******************************************/
#define RSP_STATUS 0x80 /* + SIZE_HI + SIZE_LO + TXCODE + STATUS + CRC */
#define RSP_ACK 0x90
#define RSP_CANCEL 0xA0
#define RSP_SEND 0xB0 /* + SIZE_HI + SIZE_LO + DATA + CRC */
#define RSP_NACK 0xC0


#ifdef __cplusplus
}
#endif
#endif /* ADAMNET_H */
