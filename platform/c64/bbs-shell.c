/**
 * \file
 *         shell.c - Contiki BBS core shell based on the Contiki OS shell. 
 *
 *         Contiki OS Shell Copyright (c) 2008, Swedish Institute of Computer Science.
 *         All rights reserved.
 *
 * \author
 *         Contiki BBS shell modifications (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "bbs-shell.h"
#include "bbs-encodings.h"
#include "bbs-setboard.h"
#include "bbs-file.h"
//#include <em.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>


LIST(commands);

int shell_event_input;
static struct process *front_process;
static unsigned long time_offset, last_time;
/*static struct etimer bbs_login_timer;*/

BBS_BOARD_REC board;
BBS_CONFIG_REC bbs_config;
BBS_STATUS_REC bbs_status;
BBS_USER_REC bbs_user;
BBS_USER_STATS bbs_usrstats;
BBS_TIME_REC bbs_time;
extern TELNETD_STATE s;

unsigned short bbs_locked=0;
unsigned short set_step=0;

/*---------------------------------------------------------------------------*/
PROCESS(shell_process, "Shell");
PROCESS(shell_server_process, "Shell server");
PROCESS(bbs_version_process, "version");
SHELL_COMMAND(bbs_version_command, "v", "v : show version info", 
              &bbs_version_process);
PROCESS(help_command_process, "help");
SHELL_COMMAND(help_command, "?", "? : shows this help",
	      &help_command_process);
PROCESS(shell_killall_process, "killall");
SHELL_COMMAND(killall_command, "killall", "killall : stop all running commands",
	      &shell_killall_process);
PROCESS(shell_kill_process, "kill");
SHELL_COMMAND(kill_command, "kill", "kill <command> : stop a specific command",
	      &shell_kill_process);
PROCESS(shell_exit_process, "exit");
SHELL_COMMAND(quit_command, "q", "q : exit bbs",
	      &shell_exit_process);
PROCESS(bbs_login_process, "login");
SHELL_COMMAND(bbs_login_command, "login", "login  : login proc", &bbs_login_process);
PROCESS(bbs_timer_process, "timer");

PROCESS(bbs_settime_process, "settime");
//SHELL_COMMAND(bbs_settime_command, "t", "t : set time", &bbs_settime_process);
SHELL_COMMAND(bbs_settime_command, "t", "", &bbs_settime_process);



/*---------------------------------------------------------------------------*/
void bbs_defaults(void)
{
  bbs_status.encoding=1;
  bbs_status.echo=1;
  bbs_status.width=BBS_40_COL;
  bbs_status.status=STATUS_UNLOCK;
  bbs_status.board_id=1;
}
/*---------------------------------------------------------------------------*/
void set_prompt(void) 
{
	unsigned short next_msg;
	next_msg = bbs_usrstats.current_msg[bbs_status.board_id]+1;

	
	if(next_msg > bbs_config.msg_id[bbs_status.board_id]){
		if(bbs_status.encoding==0){
			sprintf(bbs_status.prompt, "\r\n\x12\x9fsub:\x05%d\x1cmsgs:\x05%d\x92\x9f>\x05 ", bbs_status.board_id, bbs_config.msg_id[bbs_status.board_id], next_msg);
		}
		else{
			sprintf(bbs_status.prompt, "\r\nsub:%d msgs:%d> ", bbs_status.board_id, bbs_config.msg_id[bbs_status.board_id], next_msg);
		}
	}
	else{
		if(bbs_status.encoding==0){
			sprintf(bbs_status.prompt, "\r\n\x12\x9fsub:\x05%d\x1eret=\x05%d\x1c/\x05%d\x92\x9f>\x05 ", bbs_status.board_id, next_msg, bbs_config.msg_id[bbs_status.board_id]);
		}
		else{
			sprintf(bbs_status.prompt, "\r\nsub:%d ret=%d / %d> ", bbs_status.board_id, next_msg, bbs_config.msg_id[bbs_status.board_id]);
		}
	}

	
}

/*---------------------------------------------------------------------------*/
static void bbs_init(void) 
{
  //unsigned char *buffer;
  unsigned short fsize=0;
  unsigned short siRet=0;
  unsigned long set_time;
  unsigned char file[25];
  //unsigned char message[40];


  sprintf(board.board_name, "\n\r     CENTRONIAN BBS\n\r");
  board.telnet_port = 6400;
  board.max_boards = 8;

  board.subs_device = 8;
  sprintf(board.subs_prefix, "//s/");

  board.sys_device = 8;
  sprintf(board.sys_prefix, "//x/");

  board.user_device = 8;
  sprintf(board.user_prefix, "//u/");


  sprintf(file, "%s:%s",board.sys_prefix, BBS_CFG_FILE);

  /* read BBS base configuration */

  sprintf(board.sub_names[1], "the lounge     ");
  sprintf(board.sub_names[2], "science & tech ");
  sprintf(board.sub_names[3], "la musique     ");
  sprintf(board.sub_names[4], "hardware corner");
  sprintf(board.sub_names[5], "games & warez  ");
  sprintf(board.sub_names[6], "vic64 news     ");
  sprintf(board.sub_names[7], "great outdoors ");
  sprintf(board.sub_names[8], "member intros  ");


  board.dir_boost=1;


  bbs_time.minute=0;
  bbs_time.hour=0;
  bbs_time.day=1;
  bbs_time.month=8;
  bbs_time.year=1982;

  set_time = bbs_time.minute*60 + bbs_time.hour*3600;

  bbs_time.offset =  set_time - clock_seconds();
  //sprintf(message, "set:%li clock:%li offset:%li", set_time, clock_seconds(), bbs_time.offset);
  //log_message("time ", message);

  sprintf(file, "%s:%s",board.sys_prefix, BBS_CFG_FILE);

  /* read BBS base configuration */
  fsize=bbs_filesize(board.sys_prefix, BBS_CFG_FILE, board.sys_device);

  if (fsize != 0) {
    cbm_open(10, board.sys_device, 10, file);
    log_message("\x99", "config loaded from file");
    cbm_read(10, &bbs_config, 2);
    cbm_read(10, &bbs_config, sizeof(bbs_config));
    cbm_close(10);
  }
  else{

    log_message("\x96", "config file not found, using defaults");
    /* set sub msg counts */
    bbs_config.msg_id[1]=1611;
    bbs_config.msg_id[2]=140;
    bbs_config.msg_id[3]=117;
    bbs_config.msg_id[4]=536;
    bbs_config.msg_id[5]=370;
    bbs_config.msg_id[6]=139;
    bbs_config.msg_id[7]=160;
    bbs_config.msg_id[8]=29;

    /*for (i=0; i<=board.max_boards; i++) {
      bbs_config.msg_id[1]=0;
    }*/
  }

  bbs_defaults();
  set_prompt();

  //siRet = em_load_driver (BBS_EMD_FILE);

  //em_load(board.sys_prefix, BBS_BANNER_LOGIN, "", board.sys_device, 0);
  
}

/*---------------------------------------------------------------------------*/
void bbs_splash(unsigned short mode) 
{
  if (mode==BBS_MODE_CONSOLE)
    log_message("\x05",BBS_COPYRIGHT_STRING);
  else
    shell_output_str(&bbs_version_command,"",BBS_COPYRIGHT_STRING);
}
/*---------------------------------------------------------------------------*/
void bbs_lock(void)
{

  log_message("\x1c","bbs lock");

  //Change border colour to red
  bordercolor(2);
  //Blank the screen to speed things up
  //poke(0xd011, peek(0xd011) & 0xef);
  
  bbs_locked=1;
  bbs_defaults();
  process_start(&bbs_timer_process, NULL);
}
/*---------------------------------------------------------------------------*/
void bbs_unlock(void)
{

  log_message("\x1e","bbs unlock");

  //Change border colour to black
  bordercolor(0);
  //Turn on the screen again
  //poke(0xd011, peek(0xd011) | 0x  10);
  
  s.connected = 0;
  bbs_status.status=STATUS_UNLOCK;
  bbs_locked=0;
  bbs_defaults();
  process_exit(&bbs_timer_process);
  shell_exit();
}
/*---------------------------------------------------------------------------*/
int bbs_get_user(char *data)
{
	unsigned short fsize=0;
	unsigned short siRet=0;
	unsigned char file[25];

	strcpy(bbs_user.user_name, data);
  //strcat(bbs_user.user_name, '\0');
/*
  if(bbs_user.user_name[0] == '\0'){
    log_message("blank handle ", "");
    return 3;
  }
*/
	sprintf(file, "u-%s", bbs_user.user_name);

	fsize=bbs_filesize(board.user_prefix, file, board.user_device);

 	sprintf(file, "%s:u-%s", board.user_prefix, bbs_user.user_name);
  //log_message("[debug] user file: ", file);


  if (fsize != 0) {
    siRet = cbm_open(10, board.user_device, 10, file);
    if ( ! siRet ) {
      log_message("\x99login: ", bbs_user.user_name);
      cbm_read(10, &bbs_user, 2);
      cbm_read(10, &bbs_user, sizeof(bbs_user));
      cbm_close(10);
    }
    return 1;
	}
	else{
    log_message("\x96user not found: ", bbs_user.user_name);
    return 2;
  }


	return 0;

}

/*---------------------------------------------------------------------------*/
int bbs_new_user(char *data)
{
	//unsigned char i;
	//unsigned char file[25];

	strcpy(bbs_user.user_pwd, data);
	bbs_user.access_req = 1;


	return 1;
}



/*---------------------------------------------------------------------------*/

int bbs_save_user()
{
  unsigned char i;
  unsigned char file[25];

  sprintf(file, "%s:u-%s",board.user_prefix, bbs_user.user_name);

  cbm_save (file, board.user_device, &bbs_user, sizeof(bbs_user));

  for (i=0; i<=board.max_boards; i++) {
    bbs_usrstats.current_msg[i]=bbs_config.msg_id[i]-20;
  }

  return 1;
}


void bbs_login()
{

  unsigned short fsize=0;
  unsigned short siRet=0;
  unsigned char file[25];

  sprintf(file, "s-%s", bbs_user.user_name);
  //log_message("[debug] user stats file: ", file);

  sprintf(file, "%s:s-%s", board.user_prefix, bbs_user.user_name);


  siRet = cbm_open(10, board.user_device, 10, file);
  if (! siRet) {
    //log_message("[debug] stats file: ", file);
    cbm_read(10, &bbs_usrstats, 2);
    cbm_read(10, &bbs_usrstats, sizeof(bbs_usrstats));
    cbm_close(10);
  }
  else{
       log_message("\x96stats file load error: ", file);
  }

  //We need a debug msg here for log on time.
  //update_time();
  //sprintf(post_buffer,"\x1c\n\rFrom: \x05%s\x1e\n\rDate: \x05%d:%d %d/%d/%d\x9e\n\r", bbs_user.user_name, bbs_time.hour, bbs_time.minute, bbs_time.day, bbs_time.month, bbs_time.year , input->data1);
  
  //**********************************************************************
	process_exit(&bbs_timer_process);
	bbs_status.status=STATUS_LOCK;
	//log_message("[bbs] main prompt for ", bbs_user.user_name);

	bbs_banner(board.sys_prefix, BBS_BANNER_LOGO, bbs_status.encoding_suffix, board.sys_device, 0);
	//em_out(0);

	shell_output_str(NULL, "\x9clast caller: \x9e", bbs_status.last_caller);
  shell_output_str(NULL, "\r\n\x05? \x9fto list commands", "");
  shell_output_str(NULL, "\x05s \x9eselect msg board\r\n", "");


	strcpy(bbs_status.last_caller, bbs_user.user_name);
	//Display the sub banner:
	bbs_sub_banner();
	set_prompt();
	shell_prompt(bbs_status.prompt);
	process_start(&bbs_timer_process, NULL);
	front_process=&shell_process;
}


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_login_process, ev, data)
{
  struct shell_input *input;
  int return_code;

  PROCESS_BEGIN();

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(ev == shell_event_input || ev == PROCESS_EVENT_TIMER);

    if (ev == PROCESS_EVENT_TIMER) {
       //bbs_unlock();
       shell_stop();
       log_message("\x9a","event timer");
    }
    if (ev == shell_event_input) {
      input = data;
      switch (bbs_status.status) {

          case STATUS_UNLOCK: {


            if(! strcmp(input->data1, "8")){
              //log_message("[debug] encoding: ", input->data1);
              bbs_status.encoding=0;
              //bbs_status.echo=1;
              bbs_status.width=BBS_80_COL;
              strcpy(bbs_status.encoding_suffix, BBS_PET80_SUFFIX);
            }

            else if(! strcmp(input->data1, "4")){
              //log_message("[debug] encoding: ", input->data1);
              bbs_status.encoding=0;
              //bbs_status.echo=1;
              //bbs_status.width=BBS_40_COL;
              strcpy(bbs_status.encoding_suffix, BBS_PET40_SUFFIX);
            }
            else if(! strcmp(input->data1, "2")){
              //log_message("[debug] encoding: ", input->data1);
              bbs_status.encoding=0;
              //bbs_status.echo=1;
              bbs_status.width=BBS_22_COL;
              strcpy(bbs_status.encoding_suffix, BBS_PET22_SUFFIX);
            }


            else if(! strcmp(input->data1, "l") || ! strcmp(input->data1, "L")){
              //log_message("[debug] encoding: ", input->data1);
              bbs_status.encoding=1;
              bbs_status.echo=0;
              bbs_status.width=BBS_80_COL;
              strcpy(bbs_status.encoding_suffix, BBS_ASCII_SUFFIX);
            }
            else if(! strcmp(input->data1, "e") || ! strcmp(input->data1, "E")){
              //log_message("[debug] encoding: ", input->data1);
              bbs_status.encoding=1;
              bbs_status.echo=1;
              bbs_status.width=BBS_40_COL;
              strcpy(bbs_status.encoding_suffix, BBS_ASCII_SUFFIX);
            }

            else if(! strcmp(input->data1, "t") || ! strcmp(input->data1, "T")){
              //log_message("[debug] encoding: ", input->data1);
              bbs_status.encoding=2;
              //bbs_status.echo=1;
              //bbs_status.width=BBS_40_COL;
              strcpy(bbs_status.encoding_suffix, BBS_ASCII_SUFFIX);
            }


            else{
              shell_prompt(BBS_ENCODING_STRING);
              break;
            }
            bbs_banner(board.sys_prefix, BBS_BANNER_LOGIN, bbs_status.encoding_suffix, board.sys_device,0);
            shell_prompt("\n\rhandle: ");
            bbs_status.status=STATUS_HANDLE;
            break;
          }

          case STATUS_HANDLE: {
            if((int)strlen(input->data1)>12){
              shell_output_str(NULL, "\r\nhandle can't be longer than 12 characters\n\r", "");
              shell_prompt("\n\rhandle: ");
              bbs_status.status=STATUS_HANDLE;
              break;
            }
    			 
      			return_code = bbs_get_user(input->data1);
            //return_code=1;
      			if ( return_code == 1 ) {
      			    shell_prompt("\n\rpassword: ");
      			    bbs_status.status=STATUS_PASSWD;
      			}
      			else if ( return_code == 2 ) {
                shell_output_str(NULL,"\n\rnew user.\n\rplease enter a password.\n\r\n\r" , "");
      			    shell_prompt("\n\rpassword: ");
      			    bbs_status.status=STATUS_NEWUSR;
      			}
      			else {
      			  shell_output_str(&bbs_login_command, "login failed.", "");
      			  //bbs_unlock();
              shell_stop();
      			  log_message("\x96", "login failed");
      			}
      			break;
          }

          case STATUS_PASSWD: {
            if(! strcmp(input->data1, bbs_user.user_pwd)) {
            	bbs_login();
            } else {
              shell_output_str(&bbs_login_command, "wrong password", "");
              //bbs_unlock();
              shell_stop();
              log_message("\x96", "wrong password ");
            }
            break;
          }
          case STATUS_NEWUSR: {
           		bbs_new_user(input->data1);
              
              shell_output_str(NULL,"\n\r\n\rhandle:   " , bbs_user.user_name);
              shell_output_str(NULL,"\n\rpassword: " , bbs_user.user_pwd);

              bbs_status.status=STATUS_CONFUSR;
              shell_prompt("\n\r\n\rcorrect (y/n): ");
          	break;
          }
          case STATUS_CONFUSR: {

            if(! strcmp(input->data1, "y") || ! strcmp(input->data1, "Y")){
              bbs_save_user();
              bbs_login();
            }
            else{
              shell_prompt("\n\rhandle: ");
              bbs_status.status=STATUS_HANDLE;
            }
            break;
          }

       }
    }
  } /* end ... while */

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
static void
command_kill(struct shell_command *c)
{
  if(c != NULL) {
    shell_output_str(&killall_command, "Stopping command ", c->command);
    process_exit(c->process);
  }
}
/*---------------------------------------------------------------------------*/
static void
killall(void)
{
  struct shell_command *c;
  for(c = list_head(commands);
      c != NULL;
      c = c->next) {
    if(c != &killall_command && process_is_running(c->process)) {
      command_kill(c);
    }
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_timer_process, ev, data)
{
  static struct etimer bbs_session_timer;
  char szBuff[20];

  PROCESS_BEGIN();
  if (bbs_status.status>STATUS_HANDLE)
     etimer_set(&bbs_session_timer, CLOCK_SECOND * BBS_TIMEOUT_SEC);
  else
     etimer_set(&bbs_session_timer, CLOCK_SECOND * BBS_LOGIN_TIMEOUT_SEC);

  while (1) {

     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&bbs_session_timer));

     if (ev == PROCESS_EVENT_TIMER) {
        if (bbs_status.status>STATUS_HANDLE)
           process_post(PROCESS_BROADCAST, PROCESS_EVENT_TIMER, NULL);
        else
           process_post(&bbs_login_process, PROCESS_EVENT_TIMER, NULL);

        sprintf(szBuff, "session timeout.");
        shell_output_str(NULL, szBuff, "");
        if (bbs_status.status>STATUS_HANDLE)
           etimer_set(&bbs_session_timer, CLOCK_SECOND * BBS_TIMEOUT_SEC);
        else
           etimer_set(&bbs_session_timer, CLOCK_SECOND * BBS_LOGIN_TIMEOUT_SEC);
     } else {
       if (ev == shell_event_input) 
         if (bbs_status.status>STATUS_HANDLE)
            etimer_set(&bbs_session_timer, CLOCK_SECOND * BBS_TIMEOUT_SEC);
         else
            etimer_set(&bbs_session_timer, CLOCK_SECOND * BBS_LOGIN_TIMEOUT_SEC);
     }
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_killall_process, ev, data)
{

  PROCESS_BEGIN();

  killall();
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_kill_process, ev, data)
{
  struct shell_command *c;
  char *name;
  PROCESS_BEGIN();

  name = data;
  if(name == NULL || strlen(name) == 0) {
    shell_output_str(&kill_command,
		     "kill <command>: command name must be given", "");
  }

  for(c = list_head(commands);
      c != NULL;
      c = c->next) {
    if(strcmp(name, c->command) == 0 &&
       c != &kill_command &&
       process_is_running(c->process)) {
      command_kill(c);
      PROCESS_EXIT();
    }
  }

  shell_output_str(&kill_command, "Command not found: ", name);
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_version_process, ev, data)
{
  PROCESS_BEGIN();

    bbs_splash(BBS_MODE_SHELL);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(help_command_process, ev, data)
{
  struct shell_command *c;
  PROCESS_BEGIN();

  shell_output_str(&help_command, "available commands:", "");
  for(c = list_head(commands);
      c != NULL;
      c = c->next) {
    shell_output_str(&help_command, c->description, "");
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_exit_process, ev, data)
{
  ST_FILE file;
  PROCESS_BEGIN();

  //**********************************************************************
  //This needs to go in a separate function...
  //**********************************************************************
  sprintf(file.szFileName, "@%s:s-%s", board.user_prefix, bbs_user.user_name);
  //log_message("[debug] user stats file: ", file.szFileName);

  cbm_save (file.szFileName, board.user_device, &bbs_usrstats, sizeof(bbs_usrstats));
  //**********************************************************************


  bbs_banner(board.sys_prefix, BBS_BANNER_LOGOUT, bbs_status.encoding_suffix, board.sys_device,0);
  log_message("\x05logout: ", bbs_user.user_name);
  shell_stop();
  //bbs_unlock();
  //log_message("[debug] *unlock2* ", "");

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_settime_process, ev, data)
{
  struct shell_input *input;
  unsigned long set_time;
  unsigned short num;
  char message[40];
  PROCESS_BEGIN();

  sprintf(message,"%d:%d %d/%d/%d\n\r", bbs_time.hour ,bbs_time.minute, bbs_time.day,  bbs_time.month, bbs_time.year);
  shell_output_str(NULL, "\n\rold time: ", message);
  log_message("\x9eold time: ", message);

  shell_prompt("year:");
  set_step=1;

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(ev == shell_event_input);

    if (ev == shell_event_input) {
      input = data;
      num = atoi(input->data1);

      if(set_step==1) {
        bbs_time.year=num;
        set_step=2;
        shell_prompt("month:");
      }
      else if (set_step==2) {
        bbs_time.month=num;
        set_step=3;
        shell_prompt("day:");
      }
      else if (set_step==3) {
        bbs_time.day=num;
        set_step=4;
        shell_prompt("hour:");
      }
      else if (set_step==4) {
        bbs_time.hour=num;
        set_step=5;
        shell_prompt("minute:");
      }
      else if (set_step==5) {
        bbs_time.minute=num;
        set_step=0;
        sprintf(message,"%d:%d %d/%d/%d\n\r", bbs_time.hour ,bbs_time.minute, bbs_time.day,  bbs_time.month, bbs_time.year);
        shell_output_str(NULL, "\n\t\rnew time: ", message);
        log_message("\x9enew time: ", message);

        set_time = bbs_time.minute*60 + bbs_time.hour*3600;
        bbs_time.offset =  set_time - clock_seconds();
        break;
      }
    }
  }
  PROCESS_EXIT();
  PROCESS_END();

}



/*---------------------------------------------------------------------------*/
static void
replace_braces(char *commandline)
{
  char *ptr;
  int level = 0;
  
  for(ptr = commandline; *ptr != 0; ++ptr) {
    if(*ptr == '{') {
      if(level == 0) {
	*ptr = ' ';
      }
      ++level;
    } else if(*ptr == '}') {
      --level;
      if(level == 0) {
	*ptr = ' ';
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
static char *
find_pipe(char *commandline)
{
  char *ptr;
  int level = 0;
  
  for(ptr = commandline; *ptr != 0; ++ptr) {
    if(*ptr == '{') {
      ++level;
    } else if(*ptr == '}') {
      --level;
    } else if(*ptr == '|') {
      if(level == 0) {
	return ptr;
      }
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
static struct shell_command *
start_command(char *commandline, struct shell_command *child)
{
  char *next, *args;
  int command_len;
  struct shell_command *c;

  /* Shave off any leading spaces. */
  while(*commandline == ' ') {
    commandline++;
  }

  /* Find the next command in a pipeline and start it. */
  next = find_pipe(commandline);
  if(next != NULL) {
    *next = 0;
    child = start_command(next + 1, child);
  }

  /* Separate the command arguments, and remove braces. */
  replace_braces(commandline);
  args = strchr(commandline, ' ');
  if(args != NULL) {
    args++;
  }

  /* Shave off any trailing spaces. */
  command_len = (int)strlen(commandline);
  while(command_len > 0 && commandline[command_len - 1] == ' ') {
    commandline[command_len - 1] = 0;
    command_len--;
  }
  
  if(args == NULL) {
    command_len = (int)strlen(commandline);
    args = &commandline[command_len];
  } else {
    command_len = (int)(args - commandline - 1);
  }
  

  
  /* Go through list of commands to find a match for the first word in
     the command line. */
  for(c = list_head(commands);
      c != NULL &&
	!(strncmp(c->command, commandline, command_len) == 0 &&
	  c->command[command_len] == 0);
      c = c->next);
  
  if(c == NULL) {
    shell_output_str(NULL, commandline, ": command not found (try '?')");
    command_kill(child);
    c = NULL;
  } else if(process_is_running(c->process) || child == c) {
    shell_output_str(NULL, commandline, ": command already running");
    c->child = NULL;
    c = NULL;
  } else {
    c->child = child;
    /*    printf("shell: start_command starting '%s'\n", c->process->name);*/
    /* Start a new process for the command. */
    process_start(c->process, args);
  }
  
  return c;
}
/*---------------------------------------------------------------------------*/
int
shell_start_command(char *commandline, int commandline_len,
		    struct shell_command *child,
		    struct process **started_process)
{
  struct shell_command *c;
  int background = 0;

  if(commandline_len == 0) {
    if(started_process != NULL) {
      *started_process = NULL;
    }
    return SHELL_NOTHING;
  }

  if(commandline[commandline_len - 1] == '&') {
    commandline[commandline_len - 1] = 0;
    background = 1;
    commandline_len--;
  }

  c = start_command(commandline, child);

  /* Return a pointer to the started process, so that the caller can
     wait for the process to complete. */
  if(c != NULL && started_process != NULL) {
    *started_process = c->process;
    if(background) {
      return SHELL_BACKGROUND;
    } else {
      return SHELL_FOREGROUND;
    }
  }
  return SHELL_NOTHING;
}
/*---------------------------------------------------------------------------*/
static void
input_to_child_command(struct shell_command *c,
		       char *data1, int len1,
		       const char *data2, int len2)
{
  struct shell_input input;
  if(process_is_running(c->process)) {
    input.data1 = data1;
    input.len1 = len1;
    input.data2 = data2;
    input.len2 = len2;
    process_post_synch(c->process, shell_event_input, &input);
  }
}
/*---------------------------------------------------------------------------*/
void
shell_input(char *commandline, int commandline_len)
{
  struct shell_input input;

  /*  printf("shell_input front_process '%s'\n", front_process->name);*/

  if(commandline[0] == '~' &&
     commandline[1] == 'K') {
    /*    process_start(&shell_killall_process, commandline);*/
    if(front_process != &shell_process) {
      process_exit(front_process);
    }
  } else {
    if(process_is_running(front_process)) {
      input.data1 = commandline;
      input.len1 = commandline_len;
      input.data2 = "";
      input.len2 = 0;
      process_post_synch(front_process, shell_event_input, &input);
    }
  }
}
/*---------------------------------------------------------------------------*/
void
shell_output_str(struct shell_command *c, char *text1, char *text2)
{
  if(c != NULL && c->child != NULL) {
    input_to_child_command(c->child, text1, (int)strlen(text1),
			   text2, (int)strlen(text2));
  } else {
    shell_default_output(text1, (int)strlen(text1),
			 text2, (int)strlen(text2));
  }
}
/*---------------------------------------------------------------------------*/
/*void
shell_output(struct shell_command *c,
	     void *data1, int len1,
	     const void *data2, int len2)
{
  if(c != NULL && c->child != NULL) {
    input_to_child_command(c->child, data1, len1, data2, len2);
  } else {
    shell_default_output(data1, len1, data2, len2);
  }
}*/
/*---------------------------------------------------------------------------*/
void
shell_unregister_command(struct shell_command *c)
{
  list_remove(commands, c);
}
/*---------------------------------------------------------------------------*/
void
shell_register_command(struct shell_command *c)
{
  struct shell_command *i, *p;

  p = NULL;
  for(i = list_head(commands);
      i != NULL &&
	strcmp(i->command, c->command) < 0;
      i = i->next) {
    p = i;
  }
  if(p == NULL) {
    list_push(commands, c);
  } else if(i == NULL) {
    list_add(commands, c);
  } else {
    list_insert(commands, p, c);
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_process, ev, data)
{
  static struct process *started_process;
  /*static struct etimer bbs_session_timer;*/
  struct shell_input *input;
  int ret;

  PROCESS_BEGIN();

  /* Let the system start up before showing the prompt. */
  PROCESS_PAUSE();
  
  /*etimer_set(&bbs_session_timer, CLOCK_SECOND * BBS_TIMEOUT_SEC);*/

  while(1) {
  
    PROCESS_WAIT_EVENT();

    if (ev == shell_event_input)
    {
      input = data;
      /*etimer_reset(&bbs_session_timer);*/
      ret = shell_start_command(input->data1, input->len1, NULL,
				&started_process);
      if(started_process != NULL && ret == SHELL_FOREGROUND && process_is_running(started_process)) {
        front_process = started_process;
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_EXITED && data == started_process);
      }
      front_process = &shell_process;
    }

    if (ev == PROCESS_EVENT_TIMER){
      log_message("\x9a", "timer event2");
      shell_stop();
      //bbs_unlock();
    }
    if(bbs_status.status>STATUS_HANDLE) {
      //etimer_set(&bbs_session_timer, CLOCK_SECOND * BBS_TIMEOUT_SEC);
      shell_prompt(bbs_status.prompt);
    }
  
  }
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_server_process, ev, data)
{
  struct process *p;
  struct shell_command *c;
  static struct etimer session_timer;
  PROCESS_BEGIN();

  etimer_set(&session_timer, CLOCK_SECOND * 10);
  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_EXITED) {
      p = data;
      /*      printf("process exited '%s' (front '%s')\n", p->name,
	      front_process->name);*/
      for(c = list_head(commands);
	  c != NULL && c->process != p;
	  c = c->next);
      while(c != NULL) {
	if(c->child != NULL && c->child->process != NULL) {
	  /*	  printf("Killing '%s'\n", c->process->name);*/
	  input_to_child_command(c->child, "", 0, "", 0);
	  /*	  process_exit(c->process);*/
	}
	c = c->child;
      }
    } else if(ev == PROCESS_EVENT_TIMER) {
      etimer_reset(&session_timer);
      shell_set_time(shell_time());
    }
  }
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
shell_init(void)
{
  /* register BBS processes */
  list_init(commands);
  shell_register_command(&help_command);
  shell_register_command(&quit_command);
  shell_register_command(&bbs_version_command);
  shell_register_command(&bbs_settime_command);


  /* local console eye candy */
  clrscr();
  bordercolor(0);
  bgcolor(0);
  textcolor(5);
  bbs_splash(BBS_MODE_CONSOLE);

  bbs_init();
  bbs_setboard_init();
  bbs_read_init();
  bbs_post_init();

  shell_event_input = process_alloc_event();
  
  process_start(&bbs_login_process, NULL);
  process_start(&shell_process, NULL);
  process_start(&shell_server_process, NULL);

  front_process = &bbs_login_process;

  bbs_status.status=STATUS_UNLOCK;
}
/*---------------------------------------------------------------------------*/
/*unsigned long
shell_strtolong(const char *str, const char **retstr)
{
  int i;
  unsigned long num = 0;
  const char *strptr = str;

  if(str == NULL) {
    return 0;
  }
  
  while(*strptr == ' ') {
    ++strptr;
  }
  
  for(i = 0; i < 10 && isdigit(strptr[i]); ++i) {
    num = num * 10 + strptr[i] - '0';
  }
  if(retstr != NULL) {
    if(i == 0) {
      *retstr = str;
    } else {
      *retstr = strptr + i;
    }
  }
  
  return num;
}*/
/*---------------------------------------------------------------------------*/
unsigned long
shell_time(void)
{
  return clock_seconds() + time_offset;
}
/*---------------------------------------------------------------------------*/
void
shell_set_time(unsigned long seconds)
{
  time_offset = seconds - clock_seconds();
}
/*---------------------------------------------------------------------------*/
void
shell_start(void)
{
  /* set BBS parameters */
  /*bbs_status.board_id=1;
  bbs_config.msg_id=1;
  process_start(&bbs_timer_process, NULL);*/
  
  if(bbs_locked == 1) {
    shell_exit(); //This disconnects the user!
    log_message("\x96","busy");
  } else {
    //bbs_locked=1;
    bbs_lock();

    shell_output_str(NULL, PETSCII_LOWER, board.board_name);

    shell_prompt(BBS_ENCODING_STRING);

    front_process=&bbs_login_process;
  } 


}
/*---------------------------------------------------------------------------*/
void
shell_stop(void)
{
  log_message("\x9e", "shell stop");
  bbs_unlock();
  killall();
}
/*---------------------------------------------------------------------------*/
void
shell_quit(void)
{
  log_message("\x9e", "shell quit");
  process_exit(&bbs_login_process);
  process_exit(&bbs_timer_process);
  process_exit(&shell_process);
  process_exit(&shell_server_process);
  shell_stop();
}
/*---------------------------------------------------------------------------*/

void update_time(void) {
  unsigned long now_sec;
  //char message[40];

  now_sec = clock_seconds() + bbs_time.offset;

  if (now_sec >  86400){
    now_sec = now_sec - 86400;
  }

  //sprintf(message,"clock_seconds: %lu now_time: %lu\n\r",clock_seconds(), now_time);
  //log_message("time: ", message);

  bbs_time.hour = now_sec/3600;
  bbs_time.minute = now_sec/60 - bbs_time.hour*60; 


  if (last_time > now_sec) {
    //sprintf(message,"%d:%d %d/%d/%d\n\r", bbs_time.hour ,bbs_time.minute, bbs_time.day,  bbs_time.month, bbs_time.year);
    //log_message("\x9etime: ", message);

    if (bbs_time.day==30){

      //if(bbs_status.month==2 && bbs_status.day==28 && (bbs_status.year % 4) == 0)

        //bbs_status.day
      
      bbs_time.day=1;

      if(bbs_time.month==12){
        bbs_time.month=1;
        ++bbs_time.year;
      }
      else{
        ++bbs_time.month;
      }
    }
    else{
      ++bbs_time.day;
    }
  }

  last_time = now_sec;
//function f(x) { return 28 + (x + Math.floor(x/8)) % 2 + 2 % x + 2 * Math.floor(1/x); }
}


/** @} */
