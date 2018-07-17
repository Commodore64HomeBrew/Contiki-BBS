/**
 * \file
 *         bbsefs.h - Contiki BBS functions and types - header file
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#ifndef __BBSDEFS_H_
#define __BBSDEFS_H_

#define BBS_COPYRIGHT_STRING "\n\r        Centronian BBS 0.1.0 \n\r     (c) 2018-> by K. Casteels\n\r           based on Contiki BBS,\n\r     (c) 2009-2015 by N. Haedecke\n\r           based on Contiki OS,\n\r     (c) 2003-2013 by Adam Dunkels\n\r"

#define BBS_ENCODING_STRING "\n\rpetscii - 40col (4)\n\rpetscii - 22col (2)\n\rascii w/ echo   (e)\n\rlinux or vt100  (l)\n\ratascii w/echo  (t)\n\r\n\r>  "

#define BBS_NAME "\n\r     CENTRONIAN BBS\n\r"
#define BBS_TELNET_PORT      2200

#define BBS_LOCKMODE_OFF        0
#define BBS_LOCKMODE_ON         1

#define BBS_MODE_SHELL          0
#define BBS_MODE_CONSOLE        1

#define BBS_MAX_BOARDS          8
#define BBS_MAX_MSGLINES        20

#define BBS_SUBS_DEVICE         8
#define BBS_SUBS_PREFIX         "0:"

#define BBS_SYS_DEVICE          8
#define BBS_SYS_PREFIX          "0:"

#define BBS_40_COL	           39
#define BBS_22_COL	           21
#define BBS_LINE_WIDTH         40
#define BBS_TIMEOUT_SEC       600
#define BBS_LOGIN_TIMEOUT_SEC  60

#define BBS_BANNER_BUFFER    1024

#define BBS_PET40_SUFFIX       "-c"
#define BBS_PET22_SUFFIX       "-v"
#define BBS_ASCII_SUFFIX       "-a"
//#define BBS_BASH_SUFFIX       "-b"
#define BBS_PREFIX_SUB         "s-"

#define BBS_BANNER_LOGIN       "login"
#define BBS_BANNER_LOGOUT      "logout"
#define BBS_BANNER_LOGO        "logo"
#define BBS_BANNER_MENU        "menu"
#define BBS_BANNER_SUBS        "subs"


#define BBS_EMD_FILE           "c64-ram.emd"
#define BBS_CFG_FILE           "bbs-cfg"
//#define BBS_BOARDCFG_FILE      "board-cfg"

//#define BBS_STRING_BOARDINFO "-id- -------board------- -acl- -msgs-"
//#define BBS_STRING_LINEMAX "  enter message (max. 40 chars per line)"
#define BBS_STRING_EDITHDR "---------+---------+---------+---------+" 
#define BBS_STRING_VERSION "0.3.0"

#define STATUS_UNLOCK   0
#define STATUS_HANDLE	1
#define STATUS_PASSWD	2
#define STATUS_LOCK		3
#define STATUS_POST		4
#define STATUS_READ		5

#define PETSCII_LOWER           "\x0e"
#define PETSCII_CLRSCN          "\x93"
#define PETSCII_WHITE           "\x05"
#define ISO_nl       	0x0a
#define ISO_cr       	0x0d
#define PETSCII_DEL  	0x14
#define PETSCII_DOWN 	0x11
#define PETSCII_UP   	0x91
#define PETSCII_LEFT 	0x9d
#define PETSCII_RIGHT 	0x1d
#define PETSCII_SPACE 	0x20

/*
typedef struct {
  unsigned char board_no;  
  unsigned char max_boards;
  unsigned short board_max;
  unsigned short board_ptr;
  unsigned char access_req;
  char  board_name[20];
} BBS_BOARD_REC;
*/

typedef struct {
  short msg_id[BBS_MAX_BOARDS+1];
  /*char bbs_name[20];
  char bbs_sysop[20];*/
} BBS_CONFIG_REC;

typedef struct {
//  unsigned short user_no;  
  char  user_name[12];
  char  user_pwd[20];
  short msg_id[BBS_MAX_BOARDS+1];
//  unsigned char  access_req;
} BBS_USER_REC;

typedef struct {
  unsigned char status;
  unsigned char board_id;
  unsigned char current_msg[BBS_MAX_BOARDS+1];
  unsigned char encoding;
  unsigned char echo;
  unsigned char width;
  unsigned short msg_size;
  char last_caller[12];
  char prompt[40];
  char encoding_suffix[3];
} BBS_STATUS_REC;

typedef struct {
  short file[10][2];
} BBS_EM_REC;

#endif /* __BBSDEFS_H_ */
