/**
 * \file
 *         bbs-file.c - Contiki BBS file access functions
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#include "bbs-shell.h"
#include "bbs-file.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <em.h>

BBS_EM_REC bbs_em;

/*---------------------------------------------------------------------------*/
short bbs_filesize(char *filename, unsigned char device)
{
    struct cbm_dirent dirent;
    unsigned short fsize=0;

    if (cbm_opendir(1, device)==0) {
        while (!cbm_readdir(1, &dirent))
            if (strstr(dirent.name, filename)) 
               fsize=dirent.size;
        cbm_closedir(1);
    }
    return fsize*256; /* one block is 256 bytes */
}

/*---------------------------------------------------------------------------*/
void bbs_banner(unsigned char szBannerFile[12], unsigned char fileSuffix[3], unsigned char device) 
{
  unsigned char *buffer;
  unsigned short fsize=0;
  unsigned short i=0, siRet=0, len=0;
  unsigned char file[15];

  sprintf(file, "%s%s",szBannerFile,fileSuffix);

  log_message("[debug] ", file);

  fsize=bbs_filesize(file, device);

  if (fsize == 0) {
    shell_output_str(NULL, "", "error: file size\n\r");
    return;
  }

  buffer = (char*) malloc(fsize);

  if (buffer == NULL) {
    shell_output_str(NULL, "", "error: malloc \n\r");
    return;
  }

  memset(buffer, 0, fsize);
  siRet = cbm_open(10, device, 10, file);

  if (! siRet) {
     len = cbm_read(10, buffer, fsize);
     cbm_close(10);

     /*for (i=0; i<len; i++) {
         if (buffer[i] == '\r')
            buffer[i] = '\n';
     }*/
  }

  shell_output_str(NULL, "\n\r", buffer);
  
  if (buffer != NULL)
     free(buffer);
}



/*---------------------------------------------------------------------------*/

void em_load(unsigned char szBannerFile[12], unsigned char fileSuffix[3], unsigned char device) 
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

  fsize=bbs_filesize(file, device);

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
    /* Fill all pages */
    n=0;
	I=0;
    //for (I = 0; I < PageCount; ++I) {
	bbs_em.file[0][0] = I;	
	while (n<fsize && I < PageCount){
		++I;
    	/* Set the next page: */
		page = em_use (I);


        /* Copy the buffer to em one page at a time: */

	    for (i = 0; i < PAGE_SIZE; ++i, ++page, ++n) {
	        *page = buffer[n];
	    }
	    /* Now commit the page to extended memory: */
        em_commit ();
    }

	bbs_em.file[0][1] = I;

	if (buffer != NULL)
	 free(buffer);
}

/*---------------------------------------------------------------------------*/
int em_out(ST_FILE *pstFile, unsigned int uiBuffSize)
{
    unsigned I;
    unsigned PageCount;

	PageCount = em_pagecount ();

    /* Check all pages */
    for (I = 0; I < PageCount; ++I) {

        /* Get the buffer and compare it */
        //cmp (I, em_map (I), PAGE_SIZE, I);

    }

  //shell_output_str(NULL, "\n\r", buffer);
  return 0;	
}


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
int siFileExists(ST_FILE *pstFile)
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

/*unsigned char ucCheckDeviceNo(unsigned char *ucDeviceNo)
{
   if (*ucDeviceNo < 8 || *ucDeviceNo > 11)
      return 8;
   else 
   	  return *ucDeviceNo;
}*/
