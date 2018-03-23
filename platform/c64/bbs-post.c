/**
 * \file
 *         bbs-post.c - post msg. to Contiki BBS message boards 
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#include "contiki.h"
#include "shell.h"

#include "bbs-post.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern BBS_STATUS_REC bbs_status;
extern BBS_USER_REC bbs_user;
/*extern char bbs_logbuf[BBS_MAX_MSGLINES][BBS_LINE_WIDTH];*/

PROCESS(bbs_post_process, "post");
SHELL_COMMAND(bbs_post_command, "p", "p : post a new message", &bbs_post_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_post_process, ev, data)
{

  static short linecount=0;
  static short disk_access=1;
  struct shell_input *input;
  //static char bbs_logbuf[BBS_MAX_MSGLINES*BBS_LINE_WIDTH];
  char bbs_logbuf[1024];
  ST_FILE file;
  //BBS_BOARD_REC board;

  /* read board data */
/*  if (disk_access) {
     strcpy(file.szFileName, BBS_BOARDCFG_FILE);
     file.ucDeviceNo=BBS_SYS_DEVICE;
     ssReadRELFile(&file, &board, sizeof(BBS_BOARD_REC), bbs_status.bbs_board_id);
     disk_access=0;
  }
*/  PROCESS_BEGIN();

//process_exit(&shell_server_process);

  shell_output_str(NULL,PETSCII_LOWER, PETSCII_WHITE);
  shell_output_str(&bbs_post_command, "on empty line: /a=abort /s=save\r\n", "");
  shell_output_str(&bbs_post_command, BBS_STRING_EDITHDR, "");
  sprintf(bbs_logbuf,"\n\rmsg from: %s\n\r\n\r", bbs_user.user_name);

 
  PROCESS_PAUSE();

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(ev == shell_event_input);
    input = data;

    if (! strcmp(input->data1, "/a") ) {
       linecount=0;
       disk_access=1;
       PROCESS_EXIT();
    }

    if (! strcmp(input->data1,"/s") || linecount >= BBS_MAX_MSGLINES) {

      /* write post */
      ++bbs_status.bbs_msg_id[bbs_status.bbs_board_id];

      sprintf(file.szFileName, "%d-%d", bbs_status.bbs_board_id, bbs_status.bbs_msg_id[bbs_status.bbs_board_id]);
      
      cbm_save (file.szFileName, BBS_SUBS_DEVICE, &bbs_logbuf, sizeof(bbs_logbuf));

      log_message("[bbs] *post* ", bbs_logbuf);

/*
      sprintf(file.szFileName,"S0:%s", BBS_CFG_FILE);  
      cbm_open( 15, BBS_SYS_DEVICE , 15, file.szFileName);
      cbm_close(15); 

      sprintf(file.szFileName, "%s", BBS_CFG_FILE);
      cbm_save (file.szFileName, BBS_SYSTEM_DRIVE, &bbs_status, sizeof(struct bbs_status));
*/
      memset(bbs_logbuf, 0, sizeof(bbs_logbuf));
      linecount=0;
      disk_access=1;

      PROCESS_EXIT();
    }

    strcat(bbs_logbuf, input->data1);
    //log_message("[bbs] *post* ", bbs_logbuf);
    /*if (linecount <= BBS_MAX_MSGLINES) {
       disk_access=0;
       strncpy(bbs_logbuf[linecount], input->data1, BBS_LINE_WIDTH);
       linecount++;
    }*/
 } /* end ... while */

 PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
bbs_post_init(void)
{
  shell_register_command(&bbs_post_command);
}
