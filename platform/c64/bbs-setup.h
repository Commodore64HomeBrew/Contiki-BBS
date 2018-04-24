/**
 * \file
 *         bbs-setup.h - main program of the Contiki BBS setup program - header file
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#ifndef __BBS_SETUP_H__
#define __BBS_SETUP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bbs-file.h"
#include "bbs-defs.h"

typedef struct {
   unsigned int   srvip[4];
   unsigned int   netmask[4];
   unsigned int   gateway[4];
   unsigned int   nameserv[4];
   unsigned int   mem;
   unsigned char  driver[15];
} CTK_CFG_REC; 

void mainMenu(void);
void scrollScreen(void);
int nibbleIP(unsigned char *src, unsigned int *addr);
int networkSetup(unsigned short drive);
int baseSetup(void);
int boardSetup(unsigned short drive);
int userSetup(unsigned short drive);
int enterUserData(BBS_USER_REC *rec, unsigned short *user_count);
#endif /* __BBS_SETUP_H__ */
