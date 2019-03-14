/**
 * \file
 *         bbs-file.c - Contiki BBS file access functions
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#include "bbs-shell.h"
#include "bbs-file.h"
#include "bbs-defs.h"
#include "bbs-telnetd.h"
#include "bbs-encodings.h"
#include "contiki-net.h"

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <em.h>

BBS_EM_REC bbs_em;
extern BBS_STATUS_REC bbs_status;
extern BBS_BOARD_REC board;

extern BBS_BUFFER buf;
//extern telnetd_buf buf;
//static telnetd_buf buf;
extern TELNETD_STATE s;
//PROCESS(file_read_process, "file");

/*---------------------------------------------------------------------------*/
/*short bbs_filesize(char *prefix, char *filename, unsigned char device)
{
    struct cbm_dirent dirent;
    unsigned short fsize=0;
    char dir[12];

    sprintf(dir,"cd%s",prefix);

    cbm_open(1, device, 15, dir);
    cbm_close(1);

    if (cbm_opendir(1, device)==0) {
        while (!cbm_readdir(1, &dirent))
            if (strstr(dirent.name, filename)) 
               fsize=dirent.size;
        cbm_closedir(1);
    }
    cbm_open(1, device, 15, "cd//");
    cbm_close(1);

    return fsize*256; 
}*/

/*---------------------------------------------------------------------------*/
void bbs_banner(unsigned char filePrefix[20], unsigned char szBannerFile[12], unsigned char fileSuffix[3], unsigned char device, unsigned char wordWrap)//, unsigned char encodeToggle) 
{

  unsigned short fsize;
  unsigned char file[25];
  unsigned short ptr;


  //Blank the screen to speed things up
  poke(0xd011, peek(0xd011) & 0xef);


  //bbs_status.wrap = wordWrap;

  sprintf(file, "%s%s",szBannerFile, fileSuffix);
  log_message("\x9fread: ", file);

  //log_message("[debug] ", file);
  ptr = buf.ptr;

  fsize = BBS_BUFFER_SIZE-ptr;

  sprintf(file, "%s:%s%s",filePrefix, szBannerFile, fileSuffix);

  cbm_open(10, device, 10, file);


  buf.ptr += cbm_read(10, &buf.bufmem[ptr+2] , fsize) + 2;


  buf.bufmem[ptr]= ISO_cr;
  buf.bufmem[ptr+1]= ISO_nl;

  
  cbm_close(10);

  //Turn on the screen again
  poke(0xd011, peek(0xd011) | 0x10);

}

/*---------------------------------------------------------------------------*/
const char * file_path(char *file, unsigned short num)
{
  unsigned char sub_num_prefix[20];
    
    if(board.dir_boost==1){

      if(num<10){
        sprintf(sub_num_prefix, "%s%d/0/0/0/%c/", board.subs_prefix,bbs_status.board_id, file[2]);
      }
      else if(num<100){
        sprintf(sub_num_prefix, "%s%d/0/0/%c/%c/", board.subs_prefix,bbs_status.board_id, file[2], file[3]);
      }
      else if(num<1000){
        sprintf(sub_num_prefix, "%s%d/0/%c/%c/%c/", board.subs_prefix,bbs_status.board_id, file[2], file[3],file[4]);
      }
      else if(num<10000){
        sprintf(sub_num_prefix, "%s%d/%c/%c/%c/%c/", board.subs_prefix,bbs_status.board_id, file[2], file[3],file[4],file[5]);
      }
    }
    else {
      sprintf(sub_num_prefix, "%s%d/", board.subs_prefix,bbs_status.board_id);
    }

    return sub_num_prefix;
}



//uip_send(&c,1);


//m/terror.prg
//m/tmnt.prg

/*---------------------------------------------------------------------------*/
/*PROCESS_THREAD(bbs_read_file, ev, data)
{
  struct shell_input *input;
  int return_code;

  PROCESS_BEGIN();

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(ev == shell_event_input || ev == PROCESS_EVENT_TIMER);

    if (ev == PROCESS_EVENT_TIMER) {
       shell_stop();
       log_message("\x9a","event timer");
    }
    if (ev == shell_event_input) {
      input = data;
      switch (bbs_status.status) {

          case STATUS_CONFUSR: {

            if(! strcmp(input->data1, "y") || ! strcmp(input->data1, "Y")){

              shell_output_str(NULL, "\r\nhit return to continue", "");
              bbs_status.status=STATUS_STATS;

            }
            else{
              shell_prompt("\n\rhandle: ");
              bbs_status.status=STATUS_HANDLE;
            }
            break;
          }
          case STATUS_STATS: {
            if(strlen(input->data1)>0) {
              bbs_login();
            }
            break;
          }

       }
    }
  }

  PROCESS_END();
}*/

/*---------------------------------------------------------------------------*/
/*
void em_load(unsigned char filePrefix[10], unsigned char szBannerFile[12], unsigned char fileSuffix[3], unsigned char device, unsigned short file_num) 
{
  unsigned char *buffer;
  unsigned short fsize=0;
  unsigned short i=0, siRet=0, len=0; 
  int *page,n;
  unsigned char file[15];
  unsigned I;
  unsigned PageCount;




  sprintf(file, "%s%s",szBannerFile,fileSuffix);

  log_message("[bbs] em loaded: ", file);

  fsize=bbs_filesize(filePrefix, file, device);

  buffer = (char*) malloc(fsize);

  memset(buffer, 0, fsize);
  siRet = cbm_open(10, device, 10, file);

  if (! siRet) {
     len = cbm_read(10, buffer, fsize);
     cbm_close(10);

  }

  shell_output_str(NULL, "\n\r", buffer);
  

  //+++++++++++++++++++++++++++++++++++++++++++++++++++

	PageCount = em_pagecount ();
    // Fill all pages 
  n=0;
	I=0;
    //for (I = 0; I < PageCount; ++I) {
	bbs_em.file[0][0] = I;	
	while (n<fsize && I < PageCount){
		++I;
    	// Set the next page:
		page = em_use (I);


        // Copy the buffer to em one page at a time:

	    for (i = 0; i < PAGE_SIZE; ++i, ++page, ++n) {
	        *page = buffer[n];
	    }
	    //Now commit the page to extended memory: 
        em_commit ();
    }

	bbs_em.file[0][1] = I;

	if (buffer != NULL)
	 free(buffer);
}
*/
/*---------------------------------------------------------------------------*/
/*
void em_out(unsigned short file_num)
{

  unsigned short i,n;
  unsigned I;
  unsigned PageCount;
  register const unsigned* em_buf;
  unsigned char *buffer;

  buffer = (char*) malloc((bbs_em.file[file_num][1] - bbs_em.file[file_num][0])*PAGE_SIZE);

  PageCount = em_pagecount ();

  if(bbs_em.file[file_num][1] <= PageCount){
    n=0;
    // Check all pages
    for (I = bbs_em.file[file_num][0]; I <= bbs_em.file[file_num][1]; ++I) {

        em_buf = em_map(I);

        //buf_append(&buf, em_buf, PageCount);

        for (i = 0; i < PAGE_SIZE; ++i, ++em_buf, ++n) {
          buffer[n] = *em_buf;
        }

        // Get the buffer and compare it
        //cmp (I, em_map (I), PAGE_SIZE, I);

    }
  }
  shell_output_str(NULL, "\n\r", buffer);
}

*/
/*---------------------------------------------------------------------------*/

/*int ssWriteSEQFile(ST_FILE *pstFile, short ssMode, void *pvBuffer, unsigned int uiBuffSize)
{
  int siRet=0;
  char szTmp[15];
  
  strcpy(szTmp,"@:");
  strcat(szTmp, pstFile->szFileName);  
  //(ssMode != 0) ? strcat(szTmp, ",s,a") : strcat(szTmp, ",s,w");
  strcat(szTmp, ",s,w");
  siRet = cbm_open(10, pstFile->ucDeviceNo, 10, szTmp);
  log_message("[bbs] *szTmp* ", szTmp);
  
  if (! siRet)
  {
     if (pvBuffer != NULL) {
        log_message("[bbs] *write* ", pvBuffer);     
        cbm_write(10, pvBuffer, uiBuffSize);   
     }
  } else {
    cbm_close(10);
    return siRet;
  }

  cbm_close(10);
    
  return siRet;
}*/
/*
int ssReadSEQFile(ST_FILE *pstFile, void *pvBuffer, unsigned int uiBuffSize)
{
  int siRet=0;
  char szTmp[15];
 
  memset(pvBuffer, 0, uiBuffSize);
 
  strcpy(szTmp, pstFile->szFileName);
  strcat(szTmp, ",s,r");
   
  siRet = cbm_open(10, pstFile->ucDeviceNo, 10, szTmp);
 
  if (! siRet)
  {
     cbm_read(10, pvBuffer, uiBuffSize);
  } else {
    cbm_close(10);
    return siRet;
  }

  cbm_close(10);

  return siRet;    
}
*/
/*int ssStreamSEQFile(ST_FILE *pstFile, void *pvBuffer, unsigned int uiBuffSize)
{
  int i;
  int siRet=0;
  char szTmp[15];
  char in[1];

  memset(pvBuffer, 0, uiBuffSize);
 
  strcpy(szTmp, pstFile->szFileName);
  strcat(szTmp, ",s,r");
   
  siRet = cbm_open(10, pstFile->ucDeviceNo, 10, szTmp);
 
  if (! siRet)
  {
    for(i=0;i<uiBuffSize;i++){
      cbm_read(10, in, 1);
      shell_output_str(NULL, in, "");
      //buf_append(&buf, in, 1);
    }  
  } else {
    cbm_close(10);
    return siRet;
  }

  cbm_close(10);

  return siRet;    
}*/


/*---------------------------------------------------------------------------*/
/*int siDriveStatus(ST_FILE *pstFile)
{
   unsigned char ucBuff[128];
   unsigned char msg[40];
   unsigned char e, t, s;


   if (cbm_open(1, pstFile->ucDeviceNo, 15, "") == 0) {
   
      if ( cbm_read(1, ucBuff, sizeof(ucBuff)) < 0) {
         return -1;
      }
      cbm_close(1);
   }

   if (sscanf(ucBuff, "%hhu, %[^,], %hhu, %hhu", &e, msg, &t, &s) != 4) {
      printf("\nparse error\n");
      puts(ucBuff);
      return -1;
   }

   //printf("\n%hhu,%s,%hhu,%hhu\n", (int) e, msg, (int) t, (int) s);

   return (int) e;
}*/
/*---------------------------------------------------------------------------*/
/*int siFileExists(ST_FILE *pstFile)
{
   unsigned char ucBuff[128];
   unsigned char szTmp[15];
   unsigned char msg[40];
   unsigned char e, t, s;
   int siRet=0;

   strcpy(szTmp,"@:");
   strcat(szTmp, pstFile->szFileName);  
   strcat(szTmp, ",p,r");

   cbm_open( 15, pstFile->ucDeviceNo, 15, NULL);
   cbm_open( 2, pstFile->ucDeviceNo,  3, pstFile->szFileName);    

   if ( cbm_read(15, ucBuff, sizeof(ucBuff)) < 0) {
      return -1;
   }

   cbm_close(15);

   if (sscanf(ucBuff, "%hhu, %[^,], %hhu, %hhu", &e, msg, &t, &s) != 4) {
      puts("parse error");
      puts(ucBuff);
      return -1;
   }

   cbm_close(2);
   cbm_close(15);

   return (int) e;
}
*/
/*unsigned char ucCheckDeviceNo(unsigned char *ucDeviceNo)
{
   if (*ucDeviceNo < 8 || *ucDeviceNo > 11)
      return 8;
   else 
   	  return *ucDeviceNo;
}*/
