/**
 * \file
 *         bbsefs.h - Contiki BBS functions and types - header file
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#ifndef __BBSDEFS_H_
#define __BBSDEFS_H_

#define BBS_COPYRIGHT_STRING "\n\r          Contiki BBS 0.3.0 \n\r     (c) 2018-> by K. Casteels\n\r     (c) 2009-2015 by N. Haedecke\n\r           based on Contiki OS,\n\r     (c) 2003-2013 by Adam Dunkels\n\r"

#define BBS_NAME "\n\r     CENTRONIAN BBS\n\r"
#define BBS_TELNET_PORT      2200

#define BBS_LOCKMODE_OFF        0
#define BBS_LOCKMODE_ON         1

#define BBS_MODE_SHELL          0
#define BBS_MODE_CONSOLE        1

#define BBS_MAX_BOARDS          8
#define BBS_MAX_MSGLINES        6

#define BBS_BOARD_DRIVE         9
//#define BBS_BOARD_PREFIX        ""

#define BBS_SUBS_DRIVE          8
//#define BBS_SUBS_PREFIX         "0:"

#define BBS_SYSTEM_DRIVE        9

#define BBS_LINE_WIDTH         40
#define BBS_TIMEOUT_SEC       600
#define BBS_LOGIN_TIMEOUT_SEC  60

#define BBS_BANNER_BUFFER    1024
#define BBS_BANNER_LOGIN_a     "login-a"
#define BBS_BANNER_LOGIN_p     "login-p"

#define BBS_BANNER_LOGOUT_a    "logout-a"
#define BBS_BANNER_LOGOUT_p    "logout-p"

#define BBS_BANNER_LOGO_a      "logo-a"
#define BBS_BANNER_LOGO_p      "logo-p"

#define BBS_BANNER_MENU_a      "menu-a"
#define BBS_BANNER_MENU_p      "menu-p"

#define BBS_BANNER_SUBS_a      "subs-a"
#define BBS_BANNER_SUBS_p      "subs-p"

#define BBS_PREFIX_SUB_a       "sa-"
#define BBS_PREFIX_SUB_p       "sp-"

#define BBS_CFG_FILE           "bbs-cfg"
//#define BBS_BOARDCFG_FILE      "board-cfg"

#define BBS_STRING_BOARDINFO "-id- -------board------- -acl- -msgs-"
#define BBS_STRING_LINEMAX "  enter message (max. 40 chars per line)"
#define BBS_STRING_EDITHDR "---------+---------+---------+---------+" 
#define BBS_STRING_VERSION "0.3.0"

#define PETSCII_LOWER           "\x0e"
#define PETSCII_CLRSCN          "\x93"
#define PETSCII_WHITE           "\x05"

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
  unsigned short user_no;  
  char  user_name[14];
  char  user_pwd[14];
  unsigned char  access_req;
} BBS_USER_REC;

typedef struct {
  unsigned char board_drive;
  unsigned char subs_drive;
  unsigned short bbs_timeout_login;
  unsigned short bbs_timeout_session;
  unsigned char bbs_status;
  short bbs_msg_id[BBS_MAX_BOARDS];
  unsigned char bbs_board_id;
  unsigned char bbs_encoding;
  /*char bbs_name[20];
  char bbs_sysop[20];*/
  char bbs_last_caller[14];
  char bbs_prompt[10];
} BBS_STATUS_REC;

#endif /* __BBSDEFS_H_ */
