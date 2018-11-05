/**
 * \file
 *         bbs-post.c - post msg. to Contiki BBS message boards 
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#include "contiki.h"
#include "bbs-shell.h"

#include "bbs-post.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern BBS_BOARD_REC board;
extern BBS_CONFIG_REC bbs_config;
extern BBS_STATUS_REC bbs_status;
extern BBS_USER_REC bbs_user;

/*extern char bbs_logbuf[BBS_MAX_MSGLINES][BBS_LINE_WIDTH];*/

PROCESS(bbs_post_process, "write");
SHELL_COMMAND(bbs_post_command, "w", "w : write a new message", &bbs_post_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_post_process, ev, data)
{

	//static short linecount=0;
	//static short disk_access=1;
	//static short bytes_used=2;
	struct shell_input *input;
	//static char bbs_logbuf[BBS_MAX_MSGLINES*BBS_LINE_WIDTH];
	char bbs_logbuf[1024];
	ST_FILE file;
	//BBS_BOARD_REC board;

	/* read board data */
	/*  if (disk_access) {
	 strcpy(file.szFileName, BBS_BOARDCFG_FILE);
	 file.ucDeviceNo=board.sys_device;
	 ssReadRELFile(&file, &board, sizeof(BBS_BOARD_REC), bbs_status.board_id);
	 disk_access=0;
	}
	*/  PROCESS_BEGIN();

	//process_exit(&bbs_read_process);
	//process_exit(&bbs_setboard_process);

	shell_output_str(NULL,PETSCII_LOWER, PETSCII_WHITE);
	shell_output_str(&bbs_post_command, "on empty line: /a=abort /s=save\r\n", "");
	
	if ( bbs_status.width == BBS_22_COL) {
		shell_output_str(&bbs_post_command, BBS_STRING_EDITH22, "");
	}
	else {
		shell_output_str(&bbs_post_command, BBS_STRING_EDITH40, "");
	}

	sprintf(bbs_logbuf,"\r\n\x92\n\rmsg from: %s\n\r\x05\n\r", bbs_user.user_name);

	bbs_status.status=STATUS_POST;
	bbs_status.msg_size=30;
	PROCESS_PAUSE();

	while(1) {

		PROCESS_WAIT_EVENT_UNTIL(ev == shell_event_input);
		input = data;

		if (! strcmp(input->data1, "/a") ) {
			//linecount=0;
			//disk_access=1;
			PROCESS_EXIT();
		}

		if (! strcmp(input->data1,"/s")){// || linecount >= BBS_MAX_MSGLINES) {

			/* write post */
			++bbs_config.msg_id[bbs_status.board_id];

			sprintf(file.szFileName, "%s:%d-%d", board.subs_prefix, bbs_status.board_id, bbs_config.msg_id[bbs_status.board_id]);

			log_message("[debug] file postmsg: ", file.szFileName);

			/* Save the post to file */
		
			cbm_save (file.szFileName, board.subs_device, &bbs_logbuf, bbs_status.msg_size);

			log_message("[bbs] *post* ", bbs_logbuf);

			//sprintf(bbs_logbuf, "bu:%d ms:%d lb:%d", bytes_used, bbs_status.msg_size, sizeof(bbs_logbuf));
			//log_message("[debug] *bytes-used* ", bbs_logbuf);


			/* Save the msg count struct to disk */
			sprintf(file.szFileName, "@%s:%s", board.sys_prefix, BBS_CFG_FILE);
			cbm_save (file.szFileName, board.sys_device, &bbs_config, sizeof(bbs_config));

			memset(bbs_logbuf, 0, sizeof(bbs_logbuf));
			//linecount=0;
			//disk_access=1;

			bbs_status.status=STATUS_LOCK;

			set_prompt();
			PROCESS_EXIT();
		}


		//bytes_used += sizeof(input->data1);
		strcat(bbs_logbuf, input->data1);

		//log_message("[bbs] *post* ", bbs_logbuf);
		/*if (linecount <= BBS_MAX_MSGLINES) {
		   disk_access=0;
		   strncpy(bbs_logbuf[linecount], input->data1, BBS_LINE_WIDTH);
		   linecount++;
		}*/
	} /* end ... while */

	//bbs_setboard_init();
	//bbs_read_init();

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
bbs_post_init(void)
{
  shell_register_command(&bbs_post_command);
}
