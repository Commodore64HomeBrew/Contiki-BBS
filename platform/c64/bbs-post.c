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
/*extern char bbs_logbuf[BBS_MAX_MSGLINES][BBS_LINE_WIDTH];*/

PROCESS(bbs_post_process, "post");
SHELL_COMMAND(bbs_post_command, "p", "p : post a new message", &bbs_post_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_post_process, ev, data)
{

  static short linecount=0;
  static short disk_access=1;
  struct shell_input *input;
  static char bbs_logbuf[BBS_MAX_MSGLINES][BBS_LINE_WIDTH];
  ST_FILE file;
  BBS_BOARD_REC board;

  /* read board data */
  if (disk_access) {
     strcpy(file.szFileName, BBS_BOARDCFG_FILE);
     file.ucDeviceNo=bbs_status.board_drive;
     ssReadRELFile(&file, &board, sizeof(BBS_BOARD_REC), bbs_status.bbs_board_id);
     disk_access=0;
  }

  PROCESS_BEGIN();

  shell_output_str(&bbs_post_command, "on empty line: /abt=abort /s=save\r\nlns:6, chrs:39", "");
  shell_output_str(&bbs_post_command, BBS_STRING_EDITHDR, "");      

  PROCESS_PAUSE();

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(ev == shell_event_input);
    input = data;

    if (! strcmp(input->data1, "/abt") ) {
       linecount=0;
       disk_access=1;
       PROCESS_EXIT();
    }

    if (! strcmp(input->data1,"/s") || linecount >= BBS_MAX_MSGLINES) {

      if (board.board_ptr < board.board_max) {
         board.board_ptr += 1;
         } else {
           board.board_ptr = 1;
      }

      /* write post */
      sprintf(file.szFileName, "board%d.msg", bbs_status.bbs_board_id);
      ssWriteRELFile(&file, &bbs_logbuf, sizeof(bbs_logbuf), board.board_ptr);

      /* write board data */
      strcpy(file.szFileName, BBS_BOARDCFG_FILE);
      ssWriteRELFile(&file, &board, sizeof(BBS_BOARD_REC), bbs_status.bbs_board_id);

      memset(bbs_logbuf, 0, sizeof(bbs_logbuf));
      linecount=0;
      disk_access=1;

      PROCESS_EXIT();
    }

    if (linecount <= BBS_MAX_MSGLINES) {
       disk_access=0;
       strncpy(bbs_logbuf[linecount], input->data1, BBS_LINE_WIDTH);
       linecount++;
    }
 } /* end ... while */

 PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
bbs_post_init(void)
{
  shell_register_command(&bbs_post_command);
}
