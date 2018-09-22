#if(FEATURE_UPDATEONLINE == FEATURE_ON)
#ifndef  _UPDATE_COMMON_H_
#define  _UPDATE_COMMON_H_

#include <mach/platform.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include "ptable_def.h"

#ifndef UINT32
#define UINT32  unsigned int
#endif

#ifndef UINT8
#define UINT8   unsigned char
#endif

#ifndef UINT16
#define UINT16   unsigned short
#endif

#define OSAL_OK            (0)
#define OSAL_ERROR         (-1) 

#define UPDATE_FILE_LOG_PATH  "/online/update.log"
#define UPDATE_FILE_LOG_MAX   0x800
#define LOG_BUF_LEN           256

typedef enum 
{
    NV_NO_BACKUP = 0,
    NV_BACKUP = 1
    
}NV_BACKUP_FLAG_STATUS;

#ifndef MOBILE_CONNECT_HD_ADDR 
#define MOBILE_CONNECT_HD_ADDR 0xA0000    /*�����ն��������̨����������߶����ISO�ļ�ͷ�ĵ�ַ*/
#define MOBILE_CONNECT_ADDR    0xB0000    /*�����ն��������̨����������߶����ISO�ļ��ĵ�ַ*/
#define WEBUI_HD_ADDR 0x550000    /*�����ն������WEBUI����������߶����WEBUI�ļ��汾�ĵ�ַ*/
#define WEBUI_ADDR    0x560000    /*�����ն������WEBUI����������߶����WEBUI�ļ��ĵ�ַ*/
#endif

void writeM_u32(UINT32 pAddr, UINT32 value);
void  ImageTypeChange(int* pType);
int update_nv_backup_flag_set(int flag);
void update_record_file(char* traceLog);
 
#endif /* _UPDATE_COMMON_H_ */
#endif /*FEATURE_UPDATEONLINE*/

