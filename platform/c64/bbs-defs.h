/**
 * \file
 *         bbsefs.h - Contiki BBS functions and types - header file
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#ifndef __BBSDEFS_H_
#define __BBSDEFS_H_

#define BBS_STRING_VERSION "0.1.0"
#define BBS_COPYRIGHT_STRING "\n\r        magnetar bbs 0.1.0 \n\r     (c) 2018-> by k. casteels\n\r           based on contiki bbs,\n\r     (c) 2009-2015 by n. haedecke\n\r           based on contiki os,\n\r     (c) 2003-2013 by adam dunkels\n\r"

//#define BBS_ENCODING_STRING "\n\rpetscii - 40col (4)\n\rpetscii - 22col (2)\n\rascii w/ echo   (e)\n\rlinux or vt100  (l)\n\ratascii w/echo  (t)\n\r\n\r>  "
#define BBS_ENCODING_STRING "\n\rpetscii - 80col (8)\n\rpetscii - 40col (4)\n\rpetscii - 22col (2)\n\rascii w/ echo   (e)\n\rlinux or vt100  (l)\n\r\n\r>  "


#define BBS_LOCKMODE_OFF        0
#define BBS_LOCKMODE_ON         1

#define BBS_MODE_SHELL          0
#define BBS_MODE_CONSOLE        1

#define BBS_MAX_BOARDS          8
#define BBS_MAX_MSGLINES        20
#define BBS_80_COL             78
#define BBS_40_COL	           38
#define BBS_22_COL	           20
#define TELNETD_CONF_LINELEN 80
#define TELNETD_CONF_NUMLINES 25

#define BBS_BUFFER_SIZE    1550

#define BBS_SESSION_TIMEOUT (CLOCK_SECOND * 3600)
#define BBS_LOGIN_TIMEOUT   (CLOCK_SECOND * 60)
#define BBS_IDLE_TIMEOUT    (CLOCK_SECOND * 120)



#define BBS_PET80_SUFFIX       "-c"
#define BBS_PET40_SUFFIX       "-c"
#define BBS_PET22_SUFFIX       "-v"
#define BBS_ASCII_SUFFIX       "-a"
//#define BBS_BASH_SUFFIX       "-b"
#define BBS_PREFIX_SUB         "s-"
#define BBS_PREFIX_USER        "u-"


#define BBS_BANNER_LOGIN       "login"
#define BBS_BANNER_LOGOUT      "logout"
#define BBS_BANNER_LOGO        "logo"
#define BBS_BANNER_MENU        "menu"
#define BBS_BANNER_SUBS        "subs"

#define BBS_LOG_FILE           "bbs.log"

#define BBS_EMD_FILE           "c64-ram.emd"
#define BBS_CFG_FILE           "bbs-cfg"
//#define BBS_BOARDCFG_FILE      "board-cfg"

//#define BBS_STRING_BOARDINFO "-id- -------board------- -acl- -msgs-"
//#define BBS_STRING_LINEMAX "  enter message (max. 40 chars per line)"
#define BBS_STRING_EDITH40 "---------+---------+---------+---------+"
#define BBS_STRING_EDITH22 "----------+----------+"


#define STATUS_UNLOCK	0
#define STATUS_HANDLE	1
#define STATUS_PASSWD	2
#define STATUS_NEWUSR	3
#define STATUS_CONFUSR	4
#define STATUS_LOCK		5
#define STATUS_SUBJ		6
#define STATUS_POST		7
#define STATUS_READ		8

#define PETSCII_LOWER           "\x0e"
#define PETSCII_WHITE           "\x05"
#define ISO_nl       	0x0a
#define ISO_cr       	0x0d
#define PETSCII_DEL  	0x14
#define PETSCII_DOWN 	0x11
#define PETSCII_UP   	0x91
#define PETSCII_LEFT 	0x9d
#define PETSCII_RIGHT 	0x1d
#define PETSCII_SPACE 	0x20
#define PETSCII_CLRSCN  0x93
#define PETSCII_HOME  0x13 

#define poke(A,X) (*(unsigned short *)A) = (X)
#define peek(A) (*(unsigned short *)A)


typedef struct {
  char  board_name[40];
  short telnet_port;
  unsigned char max_boards;
  unsigned char subs_device;
  char subs_prefix[10];
  unsigned char sys_device;
  char sys_prefix[10];
  unsigned char user_device;
  char user_prefix[10];
  char sub_names[9][20];
  unsigned char dir_boost; 
} BBS_BOARD_REC;


typedef struct {
  short msg_id[BBS_MAX_BOARDS+1];
} BBS_CONFIG_REC;

typedef struct {  
  char  user_name[12];
  char  user_pwd[20];
  unsigned char  access_req;
} BBS_USER_REC;

typedef struct {
  short num_calls;
  int last_call;
  short current_msg[BBS_MAX_BOARDS+1];
} BBS_USER_STATS;

typedef struct {
  unsigned char status;
  unsigned char board_id;
  unsigned char encoding;
  unsigned char echo;
  unsigned char wrap;
  unsigned char width;
  unsigned char lines;
  unsigned short msg_size;
  char last_caller[12];
  char prompt[40];
  char encoding_suffix[3];
} BBS_STATUS_REC;

typedef struct {
  unsigned short year;
  unsigned char month;
  unsigned char day;
  unsigned char hour;
  unsigned char minute;
} BBS_TIME_REC;


typedef struct {
  short file[10][2];
} BBS_EM_REC;


typedef struct {
  char bufmem[BBS_BUFFER_SIZE];
  unsigned long ptr;
  unsigned long size;
} BBS_BUFFER;

typedef struct {
  char buf[TELNETD_CONF_LINELEN + 1];
  char bufptr;
  char connected;
  long numsent;
  short state;
} TELNETD_STATE;

#endif /* __BBSDEFS_H_ */
