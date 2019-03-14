/**
 * \file
 *         bbs-read.c - read msg. from Contiki BBS message boards
 * \author
 *         (c) 2099-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */


#include "contiki.h"
#include "bbs-shell.h"
#include "bbs-read.h"
#include "bbs-file.h"
#include "bbs-telnetd.h"
#include "bbs-encodings.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern BBS_BOARD_REC board;
extern BBS_CONFIG_REC bbs_config;
extern BBS_STATUS_REC bbs_status;
extern BBS_USER_STATS bbs_usrstats;
extern BBS_BUFFER buf;

/*---------------------------------------------------------------------------*/

void open_msg(unsigned short num)
{

  unsigned char file2[40];
  unsigned char file[20];

  set_prompt();
  bbs_status.status=STATUS_READ;

  //Blank the screen to speed things up
  poke(0xd011, peek(0xd011) & 0xef);

  bbs_status.wrap = 1;

  sprintf(file, "%d-%d", bbs_status.board_id, num);

  sprintf(file2, "%s:%s",file_path(file, num), file);

  cbm_open(10, board.subs_device, 10, file2);

}

/*---------------------------------------------------------------------------*/

int read_msg()
{



  unsigned short fsize, bytes_read;
  unsigned short i=0, j=0;
  unsigned short line=0;
  unsigned short col, preCol;
  unsigned short width;
  unsigned short ptr;
  unsigned char c;



  ptr = buf.ptr;

  fsize = BBS_BUFFER_SIZE-ptr;

  bytes_read = cbm_read(10, &buf.bufmem[ptr] , fsize);

  buf.ptr += bytes_read;

  if(bbs_status.encoding==1){
    petscii_to_ascii(&buf.bufmem[ptr], buf.ptr-ptr);
  }

  /*if(more==2){
    buf.bufmem[buf.ptr++]= ISO_cr;
    buf.bufmem[buf.ptr]= ISO_nl;
  }*/

  
  if (bbs_status.wrap==1){

    width = bbs_status.width;
    col=0;
    preCol=0;
    i=ptr;
    line=0;
    //for (i=ptr; i<buf.ptr; i++) {
    while(line<24 && i<bytes_read) {
      c=buf.bufmem[i++];

      if (col >= width){

        //We're at the end of the row. Walk back until you find a space and then insert a CR:
        j=i;
        while(buf.bufmem[j] != PETSCII_SPACE && j>preCol){
          --j;
        }
        //Space is found; insert CR:
        if(bbs_status.encoding==1){
          buf.bufmem[j] = ISO_nl;
        }
        else{
          buf.bufmem[j] = ISO_cr;
        }
        //Record counter position of previous line:
        preCol=j;
        //Set the new column counter, taking into account the wrapped word:
        col=i-j;
        ++line;
        printf("width line: %d\n",line);


      }
      else if (c == ISO_cr){// || c == ISO_nl){
        col=0;
        ++line;
        printf("CR lines: %d\n",line);

      }
      else if (c==0x05 || c==0x1c || c==0x1e || c==0x1f|| c==0x81 || c==0x90 || c==0x95 || c==0x96 || c==0x97 || c==0x98 || c==0x99 || c==0x9a || c==0x9b || c==0x9c || c==0x9e || c==0x9f){
        //nothing
      }
      else if(c==PETSCII_UP || c==PETSCII_DOWN || c==PETSCII_LEFT || c==PETSCII_RIGHT || c==PETSCII_CLRSCN || c==PETSCII_HOME){


        if(c==PETSCII_LEFT){
          if(col>0){--col;}
        }
        else if(c==PETSCII_RIGHT){
          ++col;
        }
        else if(c==PETSCII_UP){
          if(line>0){--line;}
          col=0;
        }
        else if(c==PETSCII_DOWN){
          ++line;
          col=0;
        }
        else if(c==PETSCII_HOME || c==PETSCII_CLRSCN){
          col=0;
          line=0;
        }
      }
      else{
        ++col;
      }
    }
  }
  //This is just for testing...
  buf.ptr = i;

  printf("lines: %d bytes: %d i: %d\n",line, bytes_read, i);

  //if(bytes_read==fsize)

  return fsize-bytes_read;

}

/*---------------------------------------------------------------------------*/

void close_msg()
{
  cbm_close(10);

  bbs_status.status=STATUS_LOCK;

  //Turn on the screen again
  poke(0xd011, peek(0xd011) | 0x10);
}

/*---------------------------------------------------------------------------*/
PROCESS(bbs_read_process, "read");
SHELL_COMMAND(bbs_read_command, "#", "# : select message #", &bbs_read_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_read_process, ev, data)
{

  struct shell_input *input;
  char message[40];
  unsigned short num;

  PROCESS_BEGIN();

  shell_output_str(NULL,PETSCII_LOWER, PETSCII_WHITE);

  sprintf(message, "\n\rselect msg (1-%d): ", bbs_config.msg_id[bbs_status.board_id]);
  shell_prompt(message);


  PROCESS_WAIT_EVENT_UNTIL(ev == shell_event_input);
  input = data;
  num = atoi(input->data1);
  bbs_usrstats.current_msg[bbs_status.board_id] = num;

  if(num>0 && num <= bbs_config.msg_id[bbs_status.board_id]){
    open_msg(num);
    read_msg();
    close_msg();
  }

  PROCESS_EXIT();

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/

PROCESS(bbs_nextmsg_process, "nextmsg");
SHELL_COMMAND(bbs_nextmsg1_command, "\x0d", "", &bbs_nextmsg_process);
SHELL_COMMAND(bbs_nextmsg2_command, "\x0a", "ret : read next message", &bbs_nextmsg_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(bbs_nextmsg_process, ev, data)
{
  unsigned short num;

  PROCESS_BEGIN();

  num = bbs_usrstats.current_msg[bbs_status.board_id]+1;

  if(num>0 && num <= bbs_config.msg_id[bbs_status.board_id]){
    ++bbs_usrstats.current_msg[bbs_status.board_id];
    
    shell_output_str(NULL,PETSCII_LOWER, PETSCII_WHITE);
    open_msg(num);
    read_msg();
    close_msg();
  }

  PROCESS_EXIT();
   
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
void
bbs_read_init(void)
{
  shell_register_command(&bbs_read_command);
  shell_register_command(&bbs_nextmsg1_command);
  shell_register_command(&bbs_nextmsg2_command);
}

