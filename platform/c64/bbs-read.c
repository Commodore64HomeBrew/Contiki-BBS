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

extern BBS_CONFIG_REC bbs_config;
extern BBS_STATUS_REC bbs_status;

/*---------------------------------------------------------------------------*/
PROCESS(bbs_read_process, "read");
SHELL_COMMAND(bbs_read_command, "r", "r : read a message", &bbs_read_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_read_process, ev, data)
{

  struct shell_input *input;
  char message[40];
  unsigned short num;
  static short linecount=0;
  //struct shell_input *input;
  //static char bbs_logbuf[BBS_MAX_MSGLINES][BBS_LINE_WIDTH];
  ST_FILE file;
  //BBS_BOARD_REC board;


  PROCESS_BEGIN();

  shell_output_str(NULL,PETSCII_LOWER, "");
  shell_output_str(NULL,PETSCII_WHITE, "");
  sprintf(message, "\n\rselect msg (1-%d): ", bbs_config.bbs_msg_id[bbs_status.bbs_board_id]);
  shell_prompt(message);


  PROCESS_WAIT_EVENT_UNTIL(ev == shell_event_input);
  input = data;
  num = atoi(input->data1);

  if(num>0 && num <= bbs_config.bbs_msg_id[bbs_status.bbs_board_id]){
    sprintf(file.szFileName, "%d-%d", bbs_status.bbs_board_id, num);
    bbs_banner(file.szFileName, BBS_SUBS_DEVICE);
  }


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
       bbs_config.bbs_msg_id=atoi(input->data1); 

       sprintf(bbs_logbuf[0], "* reading: board #%d, msg. #%d", bbs_status.bbs_board_id, bbs_config.bbs_msg_id);
       shell_output_str(&bbs_read_command, bbs_logbuf[0], "");
       shell_output_str(&bbs_read_command, BBS_STRING_EDITHDR, "");
       memset(bbs_logbuf, 0, sizeof(bbs_logbuf));

       sprintf(file.szFileName, "board%d.msg", bbs_status.bbs_board_id);
       //ssReadRELFile(&file, bbs_logbuf, sizeof(bbs_logbuf), bbs_config.bbs_msg_id);

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
