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
SHELL_COMMAND(bbs_setboard_command, "s", "s : select active board", &bbs_setboard_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_setboard_process, ev, data)
{

  struct shell_input *input;
  //char szBuff[BBS_LINE_WIDTH];
  unsigned short num;
  char file[12];
  //ST_FILE file;
  //BBS_BOARD_REC board;

  PROCESS_BEGIN();

  if(bbs_status.bbs_encoding==1){
    bbs_banner(BBS_BANNER_SUBS_a);
  }
  else{
    bbs_banner(BBS_BANNER_SUBS_p);
  }
  /*memset(szBuff, 0, sizeof(szBuff));
  sprintf(szBuff, "(%s (%d, acl: %d) Choose board # (1-%d, 0=quit)? ", board.board_name, board.board_no, board.access_req, board.max_boards);
  shell_prompt(szBuff);
  */

  shell_prompt("select board (1-8):");
  PROCESS_WAIT_EVENT_UNTIL(ev == shell_event_input);
  input = data;
  num = atoi(input->data1);


  if(num>0 && num <=BBS_MAX_BOARDS){
    bbs_status.bbs_board_id = num;
    sprintf(file, "%s%d",BBS_PREFIX_SUB_p,num);
    bbs_banner(file);
  }

/*

  if(! strcmp(input->data1, "1"){bbs_banner("sp-1");}
  else if(! strcmp(input->data1, "2"){bbs_banner("sp-1");}

  num = int(input->data1);

  if( num>0 && num <= BBS_MAX_BOARDS ){

    strcat(BBS_SUB_PRE_p , &c);
    bbs_banner();
  }

*/


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


PROCESS(bbs_nextboard_process, "next");
SHELL_COMMAND(bbs_nextboard_command, "+", "+ : next sub board", &bbs_nextboard_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_nextboard_process, ev, data)
{
  char file[12];

  PROCESS_BEGIN();

  if(bbs_status.bbs_board_id < BBS_MAX_BOARDS){

    ++bbs_status.bbs_board_id;
    sprintf(file, "%s%d",BBS_PREFIX_SUB_p,bbs_status.bbs_board_id);
    bbs_banner(file);

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
  char file[12];
  PROCESS_BEGIN();

  if(bbs_status.bbs_board_id > 1){

    --bbs_status.bbs_board_id;
    sprintf(file, "%s%d",BBS_PREFIX_SUB_p,bbs_status.bbs_board_id);
    bbs_banner(file);

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
