/**
 * \file
 *         bbs-read.c - read msg. from Contiki BBS message boards
 * \author
 *         (c) 2099-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */


#include "contiki.h"
#include "shell.h"
#include "bbs-read.h"
#include "telnetd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern BBS_STATUS_REC bbs_status;

/*---------------------------------------------------------------------------*/
PROCESS(bbs_read_process, "read");
SHELL_COMMAND(bbs_read_command, "r", "r : read a message", &bbs_read_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_read_process, ev, data)
{
  static short linecount=0;
  //struct shell_input *input;
  static char bbs_logbuf[BBS_MAX_MSGLINES][BBS_LINE_WIDTH];
  ST_FILE file;
  //BBS_BOARD_REC board;

  /* read board data */
  //strcpy(file.szFileName, "board.cfg");
  file.ucDeviceNo=bbs_status.board_drive;
  //ssReadRELFile(&file, &board, sizeof(BBS_BOARD_REC), bbs_status.bbs_board_id);
  memset(bbs_logbuf, 0, sizeof(bbs_logbuf));

  PROCESS_BEGIN();


  sprintf(file.szFileName, "alienterror");

  ssStreamSEQFile(&file, bbs_logbuf, sizeof(bbs_logbuf));
  
  PROCESS_EXIT();
/*
  sprintf(bbs_logbuf[0], "(%s, %d msgs.) msg# (0=quit)? ", board.board_name, board.board_ptr);
  shell_prompt(bbs_logbuf[0]); 

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == shell_event_input);
    input = data;

    if(atoi(input->data1) < 1 || atoi(input->data1) > board.board_ptr)  {
       shell_prompt(""); 
       PROCESS_EXIT();
    } else {
       bbs_status.bbs_msg_id=atoi(input->data1); 

       sprintf(bbs_logbuf[0], "* reading: board #%d, msg. #%d", bbs_status.bbs_board_id, bbs_status.bbs_msg_id);
       shell_output_str(&bbs_read_command, bbs_logbuf[0], "");
       shell_output_str(&bbs_read_command, BBS_STRING_EDITHDR, "");
       memset(bbs_logbuf, 0, sizeof(bbs_logbuf));

       sprintf(file.szFileName, "board%d.msg", bbs_status.bbs_board_id);
       //ssReadRELFile(&file, bbs_logbuf, sizeof(bbs_logbuf), bbs_status.bbs_msg_id);

       do {
          shell_output_str(&bbs_read_command, bbs_logbuf[linecount], "");
          linecount++;
       } while (linecount < BBS_MAX_MSGLINES); 

       linecount=0;
       PROCESS_EXIT();
    }
  } 
*/
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
bbs_read_init(void)
{
  shell_register_command(&bbs_read_command);
}
