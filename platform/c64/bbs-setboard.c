/**
 * \file
 *         bbs-setboard.c - select Contiki BBS message boards 
 *
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */


#include "contiki.h"
#include "shell.h"

#include "bbs-setboard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern BBS_STATUS_REC bbs_status;
extern BBS_USER_REC bbs_user;

PROCESS(bbs_setboard_process, "board");
SHELL_COMMAND(bbs_setboard_command, "s", "s  : select active board", &bbs_setboard_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_setboard_process, ev, data)
{

  //struct shell_input *input;
  //char szBuff[BBS_LINE_WIDTH];
  //unsigned short num;
  //ST_FILE file;
  //BBS_BOARD_REC board;

  PROCESS_BEGIN();
  shell_prompt("\n\rselect board");
  /* read board data */
/*  strcpy(file.szFileName, BBS_BOARDCFG_FILE);
  file.ucDeviceNo=8;
  ssReadRELFile(&file, &board, sizeof(BBS_BOARD_REC), bbs_status.bbs_board_id);

  memset(szBuff, 0, sizeof(szBuff));
  sprintf(szBuff, "(%s (%d, acl: %d) Choose board # (1-%d, 0=quit)? ", board.board_name, board.board_no, board.access_req, board.max_boards);
  shell_prompt(szBuff);

  PROCESS_WAIT_EVENT_UNTIL(ev == shell_event_input);
  input = data;
  num = atoi(input->data1);
*/
  /* read new board data */
/*  strcpy(file.szFileName, BBS_BOARDCFG_FILE);
  file.ucDeviceNo=8;
  ssReadRELFile(&file, &board, sizeof(BBS_BOARD_REC), num);

  if (atoi(input->data1) < 0 || atoi(input->data1) > board.max_boards) {
    shell_prompt("invalid board id.\n");
  } else {
      if (bbs_user.access_req >= board.access_req) {
         bbs_status.bbs_board_id = num;
         shell_prompt("ok\n");
      } else {
         shell_prompt("insufficient access rights.\n");
      }
  }*/
  PROCESS_EXIT();
   
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
bbs_setboard_init(void)
{
  shell_register_command(&bbs_setboard_command);
}
