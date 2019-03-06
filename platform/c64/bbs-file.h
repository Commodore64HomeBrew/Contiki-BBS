/**
 * \file
 *         bbs-file.h - Contiki BBS file access functions - header file
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cbm.h>

#include "bbs-shell.h"
#include "bbs-file.h"
#include <conio.h>
#include <em.h>

#define MAX_FILENAME 16
#define FILE_MODE_WRITE  0
#define FILE_MODE_APPEND 1

#define PAGE_SIZE       128                     /* Size in words */
#define BUF_SIZE        (PAGE_SIZE + PAGE_SIZE/2)

typedef struct
{
	char szFileName[MAX_FILENAME];
	unsigned char ucDeviceNo;
} ST_FILE;
//short bbs_filesize(char *prefix, char *filename, unsigned char device);
void bbs_banner(unsigned char filePrefix[10], unsigned char szBannerFile[12], unsigned char fileSuffix[3], unsigned char device, unsigned char wordWrap);

void stream_file(void);

const char * file_path(char *file, unsigned short num);

//void em_load(unsigned char filePrefix[10], unsigned char szBannerFile[12], unsigned char fileSuffix[3], unsigned char device, unsigned short file_num);
//void em_out(unsigned short file_num);

//int ssWriteSEQFile(ST_FILE *pstFile, short ssMode, void *pvBuffer, unsigned int uiBuffSize);
//int ssReadSEQFile(ST_FILE *pstFile, void *pvBuffer, unsigned int uiBuffSize);*/
//int ssStreamSEQFile(ST_FILE *pstFile, void *pvBuffer, unsigned int uiBuffSize);
//int ssWritePRGFile(ST_FILE *pstFile, void *pvBuffer, unsigned int uiBuffSize);
int siFileExists(ST_FILE *pstFile);
//unsigned char ucCheckDeviceNo(unsigned char *ucDeviceNo);
int siDriveStatus(ST_FILE *pstFile);
#endif
