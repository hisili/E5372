// CRC.h : main header file for the CRC application
//
#ifndef _CRC_H_
#define _CRC_H_


#ifndef DWORD
#define DWORD unsigned int
#endif
#ifndef BYTE
#define BYTE unsigned char
#endif
#ifndef WORD
#define WORD unsigned short
#endif

#ifndef TRUE
#define TRUE  1
#endif
int CalculateCRC(BYTE* pBuffer, DWORD dwCount, WORD* wCRC);

#endif
