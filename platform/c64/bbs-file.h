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

#define MAX_FILENAME 16
#define FILE_MODE_WRITE  0
#define FILE_MODE_APPEND 1


typedef struct
{
	char szFileName[MAX_FILENAME];
	unsigned char ucDeviceNo;
} ST_FILE;

int ssInitRELFile(ST_FILE *pstFile, void *pvBuffer, unsigned int uiBuffSize, unsigned int uiRecCount);
int ssWriteRELFile(ST_FILE *pstFile, void *pvBuffer, unsigned int uiBuffSize, unsigned int uiRecNo);
int ssReadRELFile(ST_FILE *pstFile, void *pvBuffer, unsigned int uiBuffSize, unsigned int uiRecNo);
/*int ssWriteSEQFile(ST_FILE *pstFile, short ssMode, void *pvBuffer, unsigned int uiBuffSize);
int ssReadSEQFile(ST_FILE *pstFile, void *pvBuffer, unsigned int uiBuffSize);*/
int ssStreamSEQFile(ST_FILE *pstFile, void *pvBuffer, unsigned int uiBuffSize);
int siFileExists(ST_FILE *pstFile);
unsigned char ucCheckDeviceNo(unsigned char *ucDeviceNo);
int siDriveStatus(ST_FILE *pstFile);
#endif
