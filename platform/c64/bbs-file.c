/**
 * \file
 *         bbs-file.c - Contiki BBS file access functions
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#include "shell.h"
#include "bbs-file.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
/*
int ssInitRELFile(ST_FILE *pstFile, void *pvBuffer, unsigned int uiBuffSize, unsigned int uiRecCount)
{
  char command[40];
  char cbm_filename[40];  
  int siRet=0;
  int siCount=0;
  
  int channel, hi, lo;
  
  channel=3+96;

  sprintf(cbm_filename,"0:%s,l,%c", pstFile->szFileName, uiBuffSize);
  
  siRet = cbm_open( 15, pstFile->ucDeviceNo, 15, NULL);
  siRet = cbm_open( 2, pstFile->ucDeviceNo,  3, cbm_filename);

  for (siCount = uiRecCount; siCount >= 1; siCount--)
  {
     hi=(siCount/256);
     lo=siCount-(hi*256);
    
     sprintf(command, "p%c%c%c", channel, lo, hi);    
     siRet=cbm_write( 15, command, strlen(command));
     siRet = cbm_write( 2, pvBuffer, uiBuffSize); 
  }

  cbm_close(15);  
  cbm_close(2);
  return siRet;	
}

int ssWriteRELFile(ST_FILE *pstFile, void *pvBuffer, unsigned int uiBuffSize, unsigned int uiRecNo)
{
  char command[40];
  char cbm_filename[40];
  int siRet=0;
  
  int channel, hi, lo;
  
  channel=3+96;
  
  sprintf(cbm_filename,"0:%s,l,%c", pstFile->szFileName, uiBuffSize);
  
  siRet = cbm_open( 15, pstFile->ucDeviceNo, 15, NULL);
  siRet = cbm_open( 2, pstFile->ucDeviceNo,  3, cbm_filename);    

  hi=(uiRecNo/256);
  lo=uiRecNo-(hi*256);
  
  sprintf(command, "p%c%c%c", channel, lo, hi);    
  siRet=cbm_write( 15, command, strlen(command));

  siRet = cbm_write( 2, pvBuffer, uiBuffSize);

  cbm_close(15);  
  cbm_close(2);
    
  return siRet;
}

int ssReadRELFile(ST_FILE *pstFile, void *pvBuffer, unsigned int uiBuffSize, unsigned int uiRecNo)
{
  char command[40];
  char cbm_filename[40];
  int siRet=0;
  
  int channel, hi, lo;
  
  channel=3+96;

  sprintf(cbm_filename,"0:%s,l,%c", pstFile->szFileName, uiBuffSize);

  siRet = cbm_open( 15, pstFile->ucDeviceNo, 15, NULL);
  siRet = cbm_open( 2, pstFile->ucDeviceNo,  3, cbm_filename);    

  hi=(uiRecNo/256);
  lo=uiRecNo-(hi*256);
    
  sprintf(command, "p%c%c%c", channel, lo, hi);    
  siRet=cbm_write( 15, command, strlen(command));
  
  siRet = cbm_read( 2, pvBuffer, uiBuffSize);
  
  cbm_close(15);  
  cbm_close(2); 
       
  return siRet;
}
*/
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

/*int ssWritePRGFile(ST_FILE *pstFile, void *pvBuffer, unsigned int uiBuffSize)
{
  int siRet=0;
  char szTmp[15];
  
  strcpy(szTmp,"@:");
  strcat(szTmp, pstFile->szFileName);
  strcat(szTmp, ",p,w");
  //(ssMode != 0) ? strcat(szTmp, ",s,a") : strcat(szTmp, ",s,w");
   
  siRet = cbm_open(10, pstFile->ucDeviceNo, 10, szTmp);
    
  if (! siRet)
  {
     if (pvBuffer != NULL) {       
        cbm_write(10, pvBuffer, uiBuffSize);   
     }
  } else {
    cbm_close(10);
    return siRet;
  }

  cbm_close(10);
    
  return siRet;
}
*/

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

unsigned char ucCheckDeviceNo(unsigned char *ucDeviceNo)
{
   if (*ucDeviceNo < 8 || *ucDeviceNo > 11)
      return 8;
   else 
   	  return *ucDeviceNo;
}
