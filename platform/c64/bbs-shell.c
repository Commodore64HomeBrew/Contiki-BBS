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
#include <em.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

LIST(commands);

int shell_event_input;
static struct process *front_process;
static unsigned long time_offset;
/*static struct etimer bbs_login_timer;*/
 
BBS_CONFIG_REC bbs_config;
BBS_STATUS_REC bbs_status;
BBS_USER_REC bbs_user;
unsigned short bbs_locked=0;

/*---------------------------------------------------------------------------*/
PROCESS(shell_process, "Shell");
PROCESS(shell_server_process, "Shell server");
PROCESS(bbs_version_process, "version");
SHELL_COMMAND(bbs_version_command, "v", "v : show version and copyright", 
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

/*---------------------------------------------------------------------------*/
void bbs_defaults(void)
{
  bbs_status.encoding=1;
  bbs_status.echo=1;
  bbs_status.width=39;
  bbs_status.status=0;
  bbs_status.board_id=1;
}
/*---------------------------------------------------------------------------*/
void set_prompt(void) 
{
    sprintf(bbs_status.prompt, "\x05sub %d, msg %d > ", bbs_status.board_id, bbs_status.current_msg[bbs_status.board_id]);
}

/*---------------------------------------------------------------------------*/
static void bbs_init(void) 
{
  //unsigned char *buffer;
  unsigned short fsize=0;
  unsigned short siRet=0;
  unsigned short i;


  /* read BBS base configuration */
  fsize=bbs_filesize(BBS_CFG_FILE, BBS_SUBS_DEVICE);

  if (fsize == 0) {
     log_message("[bbs] ", "config file not found, using defaults");
     /* set sub msg counts */
     for (i=0; i<=BBS_MAX_BOARDS; i++) {
          bbs_config.msg_id[1]=0;
     }
  }
  else{
    siRet = cbm_open(10, BBS_SUBS_DEVICE, 10, BBS_CFG_FILE);
    if (! siRet) {
      log_message("[bbs] ", "config loaded from file");
      cbm_read(10, &bbs_config, 2);
      cbm_read(10, &bbs_config, sizeof(bbs_config));
      cbm_close(10);
    }
    else{log_message("[bbs] ", "config file error");}
  }

  bbs_defaults();
  set_prompt();

  siRet = em_load_driver (BBS_EMD_FILE);

  em_load(BBS_BANNER_LOGIN, "", BBS_SYS_DEVICE);
  
}

/*---------------------------------------------------------------------------*/
void bbs_splash(unsigned short mode) 
{
  if (mode==BBS_MODE_CONSOLE)
    log_message("",BBS_COPYRIGHT_STRING);
  else
    shell_output_str(&bbs_version_command,"",BBS_COPYRIGHT_STRING);
}
/*---------------------------------------------------------------------------*/
void bbs_lock(void)
{
  bbs_status.board_id=1;
  //bbs_config.msg_id=1;
  process_start(&bbs_timer_process, NULL);
}
/*---------------------------------------------------------------------------*/
void bbs_unlock(void)
{
  log_message("[bbs] ", "*session timeout*");

  bbs_locked=0;
  bbs_defaults();
  process_exit(&bbs_timer_process);
  shell_exit();
}
/*---------------------------------------------------------------------------*/
int bbs_get_user(char *data)
{
  /*int user_count,count=1;
  ST_FILE file;

  strcpy(file.szFileName, "user.idx");
  file.ucDeviceNo = BBS_SYS_DEVICE;
  ssReadRELFile(&file, &user_count, sizeof(unsigned short), 1);

  strcpy(file.szFileName, "user.dat");
  file.ucDeviceNo = BBS_SYS_DEVICE;

  do {     
       memset(&bbs_user, 0, sizeof(BBS_USER_REC));
       ssReadRELFile(&file, &bbs_user, sizeof(BBS_USER_REC), count);

       if (! strcmp(bbs_user.user_name, data)) {
          return count;
       }

       count++;
  } while (count <= user_count); 

  return 0;
*/
  strcpy(bbs_user.user_name, data);
  return 1;

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_login_process, ev, data)
{
  struct shell_input *input;

  PROCESS_BEGIN();

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(ev == shell_event_input || ev == PROCESS_EVENT_TIMER);

    if (ev == PROCESS_EVENT_TIMER) {
       bbs_unlock();
       log_message("[bbs] *unlock0* ", "");
    }
    if (ev == shell_event_input) {
      input = data;
      switch (bbs_status.status) {

          case 0: {

            if(! strcmp(input->data1, "4")){
              log_message("[debug] encoding: ", input->data1);
              bbs_status.encoding=0;
			  //bbs_status.echo=1;
			  //bbs_status.width=40;
              strcpy(bbs_status.encoding_suffix, BBS_PET40_SUFFIX);
            }
            else if(! strcmp(input->data1, "2")){
              log_message("[debug] encoding: ", input->data1);
              bbs_status.encoding=0;
			  //bbs_status.echo=1;
			  bbs_status.width=21;
              strcpy(bbs_status.encoding_suffix, BBS_PET22_SUFFIX);
            }


            else if(! strcmp(input->data1, "l") || ! strcmp(input->data1, "L")){
              log_message("[debug] encoding: ", input->data1);
              bbs_status.encoding=1;
			  bbs_status.echo=0;
			  //bbs_status.width=40;
              strcpy(bbs_status.encoding_suffix, BBS_ASCII_SUFFIX);
            }
            else if(! strcmp(input->data1, "e") || ! strcmp(input->data1, "E")){
              log_message("[debug] encoding: ", input->data1);
              bbs_status.encoding=1;
			  //bbs_status.echo=1;
			  //bbs_status.width=40;
              strcpy(bbs_status.encoding_suffix, BBS_ASCII_SUFFIX);
            }

            else if(! strcmp(input->data1, "t") || ! strcmp(input->data1, "t")){
              log_message("[debug] encoding: ", input->data1);
              bbs_status.encoding=2;
			  //bbs_status.echo=1;
			  //bbs_status.width=40;
              strcpy(bbs_status.encoding_suffix, BBS_ASCII_SUFFIX);
            }


            else{
              shell_prompt(BBS_ENCODING_STRING);
              break;
            }
            bbs_banner(BBS_BANNER_LOGIN, bbs_status.encoding_suffix, BBS_SYS_DEVICE);
            shell_prompt("handle: ");
            bbs_status.status=1;
            break;
          }

          case 1: {
            if ((bbs_get_user(input->data1) != 0)) {

              if((int)strlen(input->data1)>12){
                 shell_output_str(NULL, "\r\nhandle can't be longer than 12 characters\n\r", "");
	         shell_prompt("handle: ");
                 bbs_status.status=1;
              }
              else{
                 shell_prompt("password: ");
                 bbs_status.status=2;
              }
            } 
            else {
              shell_output_str(&bbs_login_command, "login failed.", "");
              bbs_status.status=0;
              bbs_unlock();
              log_message("[bbs] *unlock1* ", "");
            }
            break;
          }

          case 2: {
            //if(! strcmp(input->data1, bbs_user.user_pwd)) {
              process_exit(&bbs_timer_process);
              bbs_status.status=3;
              log_message("[bbs] *login* ", bbs_user.user_name);

              bbs_banner(BBS_BANNER_LOGO, bbs_status.encoding_suffix, BBS_SYS_DEVICE);

              shell_output_str(NULL, "\r\nlast caller: ", bbs_status.last_caller);
              strcpy(bbs_status.last_caller, bbs_user.user_name);
              //Display the sub banner:
              bbs_sub_banner();
              set_prompt();
              shell_prompt(bbs_status.prompt);
              process_start(&bbs_timer_process, NULL);
              front_process=&shell_process;
            /*} else {
              shell_output_str(&bbs_login_command, "login failed.", "");
              bbs_unlock();
            }*/
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
  if (bbs_status.status>1)
     etimer_set(&bbs_session_timer, CLOCK_SECOND * BBS_TIMEOUT_SEC);
  else
     etimer_set(&bbs_session_timer, CLOCK_SECOND * BBS_LOGIN_TIMEOUT_SEC);

  while (1) {

     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&bbs_session_timer));

     if (ev == PROCESS_EVENT_TIMER) {
        if (bbs_status.status>1)
           process_post(PROCESS_BROADCAST, PROCESS_EVENT_TIMER, NULL);
        else
           process_post(&bbs_login_process, PROCESS_EVENT_TIMER, NULL);

        sprintf(szBuff, "session timeout.");
        shell_output_str(NULL, szBuff, "");
        if (bbs_status.status>1)
           etimer_set(&bbs_session_timer, CLOCK_SECOND * BBS_TIMEOUT_SEC);
        else
           etimer_set(&bbs_session_timer, CLOCK_SECOND * BBS_LOGIN_TIMEOUT_SEC);
     } else {
       if (ev == shell_event_input) 
         if (bbs_status.status>1)
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

  shell_output_str(&help_command, "Available commands:", "");
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
  PROCESS_BEGIN();

  bbs_banner(BBS_BANNER_LOGOUT, bbs_status.encoding_suffix, BBS_SYS_DEVICE);
  log_message("[bbs] *logout* ", bbs_user.user_name);
  bbs_unlock();
  log_message("[bbs] *unlock2* ", "");

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
    shell_output_str(NULL, commandline, ": command not found (try 'help')");
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
shell_output_str(struct shell_command *c, char *text1, const char *text2)
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
      if(started_process != NULL &&
	 ret == SHELL_FOREGROUND &&
	 process_is_running(started_process)) {
	front_process = started_process;
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_EXITED &&
				 data == started_process);
      }
      front_process = &shell_process;
    }

    if (ev == PROCESS_EVENT_TIMER){
       bbs_unlock();
       log_message("[bbs] *unlock3* ", "");
    }
    if(bbs_status.status>1) {
      /*etimer_set(&bbs_session_timer, CLOCK_SECOND * BBS_TIMEOUT_SEC);*/
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

  /* local console eye candy */
  clrscr();
  bordercolor(0);
  bgcolor(0);
  textcolor(5);
  bbs_splash(BBS_MODE_CONSOLE);

  bbs_init();
  bbs_setboard_init();
  //bbs_blist_init(); 
  bbs_read_init();
  bbs_post_init();
  //bbs_page_init(); 


  shell_event_input = process_alloc_event();
  
  process_start(&bbs_login_process, NULL);
  process_start(&shell_process, NULL);
  process_start(&shell_server_process, NULL);

  front_process = &bbs_login_process;

  bbs_status.status=0;
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
  bbs_lock();
  
  if(bbs_locked == 1) {
    shell_exit(); 
    log_message("[bbs] *busy*","");
  } else {
    bbs_locked=1;

    shell_output_str(NULL, PETSCII_LOWER, BBS_NAME);

    shell_prompt(BBS_ENCODING_STRING);

    front_process=&bbs_login_process;
  } 


}
/*---------------------------------------------------------------------------*/
void
shell_stop(void)
{
   if (bbs_locked==1) {
      log_message("[bbs] ", "*timeout*!");
   }

   /* set BBS parameters */
   bbs_locked=0;
   bbs_defaults();
   //bbs_config.msg_id=1;
   killall();
}
/*---------------------------------------------------------------------------*/
void
shell_quit(void)
{
  process_exit(&bbs_login_process);
  process_exit(&bbs_timer_process);
  process_exit(&shell_process);
  process_exit(&shell_server_process);
  shell_stop();
}
/*---------------------------------------------------------------------------*/

/** @} */
