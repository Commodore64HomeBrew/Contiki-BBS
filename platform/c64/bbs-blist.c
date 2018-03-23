/**
 * \file
 *         bbs-blist.c - show Contiki BBS message boards
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#include "contiki.h"
#include "shell.h"
#include "bbs-blist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern BBS_STATUS_REC bbs_status;

/*---------------------------------------------------------------------------*/
PROCESS(bbs_blist_process, "blist");
SHELL_COMMAND(bbs_blist_command, "l", "l : list message boards", &bbs_blist_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_blist_process, ev, data)
{
  //short bbs_board_id=1;
  //char szBuff[BBS_LINE_WIDTH];
  //ST_FILE file;
  //BBS_BOARD_REC board;

  PROCESS_BEGIN();
      if(bbs_status.bbs_encoding==1){
      	bbs_banner(BBS_BANNER_SUBS_a, BBS_SYS_DEVICE);
      }
      else{
	bbs_banner(BBS_BANNER_SUBS_p, BBS_SYS_DEVICE);
      }
    shell_prompt("\n\rboard list\n\r");
     /* load BBS config */
/*     strcpy(file.szFileName, BBS_BOARDCFG_FILE);
     file.ucDeviceNo=BBS_SYS_DEVICE;
     ssReadRELFile(&file, &board, sizeof(BBS_BOARD_REC), bbs_board_id);

     sprintf(szBuff, "total boards: %d", board.max_boards);
     shell_output_str(&bbs_blist_command, szBuff, "");
     shell_output_str(&bbs_blist_command, BBS_STRING_BOARDINFO, "");

     do {
        memset(&board, 0x00, sizeof(BBS_BOARD_REC));
        ssReadRELFile(&file, &board, sizeof(BBS_BOARD_REC), bbs_board_id);

        //sprintf(szBuff, " %02d  %-20s  %03d", board.board_no, board.board_name, board.board_ptr);
        sprintf(szBuff, " %2d  %-20s %3d  %3d", board.board_no, board.board_name, board.access_req, board.board_ptr);
        shell_output_str(&bbs_blist_command, szBuff, "");

        bbs_board_id++;
     } while (bbs_board_id <= board.max_boards);
*/
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
bbs_blist_init(void)
{
  shell_register_command(&bbs_blist_command);
}
