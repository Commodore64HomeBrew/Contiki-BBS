/**
 * \file
 *         bbs-setboard.c - select Contiki BBS message boards 
 *
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */


#include "contiki.h"
#include "bbs-shell.h"

#include "bbs-setboard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern BBS_BOARD_REC board;
extern BBS_CONFIG_REC bbs_config;
extern BBS_STATUS_REC bbs_status;
extern BBS_USER_STATS bbs_usrstats;


void bbs_sub_banner(void)
{
  unsigned char message[80];
  unsigned char file[12];

  sprintf(file, "%s%d",BBS_PREFIX_SUB,bbs_status.board_id);
  bbs_banner(board.sys_prefix, file, bbs_status.encoding_suffix, board.sys_device,0);
  sprintf(message, "\r\n%s\n\rtotal msgs: %d\n\n", board.sub_names[bbs_status.board_id], bbs_config.msg_id[bbs_status.board_id]);
  shell_output_str(NULL, message, "");
}


PROCESS(bbs_setboard_process, "board");
SHELL_COMMAND(bbs_setboard_command, "s", "s : select active board", &bbs_setboard_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_setboard_process, ev, data)
{

  struct shell_input *input;
  //char szBuff[BBS_LINE_WIDTH];
  unsigned short num;
  unsigned char message[40];
  //ST_FILE file;

  PROCESS_BEGIN();

  bbs_banner(board.sys_prefix, BBS_BANNER_SUBS, bbs_status.encoding_suffix, board.sys_device, 0);


  for (num=1; num<BBS_MAX_BOARDS;num++){
    sprintf(message, "\x9e\r\n%s \x1c-> \x05%d", board.sub_names[num], bbs_config.msg_id[num] - bbs_usrstats.current_msg[num]);
    shell_output_str(NULL,"", message);
  }


  //shell_output_str(NULL,"", PETSCII_WHITE);
  sprintf(message, "\x05\n\rselect board (1-%d):", board.max_boards);
  shell_prompt(message);

  PROCESS_WAIT_EVENT_UNTIL(ev == shell_event_input);
  input = data;
  num = atoi(input->data1);

  if(num>0 && num <=board.max_boards){

    bbs_status.board_id = num;
    set_prompt();
    bbs_sub_banner();
  }


  PROCESS_EXIT();
   
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/


PROCESS(bbs_nextboard_process, "next");
SHELL_COMMAND(bbs_nextboard_command, "+", "+ : next sub board", &bbs_nextboard_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_nextboard_process, ev, data)
{
  PROCESS_BEGIN();

  if(bbs_status.board_id < board.max_boards){

    ++bbs_status.board_id;

    set_prompt();
    bbs_sub_banner();
  }

  PROCESS_EXIT();
   
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

PROCESS(bbs_prevboard_process, "previous");
SHELL_COMMAND(bbs_prevboard_command, "-", "- : previous sub board", &bbs_prevboard_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_prevboard_process, ev, data)
{
  PROCESS_BEGIN();

  if(bbs_status.board_id > 1){

    --bbs_status.board_id;

	set_prompt();
    bbs_sub_banner();
  }

  PROCESS_EXIT();
   
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

void
bbs_setboard_init(void)
{
  shell_register_command(&bbs_setboard_command);
  shell_register_command(&bbs_nextboard_command);
  shell_register_command(&bbs_prevboard_command);
}
