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
  //unsigned char *file_buffer;
  //char file_buffer[BBS_BUFFER_SIZE];

  unsigned short fsize;
  unsigned short siRet=0, len=0;
  unsigned short i=0, j=0;
  unsigned short line=0;
  unsigned short col, preCol;
  unsigned short width;
  unsigned char file[25];
  unsigned short ptr;
  unsigned char c;

  //Blank the screen to speed things up
  poke(0xd011, peek(0xd011) & 0xef);

  sprintf(file, "%s%s",szBannerFile, fileSuffix);
  log_message("\x9fread: ", file);

  //log_message("[debug] ", file);
  ptr = buf.ptr;
  //fsize=bbs_filesize(filePrefix, file, device);
  fsize = BBS_BUFFER_SIZE-ptr;
  //sprintf(file, "%d",fsize);
  //log_message("[debug] fsize:", file);


  sprintf(file, "%s:%s%s",filePrefix, szBannerFile, fileSuffix);
  //log_message("[debug] file banner2: ", file);

  //memset(buf.bufmem, 0, fsize);


  //cbm_load(const char* name, unsigned char device, void* data)
  //cbm_load(file, device, &buffer);

  //siRet = cbm_open(10, device, 10, file);
  cbm_open(10, device, 10, file);
  //if (! siRet) {

	  /*if (bbs_status.status == STATUS_READ){
			cbm_read(10, &buf.bufmem[2], 2);
      //fsize=fsize-2;
	  }*/


  if (bbs_status.status == STATUS_READ){
    buf.ptr += cbm_read(10, &buf.bufmem[ptr] , fsize);
    
    if(bbs_status.encoding==1){
      petscii_to_ascii(&buf.bufmem[ptr], buf.ptr-ptr);
    }

    buf.bufmem[buf.ptr++]= ISO_cr;
    buf.bufmem[buf.ptr]= ISO_nl;

  }
  else{
    buf.ptr += cbm_read(10, &buf.bufmem[ptr+2] , fsize) + 2;
  }

  buf.bufmem[ptr]= ISO_cr;
  buf.bufmem[ptr+1]= ISO_nl;

  //fsize = fsize-2;  
  
  cbm_close(10);

	//buf.ptr = strlen(buf.bufmem);

  if (wordWrap==1){

    width = bbs_status.width;
    col=0;
    preCol=0;
    for (i=ptr; i<buf.ptr; i++) {
      /*
      if (line == bbs_status.lines) {
          line=0;
          shell_output_str(NULL, "\n\r\x05", buf.bufmem);
          shell_prompt("\n\rreturn to continue");
      }
      */
      c=buf.bufmem[i];

      if (col == width){

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

      }
      else if (c == ISO_cr || c == ISO_nl){
      	col=0;
        ++line;
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
int siDriveStatus(ST_FILE *pstFile)
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

   /*printf("\n%hhu,%s,%hhu,%hhu\n", (int) e, msg, (int) t, (int) s);*/

   return (int) e;
}
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
