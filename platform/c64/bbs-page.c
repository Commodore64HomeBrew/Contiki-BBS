/**
 * \file
 *         bbs-page.c - page SysOp from Contiki BBS
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#include "contiki.h"
#include "shell.h"

#include "bbs-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PROCESS(bbs_page_process, "page");
SHELL_COMMAND(bbs_page_command, "z", "z : page sysop", &bbs_page_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_page_process, ev, data)
{

  static short linecount=0;
  struct shell_input *input;
  char bbs_logbuf[BBS_MAX_MSGLINES][BBS_LINE_WIDTH];

  PROCESS_BEGIN();

  shell_output_str(&bbs_page_command, "*paging sysop (/q on empty line to quit)", "");
  shell_output_str(&bbs_page_command, BBS_STRING_EDITHDR, "");      

  /*bordercolor(2);
  clrscr();*/
  log_message("\n","*pager*       SYSOP CALL!!");
  log_message("",BBS_STRING_EDITHDR);      

  memset(bbs_logbuf,0,sizeof(bbs_logbuf));

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == shell_event_input);
    input = data;

    if(! strcmp(input->data1,"/q") || linecount >= BBS_MAX_MSGLINES) {
       linecount=0;
       log_message("",BBS_STRING_EDITHDR);
       /*bordercolor(5);*/
       PROCESS_EXIT();
    } else {

       if (linecount <= BBS_MAX_MSGLINES) {
          memset(bbs_logbuf,0,sizeof(bbs_logbuf));
          strcat(bbs_logbuf[linecount], input->data1);
          log_message(bbs_logbuf[linecount],"");
          linecount++;
       }
   }
  } /* end ... while */

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
bbs_page_init(void)
{
  shell_register_command(&bbs_page_command);
}
