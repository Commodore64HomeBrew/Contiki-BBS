/**
 * \file
 *         bbsefs.h - Contiki BBS functions and types - header file
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#ifndef __BBSDEFS_H_
#define __BBSDEFS_H_

#define BBS_COPYRIGHT_STRING "\n\r          Contiki BBS 0.2.7.1 \n\r     (c) 2009-2015 by N. Haedecke\n\r           based on Contiki OS,\n\r     (c) 2003-2013 by Adam Dunkels\n\r"

#define BBS_LOCKMODE_OFF        0
#define BBS_LOCKMODE_ON         1

#define BBS_MODE_SHELL          0
#define BBS_MODE_CONSOLE        1

#define BBS_MAX_BOARDS         20
#define BBS_MAX_MSGLINES        6
#define BBS_MAX_USERS         500

#define BBS_BANNER_BUFFER    1024
#define BBS_BANNER_LOGIN     "login.txt"
#define BBS_BANNER_LOGOUT    "logout.txt"
#define BBS_BANNER_MENU      "menu.txt"

#define BBS_LINE_WIDTH         40
#define BBS_TIMEOUT_SEC       360
#define BBS_LOGIN_TIMEOUT_SEC  60

#define BBS_CFG_FILE "bbs.cfg"
#define BBS_BOARDCFG_FILE "board.cfg"

#define BBS_STRING_BOARDINFO "-id- -------board------- -acl- -msgs-"
#define BBS_STRING_LINEMAX "  enter message (max. 40 chars per line)"
#define BBS_STRING_EDITHDR "---------+---------+---------+---------+" 
#define BBS_STRING_VERSION "0.3.0"

typedef struct {
  unsigned short board_no;  
  unsigned short max_boards;
  unsigned short board_max;
  unsigned short board_ptr;
  unsigned short access_req;
  char  board_name[20];
} BBS_BOARD_REC;

typedef struct {
  unsigned short user_no;  
  char  user_name[15];
  char  user_pwd[15];
  unsigned short  access_req;
} BBS_USER_REC;

typedef struct {
  unsigned short board_drive;  
  unsigned short bbs_timeout_login;
  unsigned short bbs_timeout_session;
  unsigned short bbs_status;
  unsigned short bbs_msg_id;
  unsigned short bbs_board_id;
  /*char  bbs_name[20];
  char  bbs_sysop[20];*/
  char  bbs_prompt[20];
  unsigned short bbs_encoding;
} BBS_STATUS_REC;

#endif /* __BBSDEFS_H_ */
