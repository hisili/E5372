/*************************************************************************
*   ��Ȩ����(C) 2011-2030, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  SDUpdate.c
*
*
*   ��    �� :  SD����A���ļ�У��
*
*************************************************************************/
#include "generated/FeatureConfigDRV.h"

#include"SDUpdate.h"
#include "BSP.h"
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/thread_info.h>
#include <linux/mmc/sd.h>

#include <linux/dcache.h>
#include <linux/delay.h>
#include "../../mntn/include/omDrv.h"
#include <mach/gpio.h>
#include "CRC.h"
#include <linux/fs.h>
#include <mach/common/bsp_memory.h>

#include "../../led_drv/balong_led_drv.h"
#include <linux/netlink.h>
#include <mach/platform.h>
#include "../../nvim/balong_nvim.h"
#include "../../../init/do_mounts.h"

/*g_inFatChecking: TRUE=sdupdate; FALSE=is not sdupdate.*/
BOOL   g_inFatChecking = FALSE; 

#if (FEATURE_SDUPDATE == FEATURE_ON)

#define BUFF_SIZE                   (1024)

#define CCA_THRESH_MILLI			(14)
#define NV_ITEM_FACTORY				(36)
#define INT8                        char

char TFUP_FnSD[64] = {0};

char TFUP_FnFlash[64] = {0};

signed char  g_verSoft[31] = {0};  /*�汾��Ϣ*/

unsigned  char  g_dloadID[128];/* ����dloadid*/  
int g_updateFileNum = 0;/*�����ļ�����*/

TFFILE updateFile[UPFILENUM_MAX]={{NULL,OTHERSOFT},{NULL,OTHERSOFT}};/*�洢bin�ļ�·��*/

TFFILE verifyFile[VEFILENUM_MAX]={{NULL,OTHERSOFT},{NULL,OTHERSOFT}};/*�洢����Ҫ���lic�ļ���·��*/

char  fileNameList[FILENAME_NUM_MAX][FILENAME_LEN_MAX]={0}; /*����ȡ��dir���ļ���*/

static struct semaphore s_sem_getInfo;
extern int sprintf(char *buf, const char *fmt, ...);
extern int snprintf(char *buf, size_t size, const char *fmt, ...);

#if (FEATURE_OLED == FEATURE_ON)
extern BSP_VOID oledClearWholeScreen(BSP_VOID);
extern BSP_VOID oledStringDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucStr);
#elif (FEATURE_TFT == FEATURE_ON)
extern BSP_VOID tftClearWholeScreen(BSP_VOID);
extern BSP_VOID tftStringDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucStr);
#endif

extern BSP_S32 DR_NV_Read(BSP_U16 usID,BSP_VOID *pItem,BSP_U32 ulLength);
extern int mmc_get_disk_devidx(void);
extern int BSP_PWRCTRL_SD_LowPowerExit(void);
extern int BSP_PWRCTRL_SD_LowPowerEnter(void);

struct linux_dirent {
	unsigned long	d_ino;
	unsigned long	d_off;
	unsigned short	d_reclen;
	char		d_name[1];
};


/*******************************************************************************
*
* writeM - write a UINT32 value to perihical address
*
*
* RETURNS: N/A
********************************************************************************/
extern void writeM(UINT32 pAddr, UINT32 value);

/***************************************************************
*��������:hi_TFUP_LogPrintf
*
*��������:�������ݵ�TF��
*
*�������:��������
*
*������: 
*
*����ֵ:���ݳ���
****************************************************************/
 int hi_TFUP_LogPrintf(const char *fmt)
{
      int  ret = 0x00;

	int fd1 = 0;
	int fd2 = 0;
	mm_segment_t sd_fs = 0;

	sd_fs = get_fs();
	set_fs(KERNEL_DS);
	if ((fd1 = sys_open(TFUP_FnSD, O_RDWR|O_CREAT, 0644))>=0)
	{
		if (TF_LOGSIZE_MAX <= sys_lseek(fd1, 0, SEEK_END))
		{
			sys_close(fd1);
			sys_rmdir(TFUP_FnSD);
			fd1 = sys_open(TFUP_FnSD, O_RDWR|O_CREAT, 0644);            
		}

        if(fd1>=0)
        {
            ret = sys_write(fd1, fmt, strlen(fmt)); 
        }
		sys_close(fd1);
	}

	if ((fd2 = sys_open(TFUP_FnFlash, O_RDWR|O_CREAT, 0644))>=0)
	{  
		if (TF_LOGSIZE_MAX <= sys_lseek(fd2, 0, SEEK_END))
		{
			sys_close(fd2);
			sys_rmdir(TFUP_FnFlash);                                              
			fd2 = sys_open(TFUP_FnFlash, O_RDWR|O_CREAT, 0644);
		}

		if(fd2>=0)
		{
		     ret |= sys_write (fd2, fmt, strlen(fmt));
		}
		sys_close(fd2);
	}
    sys_sync();
	set_fs(sd_fs);
	return (ret);                 
    
}

/******************************************************************************
*  Function:  hi_TFUP_setTFUpdateFlag
*  Description:
*      set Update Flag for NORMAL update and LED control 
*
*  Calls:
*      
*
*  Called By:
*      TFUP_DloadTask
*      TFUP_CheckFileValidify
*
*  Data Accessed: 
*
*  Data Updated: 
*
*  Input:
*         flag: UPDATE STATE 1:���ñ�־λ 0:�����־λ
*        
*  Output:
*         NONE
*         
*  Return:
*         OSAL_OK: the function successful returned.
*         OSAL_ERROR: the function encounter OSAL_ERROR while running.
*
********************************************************************************/
void hi_TFUP_setTFUpdateFlag(TFUP_FLAG_E eFlag)
{
    if (TFUP_RUNNING == eFlag)  /*1��λ*/
    {
        writeM(TCM_FOR_TFUP_STATUS_FLAG_ADDR,TCM_FOR_TFUP_STATUS_RUNNING_VAL);
    }
    else if(TFUP_DONE == eFlag)
    {
        writeM(TCM_FOR_TFUP_STATUS_FLAG_ADDR,TCM_FOR_TFUP_STATUS_DONE_VAL);
    }    
    else
    {
        writeM(TCM_FOR_TFUP_STATUS_FLAG_ADDR,0);
    }
}

 /***********����ֵΪ1:LED�ܿ�,��������.����ֵΪ0:LED����,ǿ������*********************/
TFUP_FLAG_E hi_TFUP_getTFUpdateFlag(void)
{
    UINT32 ucTmp = 0;

    ucTmp = *(volatile UINT32 *)(TCM_FOR_TFUP_STATUS_FLAG_ADDR);

    if(TCM_FOR_TFUP_STATUS_RUNNING_VAL == ucTmp)
    {
        return TFUP_RUNNING;
    }
    else if(TCM_FOR_TFUP_STATUS_DONE_VAL == ucTmp)
    {
        return TFUP_DONE;
    }
    else 
    {
        return TFUP_IDLE;
    }    
}

/**************************����SD�����ɹ���־��1:�ɹ���0:ʧ��*************/

int hi_TFUP_SetSuccessFlag(int flag)
{
    if (SUCCESSFLAG == flag)
    {
        writeM(TCM_FOR_SD_UPDATE_RESULT_FLAG,SD_UPDATE_RESULT_FLAG_VALUE);
        return OSAL_OK;
    }
    else if(TFUPUNDOFLAG == flag)
    {
        writeM(TCM_FOR_SD_UPDATE_RESULT_FLAG,0);
        return OSAL_OK;
    }
    else
    {
        return OSAL_ERROR;
    }

}
/**************************�õ�TF�������ɹ���־***********************************/
int hi_TFUP_GetSuccessFlag(void)
{
    UINT32 ucTmp = 0;

    ucTmp = *(volatile UINT32 *)(TCM_FOR_SD_UPDATE_RESULT_FLAG);

    if( ucTmp == SD_UPDATE_RESULT_FLAG_VALUE )
    {
        return SUCCESSFLAG;
    }
    else 
    {
        return TFUPUNDOFLAG;
    }

}

int hi_TFUP_GetGoOnFlag(void)
{
    return (SDUPDATE_GOON_MAGIC == SDUPDATE_GOON_FLAG);
}

int hi_TFUP_setTFDashFlag(bool flag)
{
	if(false == flag)
	{
		writeM(TCM_FOR_UPDATE_NVBACKUP_ADDR,UPDATE_NO_NVBACKUP_MODE);
	}
	else
	{
		writeM(TCM_FOR_UPDATE_NVBACKUP_ADDR,UPDATE_NVBACKUP_MODE);
	}

	return OSAL_OK;
}

bool hi_TFUP_getTFDashFlag(void)
{
	return (UPDATE_NVBACKUP_MODE == UPDATE_NVBACKUP_FLAG);
}

int hi_TFUP_setTFUpdateSource(int flag)
{
    if (false == flag)
    {
        writeM(TCM_FOR_SD_FORCE_UPDATE,0);                
    }
    else
    {
        writeM(TCM_FOR_SD_FORCE_UPDATE,SD_FORCE_UPDATE_VALUE);        
    }

    return OSAL_OK;
}

bool hi_TFUP_getTFUpdateSource(void)
{
    UINT32 ucTmp = 0;

    ucTmp = *(volatile UINT32 *)(TCM_FOR_SD_FORCE_UPDATE);

    return (SD_FORCE_UPDATE_VALUE == ucTmp);
}

/**********************��������ת��**********************************/

int  imageTypeChange(int* pType)
{
    UINT32 oldType = *pType;
        
    if( ZSP_FILE_ADDR == *pType )
    {
        *pType = IMAGE_ZSP;
    }
    else if( ISO_FILE_ADDR == *pType )
    {
        *pType = IMAGE_CDROMISO;
    }                
    else if( *pType >= IMAGE_PART_TOP )
    {
        if( MOBILE_CONNECT_HD_ADDR == *pType )
        {
            *pType = IMAGE_CDROMISOVER;
        }
        else if( MOBILE_CONNECT_ADDR == *pType )
        {
            *pType = IMAGE_CDROMISO;
        }
        else if( WEBUI_HD_ADDR == *pType )
        {
            *pType = IMAGE_WEBUIVER;
        }
        else if( WEBUI_ADDR == *pType )
        {
            *pType = IMAGE_WEBUI;
        }
        else if( IMAGE_MCORE_ADDR == *pType )
        {
            *pType = IMAGE_MCORE;
        }
        else
        {
            printk(KERN_INFO "imageTypeChange: image_type=0x%x error.\n", (int)(*pType));
            return OSAL_ERROR; 
        }                             
    }
    else
    {
        printk(KERN_DEBUG "imageTypeChange: image_type=0x%x .\n", (int)(*pType));
        return OSAL_OK;    
    }

    printk(KERN_DEBUG "imageTypeChange: old_type=0x%x new_type=0x%x.\n", oldType,(int)(*pType));
 
    return OSAL_OK;
}

/***************************************************************
*��������:hi_TFUP_LogInit
*
*��������:log��־�ļ���ʼ��
*
*�������:��
*
*������: ��
*
*����ֵ:��
****************************************************************/
void  hi_TFUP_LogInit (void)
{
    (void)memset(&TFUP_FnFlash[0], 0, sizeof(TFUP_FnFlash));
 
    strncat(TFUP_FnSD, LOGFILENAME, (sizeof(TFUP_FnSD)-1));  

    /*strncat(&TFUP_FnFlash[0], FLASHLOGFILENAME, (sizeof(TFUP_FnFlash)-1));*/
}

/***************************************************************
*��������:TFUP_ReadDataFromTF
*
*��������:��ȡTF���е��ļ�����
*
*�������:�ļ�·��,������ָ��,ƫ����,������,
*
*������: desAddr-��ȡ�ļ�����
*
*����ֵ:OSAL_ERROR-����ִ�д��󷵻�
*                  OSAL_OK-�����ɹ�����
*
****************************************************************/
int hi_TFUP_ReadDataFromTF(char *filePath,UINT8*desAddr,UINT32 offset,int count)
{
    char chr[256];
    int  fd = 0;
    mm_segment_t sd_fs;
    /*�ж��ļ��Ƿ����*/
    sd_fs = get_fs();
    set_fs(KERNEL_DS);
    if((fd = sys_open(filePath, O_RDONLY, S_IRUSR))<0)
    {
        set_fs(sd_fs);
        /*for pclint begin,ע��linux��windows��snprintf�����𣬱�����ֲ��ͳһ��sizeͳһΪ(sizeof(chr)-1)*/
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100);
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR]TFUP_ReadDataFromTF:��[%s]�ļ�ʧ��!\n",jiffies%100,filePath);
        /*for pclint end*/
        hi_TFUP_LogPrintf(chr);

        printk( KERN_INFO "TFUP_ReadDataFromTF:Open file error!\n");
        return OSAL_ERROR;
    }

    
    if( OSAL_ERROR == sys_lseek(fd, (int)offset, SEEK_SET) )
    {
        sys_close(fd);
        set_fs(sd_fs);
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR]TFUP_ReadDataFromTF:�ƶ�[%s]�ļ�ָ��ʧ��!\n",jiffies%100,filePath);
        hi_TFUP_LogPrintf(chr);

        printk(KERN_INFO "TFUP_ReadDataFromTF: fseek error!\n");
        return OSAL_ERROR;
    }

      /*��ȡ�ļ���ָ��λ��ָ����С������*/
    if (0 > sys_read(fd, (char*)desAddr, (UINT32)count))
    {
        sys_close(fd);
        set_fs(sd_fs);
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR]TFUP_ReadDataFromTF:��ȡ[%s]�ļ���Сʧ��!\n",jiffies%100,filePath);
        hi_TFUP_LogPrintf(chr);

        printk(KERN_INFO  "TFUP_ReadDataFromTF:fread fail!\n");
        return OSAL_ERROR;
    }

     /*�ر��ļ�*/
    if (OSAL_OK!= sys_close(fd))
    {
        set_fs(sd_fs);
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR]TFUP_ReadDataFromTF:�ر�[%s]�ļ�ʧ��!\n",jiffies%100,filePath);
        hi_TFUP_LogPrintf(chr);

        printk(KERN_INFO "TFUP_ReadDataFromTF:fclose fail!\n");
        return OSAL_ERROR;
    }

    set_fs(sd_fs);
    return OSAL_OK;

}

/******************************************************************************
*  Function:  getFilePath
*  Description:
*      Unite dirname and filename 
*
*  Calls:
*       
*
*  Called By:
*      TFUP_GetAllFileName
*
*  Data Accessed: 
*
*  Data Updated: 
*
*  Input:
*         dirName: dir name
*         fileName:file name 
*         
*
*  Output:
*         result:save filePath
*          
*
*  Return:
*         OSAL_OK: the function successful returned.
*         OSAL_ERROR: the function encounter OSAL_ERROR while running.
*
********************************************************************************/
int getFilePath(const char *dirName,char *fileName,char *result)
{
    if((dirName !=NULL)&&(result !=NULL)&&(fileName !=NULL))
    {
        strcat(result,dirName);
        strcat(result,"/");
        strcat(result,fileName);
        return OSAL_OK;
    }
    else
    {
        return OSAL_ERROR;
    }
}


/**********************************************************************
*��������:hi_TFUP_ValidateCRCWithLic
*
*��������:Lic�ļ�����Ϣ��Ԫ����
*
*�������:У���ļ�·��,�����ļ�·��
*
*������:��
*
*����ֵ:OSAL_ERROR-����ִ�д��󷵻�
*                  OSAL_OK-�����ɹ�����
***********************************************************************/
int hi_TFUP_ValidateCRCWithLic(char *licFilePath,char *binFilePath)
{
    int ret,count;
    int  fd = 0;
    UINT8 *pbuffer = NULL;
    UINT16 binCRC;
    UINT16 licCRC;
    mm_segment_t  sd_fs;
    char chr[256];
    
    if(NULL == (pbuffer = (UINT8*)kmalloc(LICCRCDATANUM, GFP_KERNEL)))
    {
        printk(KERN_INFO "TFUP_CheckFileCRC:pbuffer malloc error!\n");
        return OSAL_ERROR;
    }

    /*�ж��ļ��Ƿ����*/
    sd_fs = get_fs();
    set_fs(KERNEL_DS);
    if( (fd = sys_open(binFilePath, O_RDONLY, 0600))<0)
    {
        kfree(pbuffer);
        pbuffer = NULL;
        set_fs(sd_fs);
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_ValidateCRCWithLic:��[%s]�ļ�ʧ��!\n",jiffies%100,binFilePath);
        hi_TFUP_LogPrintf(chr);

        printk(KERN_INFO "TFUP_ValidateCRCWithLic:Open file error!\n");
        return OSAL_ERROR;
    }

    /*��ȡ���ļ�����*/
    if( OSAL_ERROR == (ret = sys_lseek(fd, 0, SEEK_END)))
    {
        kfree(pbuffer);
        pbuffer = NULL;
        sys_close(fd);
        set_fs(sd_fs);
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_ValidateCRCWithLic:��ȡ[%s]�ļ�����ʧ��!\n",jiffies%100,binFilePath);
        hi_TFUP_LogPrintf(chr);

        printk(KERN_INFO "TFUP_ValidateCRCWithLic: ftell error!\n");
        return OSAL_ERROR;
    }
    /*�ļ�С��5M���������أ�Ϊ�˷����ྵ�������*/
    if(ret <= BINFADDRFORCRC)
    {
        kfree(pbuffer);
        pbuffer = NULL;
        sys_close(fd);
        set_fs(sd_fs);
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][OK] TFUP_ValidateCRCWithLic:[%s]�ļ�С��5M����������!\n",jiffies%100,binFilePath);
        hi_TFUP_LogPrintf(chr);

        printk(KERN_INFO "TFUP_ValidateCRCWithLic:File length is less than 5M!\n");
        return OSAL_OK;
    }
    /*��ȡbin�е�����*/
    /*����SD����ȡ����ʱ����ǧ��֮����ȡ����ĸ��ʣ���CRCУ��ʧ�ܣ������ض����εľ������*/
    for(count=0; count<3; count++)
    {
        if(OSAL_ERROR == hi_TFUP_ReadDataFromTF(binFilePath,pbuffer,BINFADDRFORCRC,LICCRCDATANUM))
        {
            kfree(pbuffer);
            pbuffer = NULL;
            sys_close(fd);
            set_fs(sd_fs);
            snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
            hi_TFUP_LogPrintf(chr);
            snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_ValidateCRCWithLic:��ȡ[%s]�ļ�������ʧ��!\n",jiffies%100,binFilePath);
            hi_TFUP_LogPrintf(chr);
            
            printk(KERN_INFO "TFUP_ValidateCRCWithLic:TFUP_ReadDataFromTF ERROR!\n" );
            return OSAL_ERROR;
        }
            
        /*����bin��CRC*/
        printk(KERN_INFO "Begin CRC Fun!\n");  /*longwei tiaoshi*/
        CalculateCRC(pbuffer,LICCRCDATANUM,&binCRC);

        printk(KERN_INFO "TFUP_ValidateCRCWithLic:TFUP_ReadDataFromTF binCRC = 0x%d!\n",binCRC);/*longwei tiaoshi*/
        /*��ȡLIC�е�CRC*/
        if(OSAL_ERROR == hi_TFUP_ReadDataFromTF(licFilePath,(UINT8 *)&licCRC,32,2))
        {
            kfree(pbuffer);
            pbuffer = NULL;
            sys_close(fd);
            set_fs(sd_fs);
            snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100);
            hi_TFUP_LogPrintf(chr);
            snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_ValidateCRCWithLic:��ȡ[%s]�ļ��е�CRCʧ��!\n",jiffies%100,binFilePath);
            hi_TFUP_LogPrintf(chr);
     
            printk(KERN_INFO "TFUP_ValidateCRCWithLic:TFUP_ReadDataFromTF ERROR!\n");
            return OSAL_ERROR;
        }

        printk(KERN_INFO "TFUP_ValidateCRCWithLic:TFUP_ReadDataFromTF licCRC = 0x%d!\n",licCRC);/*longwei tiaoshi*/
        /*CRCУ��*/
        if (0 != memcmp(&binCRC,&licCRC,2))
        {
            if(2 == count)
            {
                kfree(pbuffer);
                pbuffer = NULL;
                sys_close(fd);
                set_fs(sd_fs);
                snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100);
                hi_TFUP_LogPrintf(chr);
                snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_ValidateCRCWithLic:[%s]�ļ���[%s]�ļ���CRCУ��ʧ��!\n",jiffies%100,binFilePath,licFilePath);
                hi_TFUP_LogPrintf(chr);
                
                printk(KERN_INFO "TFUP_ValidateCRCWithLic:LIC CRC CHECKOUT IS ERROR!\n");
                return OSAL_ERROR;
            }
            continue;
        }

        kfree(pbuffer);
        pbuffer = NULL;
        sys_close(fd);
        set_fs(sd_fs);
        return OSAL_OK;
    }
}

/**********************************************************************
*��������:hi_TFUP_CheckVersionMatch
*
*��������:�����汾ƥ��
*
*�������:У���ļ�·��
*
*������:��
*
*����ֵ:OSAL_ERROR-����ִ�д��󷵻�
*                  OSAL_OK-�����ɹ�����
***********************************************************************/
int hi_TFUP_CheckVersionMatch(char *licFilePath)
{
    char  chr[256];
    UINT8   bufferForLic[32]={0};
    INT8    PID[31] = {0};/*ȡ��LIC�Ĳ�Ʒ��*/
    INT8    swInfo[31] = {0};
    UINT32  verSoftNum = 0;/*��ǰ�汾����汾���ַ�����*/
    UINT32  swInfoNum =0;/*LIC����汾���ַ�����*/
    int i = 0;
    int j = 0;


    /*����TFUP_ReadDataFromTF���lic�еİ汾������Ϣ�����浽pbufferForLic��*/
    if(OSAL_ERROR == hi_TFUP_ReadDataFromTF(licFilePath,bufferForLic,0,32))
    {
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_CheckVersionMatch:��ȡ[%s]�еİ汾��Ϣʧ��!\n",jiffies%100,licFilePath);
        hi_TFUP_LogPrintf(chr);
       
        printk(KERN_INFO "TFUP_CheckVersionMatch:TFUP_ReadDataFromTF ERROR!\n");
        return OSAL_ERROR;
    }
    printk(KERN_INFO "TFUP_CheckVersionMatch: licFilePath =%s,bufferForLic = %s\n",licFilePath,bufferForLic);/*tiaoshi*/
    
    /*��pbufferForLic���ҵ�LIC�еĲ�Ʒ��*/
    for (i = 0; i<32; i++)
    {
        if (bufferForLic[i] == ':')
        {
            break;
        }       
    }
   
    for (i++, j = 0; i<32; i++, j++)
    {
        if (bufferForLic[i] != ':')
        {
            PID[j] = (INT8)bufferForLic[i];
        }
        else
        {
            break;
        }
    }

    for (i++,j = 0; i<32; i++, j++)
    {
        if (bufferForLic[i] != '\0')
        {
            swInfo[j] = (INT8)bufferForLic[i];
        }
        else
        {
            break;
        }
    }

    /*��������뵱ǰ�汾��Ʒ�űȽ�*/
    if(0 != strcmp((char*)PID, (char*)g_dloadID))
    {
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_CheckVersionMatch:��Ʒ�Ų�ͬ���޷�����!\n",jiffies%100);
        hi_TFUP_LogPrintf(chr);

        printk(KERN_INFO "TFUP_CheckVersionMatch: PID mismatch.\n");
        return OSAL_ERROR;
    }
    /*�����ļ��͵�ǰ�汾������汾�űȽ�*/
    verSoftNum = strlen(g_verSoft);
    swInfoNum = strlen(swInfo);
    printk(KERN_INFO "verSoftNum:%d, swInfoNum:%d", verSoftNum, swInfoNum);/*longwei tiaoshi*/
    /*sunyongbo*/
    /*����汾��û��"."���2����ʼ�Ƚ�*/
    while(g_verSoft[verSoftNum--]!='.')
    {
        if (0 == verSoftNum)
        {
            break;
        }
    }
    printk(KERN_INFO "verSoftNum = %d!\n",verSoftNum);/*tiaoshi*/
            
    while(swInfo[swInfoNum--]!= '.')
    {
        if (0 == swInfoNum)
        {
            break;
        }
    }

    if (((verSoftNum+2) > strlen(g_verSoft))||((swInfoNum+2) > strlen(swInfo)))
    {
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100);
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_CheckVersionMatch:�汾�Ŵ���!\n",jiffies%100);
        hi_TFUP_LogPrintf(chr);
        
        printk(KERN_INFO "TFUP_CheckVersionMatch: format is error!\n");
        return OSAL_ERROR;
    }

    if((0!=strcmp(&g_verSoft[verSoftNum+2],&swInfo[swInfoNum+2]))&&(0!=strcmp(&g_verSoft[verSoftNum+2],MAINVERSION)))
    {
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_CheckVersionMatch:�����㵥������Լ���������޷�����!\n",jiffies%100);
        hi_TFUP_LogPrintf(chr);
       
        printk(KERN_INFO "TFUP_CheckVersionMatch: versionsoft mismatch.\n");
        return OSAL_ERROR;
    }
    else
    {
        printk(KERN_INFO "TFUP_CheckVersionMatch: match is OK!\n");
        return OSAL_OK;
    }
}


/**********************************************************************
*��������:hi_TFUP_GetFileType
*
*��������:��ȡ�ļ�����
*
*�������:�ļ�·��
*
*������:��
*
*����ֵ:�ļ�����
***********************************************************************/
FILETYPE  hi_TFUP_GetFileType(char *pfilePath)
{
    char *pextName;
    int fileNameLen;

    char logFileName[48] = {0};
    strncat(logFileName, READ_PATH, (sizeof(logFileName)-1));
    strncat(logFileName, LOGFILENAME,(sizeof(logFileName)-1)); 
    
    fileNameLen = (int)strlen(pfilePath);   
    if (fileNameLen <= EXTNAMELEN)
    {
        return ELSE;
    }
    pextName = pfilePath +(fileNameLen- EXTNAMELEN); /*����.bin����ʼ��ַ*/
    if ((0 == strcmp(pextName,UPFILEEXTNAME)) \
        || (0 == strcmp(pextName,UPFILEEXTNAME_LOWCASE)))
    {
        return (BIN);

    }
    else if (0 ==(strcmp(pextName,VEFILEEXTNAME)))
    {
        return(LIC);
    }
    else if (0 ==(strcmp(pfilePath,logFileName)))
    {
        return(LOG);
    }
    else
    {
        return(ELSE);
    }
}

/**********************************************************************
*��������:hi_TFUP_IdentifyFile
*
*��������:��������ļ���У���ļ��ĸ�����ƥ����������õ����־λ
*
*�������:�ļ��ܸ���
*
*������:��
*
*����ֵ:OSAL_ERROR-����ִ�д��󷵻�
*                  OSAL_OK-�����ɹ�����
***********************************************************************/
int hi_TFUP_IdentifyFile(int fileNum)
{
    int     i = 0;
    int     j = 0;
    int     binNum = 0;
    int     licNum = 0;
    int     logNum = 0;
    UINT32  dataType = 0;    /*��Ԫ��������*/
    UINT32  fSoftNum = 0;    /*ǰ̨�������*/
    UINT32  dSoftNum = 0;    /*��̨�������*/
    FILETYPE fileType;       /*�����ļ�����*/
    char *pUpdateFileTmp[FILENAME_NUM_MAX]={NULL};  /*���bin�ļ���ַ����ʱ����*/
    char *pVerifyFileTmp[FILENAME_NUM_MAX]={NULL};  /*���lic�ļ���ַ����ʱ����*/
    char chr[256];
    for(i=0;i<fileNum;i++)
    {
        printk(KERN_INFO "fileNameList:%s\n", fileNameList[i]); /*longwei  tiaoshi*/
        /*��ø��ļ�������*/    
        fileType = hi_TFUP_GetFileType(fileNameList[i]);
        switch(fileType)
        {
        case BIN:
            pUpdateFileTmp[binNum++] = fileNameList[i];
            break;
        case LIC:
            pVerifyFileTmp[licNum++] = fileNameList[i];
            break;
        case LOG:
            logNum++;
            break;
        case ELSE:
        default:
            break;
        }
    }
    /*���ݵõ����ļ��������������ж�*/
    if((5 == fileNum)&&(0 == logNum))
    {
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR]TFUP_IdentifyFile:�ļ�����Ϊ5��������log!\n",jiffies%100);
        hi_TFUP_LogPrintf(chr);
        
        printk(KERN_ERR "TFUP_IdentifyFile:FILE number is 5 and log is 0!\n");
        return OSAL_ERROR;
    }
    /*�����ļ���������У���ļ������������ļ���������2�������ļ�����Ϊ0,����*/
    if((binNum>licNum)||(binNum>2)||(0 == binNum))
    {
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_IdentifyFile:�����ļ���������������ļ���������У���ļ�����!\n",jiffies%100);
        hi_TFUP_LogPrintf(chr);
        
        printk(KERN_ERR "TFUP_IdentifyFile:FILE number is error!\n");
        return OSAL_ERROR;
    }

    /*�����ļ���У���ļ��ļ���У��,������ָ����λ��*/
    for(i=0;i<binNum;i++)
    {
        for(j=0;j<licNum;j++)
        {
            if(strlen(pUpdateFileTmp[i]) == strlen(pVerifyFileTmp[j]))
            {
                if (0 == strncmp(pUpdateFileTmp[i],pVerifyFileTmp[j],(strlen(pUpdateFileTmp[i])-4)) )
                {
                    updateFile[i].pFileName = pUpdateFileTmp[i];
                    verifyFile[i].pFileName = pVerifyFileTmp[j];
                    break;
                }
            }
            if(j == (licNum -1))
            {
                if(NULL == updateFile[i].pFileName)
                {
                    snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
                    hi_TFUP_LogPrintf(chr);
                    snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_IdentifyFile:�����ļ���У���ļ����Ʋ�ƥ��!\n",jiffies%100);
                    hi_TFUP_LogPrintf(chr);
                    
                    printk(KERN_ERR "TFUP_IdentifyFile:bin and lic don't match!\n");
                    return OSAL_ERROR;
                }
            }
        }
    }
     /*�����ļ������жϲ����������ļ�����*/
    for(i=0;i<binNum;i++)
    {
        if(OSAL_ERROR == hi_TFUP_ReadDataFromTF(updateFile[i].pFileName,(UINT8 *)&dataType,UNITHEAD_TYPE_OFF,4))
        {
            snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
            hi_TFUP_LogPrintf(chr);
            snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_IdentifyFile:��ȡ�����ļ�����ʧ��!\n",jiffies%100);
            hi_TFUP_LogPrintf(chr);
            
            return OSAL_ERROR;
        }
        
        if(OSAL_ERROR == imageTypeChange((int*)&dataType))  
        {
            snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
            hi_TFUP_LogPrintf(chr);
            snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_IdentifyFile:�����ļ����ʹ��󣬴���26!\n",jiffies%100);
            hi_TFUP_LogPrintf(chr);
          
            return OSAL_ERROR;
        }       
       
        if ((CDROMISOVER == dataType)||(CDROMISO == dataType))
        {
            dSoftNum++;
        }
        else
        {
            updateFile[i].upFileType = FIRMWARESOFT;
            fSoftNum++; 
        }
        
    }
    printk(KERN_INFO "dSoftNum:%d,fSoftNum:%d\n", dSoftNum, fSoftNum); /*longwei  tiaoshi*/
    /*�������̨����������2*/
    if (dSoftNum >= 2||fSoftNum >=2)
    {
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_IdentifyFile:��̨�򵥰��������1!\n",jiffies%100);
        hi_TFUP_LogPrintf(chr);
        
        printk(KERN_ERR "TFUP_IdentifyFile:DASHBOARD OR FIRMWARE is over 1!\n");
        return OSAL_ERROR;
    }
    /*��λ�������������־������NV���ݱ�־λ����λ*/
    if(fSoftNum > 0)
    {
		hi_TFUP_setTFDashFlag(false);
    }
	else
	{
		hi_TFUP_setTFDashFlag(true);
	}

    /*�������ļ�ȫ�ֱ�����ֵ*/
   g_updateFileNum = binNum;
   return OSAL_OK;
}
	

/**********************************************************************
*��������:hi_TFUP_GetAllFileName
*
*��������:��ȡָ���ļ�Ŀ¼�������ļ�����
*
*�������:Ŀ¼·��,�ļ�����
*
*������:��
*
*����ֵ:OSAL_ERROR-����ִ�д��󷵻�
*                  OSAL_OK-�����ɹ�����
***********************************************************************/
int  hi_TFUP_GetAllFileName(char *pDirName,int *pFileNumber)
{
    int fd,i, nCount;
    int nRead = 0;
    char *buf = NULL;	
    mm_segment_t sd_fs;
    struct linux_dirent *dir;
    nCount = 0;
    char chr[256];

    buf = kmalloc(BUFF_SIZE, GFP_KERNEL);
    if(NULL == buf)
    {
        printk(KERN_ERR "Alloc mem error!");
        return OSAL_ERROR;
    }
    sd_fs = get_fs();
    set_fs(KERNEL_DS);
    if((fd = sys_open(pDirName, O_RDONLY | O_DIRECTORY, 0))>=0)
    {
        nRead = sys_getdents(fd, (struct linux_dirent *)buf, BUFF_SIZE);
        if(-1 == nRead)
        {
            printk(KERN_ERR "dents error!\n");
            sys_close(fd);
            set_fs(sd_fs);
            kfree(buf);  
            buf = NULL;
            return OSAL_ERROR;
        }
        if(0 == nRead)
        {
            printk(KERN_ERR "dents zero!\n");
            sys_close(fd);
            set_fs(sd_fs);
            kfree(buf);  
            buf = NULL;
            return OSAL_ERROR;
        }


        /*��ѯ�ļ��н������ļ���������ȫ�ֱ���*/
        for(i=0; i<nRead; )
        {
            dir = (struct linux_dirent *)(buf + i);
            i += dir->d_reclen;

            if (nCount>=FILENAME_NUM_MAX)
            {
                sys_close(fd);
                set_fs(sd_fs);
                snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
                hi_TFUP_LogPrintf(chr);
                snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] hi_TFUP_GetAllFileName:ָ��Ŀ¼���ļ���������5!\n",jiffies%100);
                hi_TFUP_LogPrintf(chr); 

                kfree(buf);  
                buf = NULL;    
                printk(KERN_ERR "TFUP_GetAllFileName:File number is over 5");
                return OSAL_ERROR;
            }
            if((0 == strcmp ((char *) dir->d_name, ".")) ||(0 == strcmp ((char *) dir->d_name, "..")))
            {
                continue;
            }

            /*�����������Ŀ¼���ļ���*/    
            printk("file name: %s!\n", (char *) dir->d_name);   
            if (OSAL_ERROR == getFilePath(pDirName,(char *) dir->d_name,fileNameList[nCount]))
            {
                sys_close(fd);
                set_fs(sd_fs);
                snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
                hi_TFUP_LogPrintf(chr);
                snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] getFilePath:�ϲ��ļ�·������!\n",jiffies%100);
                hi_TFUP_LogPrintf(chr); 

                kfree(buf);  
                buf = NULL; 
                printk(KERN_ERR "TFUP_GetAllFileName:getFilePath is error!\n");
                return OSAL_ERROR;
            }          
            nCount++;
            printk("%s\n",(char *) dir->d_name);
        }

        sys_close(fd);
        set_fs(sd_fs);

        /*�����ļ�����*/
        *pFileNumber = nCount;
        kfree(buf );  
        buf = NULL;
        return OSAL_OK;
    }
    else
    {
        printk(KERN_ERR "Open dir Failure!\n");
        /*�����ļ�����*/
        *pFileNumber = nCount;
        kfree(buf );  
        buf = NULL;
        return OSAL_ERROR;
    }
   
}

/**********************************************************************

*��������:hi_TFUP_CheckFileValidify
*

*��������:����ļ�����Ч��
*
*�������:��

*

*������:��

*

*����ֵ:OSAL_ERROR-����ִ�д��󷵻�

*   OSAL_OK-�����ɹ�����

***********************************************************************/
int hi_TFUP_CheckFileValidify(void)
{
    int i = 0;
    int allFileNum =0;/*dir�������ļ�����*/
    char chr[FILENAME_LEN_MAX];
    unsigned long nv_read = 0;
    struct tagHISDUPDATE_IFC_MSG_STRU  stSDUpdateIFCMsg;  //����NV�������־��ʱ�ṹ��
    char dirName[FILENAME_LEN_MAX] = {0};
  
    strncat(dirName, READ_PATH, (sizeof(dirName)-1));
    strncpy(TFUP_FnSD, dirName, sizeof(TFUP_FnSD));
    strncat(dirName, TFDIRNAME,(sizeof(dirName)-strlen(READ_PATH)-1));  
    (void)memset(fileNameList,0,(FILENAME_LEN_MAX*FILENAME_NUM_MAX));
     
     /*logд��ʼ��*/
    hi_TFUP_LogInit(); 
   
    /*�õ�TF��ָ��Ŀ¼�е������ļ������ܸ���*/
    if(OSAL_OK != hi_TFUP_GetAllFileName(dirName,&allFileNum))
    {
        printk(KERN_ERR "All file name return error!");
        return OSAL_ERROR; 
    }    
    else
    {
        memset(chr,0,sizeof(chr));
        sprintf(chr, "[%03u.%02us][OK] TFUP_GetAllFileName:�õ�ָ��Ŀ¼�ļ���!\n",jiffies/100,jiffies%100);
        hi_TFUP_LogPrintf(chr);       
        printk(KERN_INFO "TFUP_CheckFileValidify:TFUP_GetAllFileName is OK!\n"); 
    }
   
    /*��������ļ���У���ļ��ĸ�����ƥ����������õ����־λ*/ 
    if(OSAL_OK != hi_TFUP_IdentifyFile(allFileNum))
    {
        printk(KERN_ERR "check Identify error!\n");
        return OSAL_ERROR;
    } 
    else 
    {
        printk("All file name return OK\n!");
        memset(chr,0,sizeof(chr));
        sprintf(chr, "[%03u.%02us][OK] TFUP_IdentifyFile:�ļ����������ͼ�ƥ��������ͨ��!\n",jiffies/100,jiffies%100);
        hi_TFUP_LogPrintf(chr);
    }
    
    for(i=0; i<g_updateFileNum; i++)
    {   
        if(FIRMWARESOFT == updateFile[i].upFileType)
        {
            printk(KERN_INFO "Check VersionMatch verifyFile:%s\n!", verifyFile[i].pFileName);
          
             /*���а汾Լ�����*/    
            if(OSAL_ERROR == hi_TFUP_CheckVersionMatch(verifyFile[i].pFileName))         
            {
                printk(KERN_ERR "Check VersionMatch Error!\n");
                return OSAL_ERROR;
            }
            else
            {                    
                printk(KERN_INFO "Check VersionMatch OK!\n");
                memset(chr,0,sizeof(chr));
                sprintf(chr, "[%03u.%02us][OK] TFUP_CheckVersionMatch:�����������汾Լ������!\n",jiffies/100,jiffies%100);
                hi_TFUP_LogPrintf(chr);  
            }     
        }
        
        /*���������ļ���У���ļ���CRCУ��*/
        printk(KERN_INFO "Check VersionMatch updateFile:%s\n!", updateFile[i].pFileName);      
        if(OSAL_OK != hi_TFUP_ValidateCRCWithLic(verifyFile[i].pFileName,updateFile[i].pFileName))   
        {  
            printk("ValidateCRCWithLic  Error!\n");      

            /*����LED���̽���˫�������ļ�����*/
            return OSAL_ERROR;
        }
        else
        {
            printk(KERN_INFO "ValidateCRCWithLic  OK!\n");   
            sprintf(chr, "[%03u.%02us][OK] TFUP_ValidateCRCWithLic:[%s]�ļ���CRCУ��ͨ��!\n",jiffies/100,jiffies%100,verifyFile[i].pFileName);
            hi_TFUP_LogPrintf(chr);    
        }
    }
      
    sprintf(chr, "[%03u.%02us][OK] ֪ͨOM������NV�����Լ���������bootrom!\n",jiffies/100,jiffies%100);
    hi_TFUP_LogPrintf(chr); 
       
	(void)hi_TFUP_setTFUpdateFlag(TFUP_RUNNING);

    /*ͨ����ȡNV�ж�����Ƭ�汾�����������߳����汾���¼��ϱ�*/
    if(BSP_OK != DR_NV_Read(NV_ITEM_FACTORY, &nv_read, 1))
    {
        printk(KERN_ERR "DR_NV_Read failure!\n");
        return OSAL_ERROR;
    }

    if (0 == nv_read)   /*��Ƭ�汾*/
    {
        //����OM�ӿ�NV���ݡ��ָ�������ϵͳ
        /*��Ϣ�������ֵ*/	
        stSDUpdateIFCMsg.u32ModuleId = IFC_MODULE_BSP;
        stSDUpdateIFCMsg.u32FuncId = IFCP_FUNC_SD_HOOK;   //��ʱ���
        stSDUpdateIFCMsg.nHookNum = SDMMC_HOOK_UPDATE_NOTIFY;
        printk("hi_TFUP_CheckFileValidify:hi_sdio_send_to_SDDriver start\n");
        printk("u32ModuleId:%d\n",stSDUpdateIFCMsg.u32ModuleId);
        printk("u32FuncId  :%d\n",stSDUpdateIFCMsg.u32FuncId);
        printk("nHookNum   :%d\n",stSDUpdateIFCMsg.nHookNum);       
        if(OSAL_ERROR == hi_sdio_send_to_SDDriver((void*)&stSDUpdateIFCMsg,sizeof(stSDUpdateIFCMsg))) //����Ϣͨ��IFC����C��SD����
        {
            printk(KERN_ERR "NV operation infomation is send failure!\n");
            return  OSAL_ERROR;
        }        
    }
    else    /*�����汾*/
    {                
	    //net_link�ϱ�APP
	    printk("report_sd_state  KEY_UPDATE Start:!\n"); 
        report_sd_state(DEVICE_ID_KEY, KEY_UPDATE, "check over "); //�豸ID,�¼�����,˵��
        printk("report_sd_state  KEY_UPDATE End:!\n");
	}
    
    (void)hi_TFUP_setTFUpdateSource(false);
    return OSAL_OK;
}

 /*****************************************************************************

 * ������  : hi_sdio_send_to_SDDriver
*

* ��������  : SDģ��˼�ͨ��

* ����Ҫ���е�NV�������Ϣͨ��IFC����C��

*
* �������  : 

* �������  : ��

*

* �� �� ֵ  : OSAL_OK:    �����ɹ�

* OSAL_ERROR: ����ʧ��

*


*

*****************************************************************************/
int hi_sdio_send_to_SDDriver(void *pstSDLDO10Arg, int nSize)
{   
    int ret;    
    
    /*ͨ��IFC����Ϣ����*/   
    ret = BSP_IFC_Send((void*)pstSDLDO10Arg,nSize);   
    if(OSAL_OK != ret)
    {
        printk(KERN_ERR "Information of SD update which send to  SD driver in C kernel is  failure!\n");
        return OSAL_ERROR;
    } 

    return OSAL_OK; 
}


#endif

/**********************************************
*��������:�ϱ�NETLINK SD��״̬
*
*�������:dev-�豸����
*                       nValue - �¼�����
*                       chr   -  ����
*����ֵ:OSAL_OK,OSAL_ERROR
*
***********************************************/
int report_sd_state( DEVICE_ID dev, int nvalue, char *chr)
{
    int ret, size;
    DEVICE_EVENT *event;

    size =  sizeof(DEVICE_EVENT);
    event = (DEVICE_EVENT *)kmalloc(size, GFP_ATOMIC);
    if (NULL == event)
        return OSAL_ERROR;

    event->device_id = dev;//your_id �豸ID
    event->event_code = nvalue; //your_code;�¼�����
    event->len = sizeof(event);
    
    ret = device_event_report(event, size);
    if (-1 == ret) 
    {
        printk(KERN_ERR "device_event_init: can't add event\n");
        kfree(event);
        return OSAL_ERROR;
    }
    kfree(event);
    return OSAL_OK;
}

#if (FEATURE_SDUPDATE == FEATURE_ON)
/*********************************************************************
*
*��������:hi_TFUP_CheckTask
*
*��������:У������
*
*�������:��
*
*������:��
*
*����ֵ:��
***********************************************************************/
#define MNT_NODE_MMCBLK "/dev/block/mmcblk%d"
#define MNT_NODE_MMCBLKP1 "/dev/block/mmcblk%dp1"
#define MNT_CNT_MAX 10

void hi_TFUP_CheckTask(void)
{ 
    int err = 1;
    int count = 0;
    char chr[256];
    int mnt_cnt = 0;
    struct tagHISDUPDATE_VER_IFC_MSG_STRU stSDUpdate_VERIFCMsg;  //�汾��Ϣ˫��ͨ�Žṹ��

    mnt_cnt = mmc_get_disk_devidx();
    g_inFatChecking = TRUE;

	printk("hi_TFUP_CheckTask enter ...\n");
	hi_TFUP_setTFUpdateFlag(TFUP_IDLE);
	BSP_PWRCTRL_SD_LowPowerExit();
	
    if(0 == sd_get_status())
    { 
        while(1)
        {
            if (count++ > 50)
            {
                printk(KERN_INFO "******* count = %d: mounted failed!\n",count);
                sprintf(chr, "[%03u.%02us][ERROR]TFUP_CheckTask:SD��mount�ļ�ϵͳʧ��!\n",
                    (unsigned int)jiffies/100,(unsigned int)jiffies%100);
                hi_TFUP_LogPrintf(chr);
                g_inFatChecking = FALSE;
                return ;
            }
            
            snprintf(chr, (sizeof(chr)-1), MNT_NODE_MMCBLK, mnt_cnt);
            err = do_mount(chr,READ_PATH,"vfat",MS_SILENT,NULL);
            if (err)
            {
                printk(KERN_ERR "SD mmcblk%d vfat : error mounting %i\n", mnt_cnt, err); 
                snprintf(chr, (sizeof(chr)-1), MNT_NODE_MMCBLKP1, mnt_cnt);
                if(do_mount(chr,READ_PATH,"vfat",MS_SILENT,NULL))
                {
                    printk(KERN_ERR "SD mmcblk%dp1 vfat: error mounting %i\n", mnt_cnt, err); 
                    mdelay(100);
                    continue;
                }
                else
                {
                    printk(KERN_INFO "SD mmcblk%dp1 vfat: mounted!\n",mnt_cnt);
                    break;
                }
            }
            else
            {
                printk(KERN_INFO "SD vfat: mounted!\n");
                break;
            }
        }
		
		if(count >= 50)
		{
			printk(KERN_ERR"hi_TFUP_CheckTask:mount sd error!\n");
			sprintf(chr,"[%03u.%02us][OK] TFUP_CheckTask:mount sd error!\n",(unsigned int)jiffies/100,(unsigned int)jiffies%100);
			hi_TFUP_LogPrintf(chr);
			g_inFatChecking = FALSE;
			BSP_PWRCTRL_SD_LowPowerEnter();
			return;
		}

        if(OSAL_OK != balong_Ver_IFCInit())
        {
            printk(KERN_ERR "message recieve failure!\n");
			sprintf(chr,"[%03u.%02us][OK] TFUP_CheckTask:message recieve failure!\n",(unsigned int)jiffies/100,(unsigned int)jiffies%100);
			hi_TFUP_LogPrintf(chr);
			g_inFatChecking = FALSE;
			sys_umount(READ_PATH, 0);
			BSP_PWRCTRL_SD_LowPowerEnter();
	        return;
        }
          
		/*˫��ͨ�Ż�ȡ��ǰ����š�����汾��*/ 
        stSDUpdate_VERIFCMsg.u32ModuleId = IFC_MODULE_BSP;
	    stSDUpdate_VERIFCMsg.u32FuncId = IFCP_FUNC_SDUP_INFO_SET;   

        if(OSAL_ERROR == hi_sdio_send_to_SDDriver((void*)&stSDUpdate_VERIFCMsg,sizeof(stSDUpdate_VERIFCMsg))) /*����Ϣͨ��IFC����C��SD����*/
        {
	        printk(KERN_ERR "hi_TFUP_CheckTask:request SDUP_INFO error!\n");
			sprintf(chr,"[%03u.%02us][OK] TFUP_CheckTask:request SDUP_INFO error!\n",(unsigned int)jiffies/100,(unsigned int)jiffies%100);
			hi_TFUP_LogPrintf(chr);			
			g_inFatChecking = FALSE;
			sys_umount(READ_PATH, 0);
			BSP_PWRCTRL_SD_LowPowerEnter();
            return;
        }
		
		BSP_PWRCTRL_SD_LowPowerExit();
        down(&s_sem_getInfo);
        printk("balong_Ver_IFCReceive:g_verSoft=%s,g_dloadID=%s\n",g_verSoft,g_dloadID);
        
        if(OSAL_OK == hi_TFUP_CheckFileValidify())
        {
            sprintf(chr, "%02us][OK] TFUP_CheckTask:�ļ��Ϸ���У��ɹ�!\n",jiffies/100,jiffies%100);
            hi_TFUP_LogPrintf(chr);
            printk(KERN_INFO "TFUP_CheckTask:file check ok!\n");
        }
        else
	    {
	    	hi_TFUP_setTFUpdateFlag(TFUP_IDLE);
			
        	#if (FEATURE_E5 == FEATURE_ON)
            /*��Ϊ������������ʧ��,OLED��ʾUpdate Failed*/
            if (false != hi_TFUP_getTFUpdateSource())
            {
				#if (FEATURE_OLED == FEATURE_ON)
			    oledClearWholeScreen();
                oledStringDisplay(16, 16, (UINT8 *)"Update Failed!");
                #elif (FEATURE_TFT == FEATURE_ON)
				tftClearWholeScreen();
                tftStringDisplay(12, 58, (UINT8 *)"Update Failed!");
                #endif
            }
       		#endif
            (void)hi_TFUP_setTFUpdateSource(false);
            snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100);
            hi_TFUP_LogPrintf(chr);
            snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_CheckTask:�ļ��Ϸ���У��ʧ��!\n",jiffies%100);
            hi_TFUP_LogPrintf(chr);
            printk(KERN_ERR "TFUP_CheckTask:�ļ��Ϸ���У��ʧ��!\n");
	    	sys_umount(READ_PATH, 0);
			g_inFatChecking = FALSE;
			BSP_PWRCTRL_SD_LowPowerEnter();
           
            return;
        }           
    }
    
}
#endif


/******************************
*Ӧ�ýӿں�::��ȡSD��״̬
*
*����ֵ:SD_ATTATCH-1,SD_DEATCH-0
*
*
******************************/
int app_sd_get_state()
{
    if(sd_get_status()>=0)
    {
        return 1;
    }

    return 0;
}

#if (FEATURE_SDUPDATE == FEATURE_ON)
/*****************************************************************************
* �� �� ��  : balong_Ver_IFCReceive
*
* ��������  : SD����ģ��˼�ͨ��
*             ��C�˵İ汾��Ϣͨ��IFC�ش�
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : OSAL_OK:    �����ɹ�
*             OSAL_ERROR: ����ʧ��
*
* ����˵��  : ��
*
*****************************************************************************/
int balong_Ver_IFCReceive(void *pMsgBody,unsigned int u32Len)
{
    HISDUPDATE_VER_REV_STRU *pRev;
	
    (void)memset(g_verSoft, 0, sizeof(g_verSoft));
    (void)memset(g_dloadID, 0, sizeof(g_dloadID));
    //��IFC����A�˵���Ϣ����
	pRev = (HISDUPDATE_VER_REV_STRU *)pMsgBody;
    strncpy(g_verSoft, (char*)pRev->CompVer, sizeof(g_verSoft));
    strncpy((char*)g_dloadID, (char*)pRev->dloadID, sizeof(g_dloadID));
    up(&s_sem_getInfo);
    printk("balong_Ver_IFCReceive:g_verSoft=%s,g_dloadID=%s\n",g_verSoft,g_dloadID);

    return OSAL_OK;
}

/*****************************************************************************
* �� �� ��  : balong_Ver_IFCInit
*
* ��������  : SD����ģ��˼�ͨ�� ���ջ���
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : OSAL_OK:    �����ɹ�
*             OSAL_ERROR: ����ʧ��
*
* ����˵��  : ��
*
*****************************************************************************/
int balong_Ver_IFCInit(void)
{
    int ret = 0;

    ret = BSP_IFCP_RegFunc(IFCP_FUNC_SDUP_INFO_GET,(BSP_IFCP_REG_FUNC)balong_Ver_IFCReceive);
    if(OSAL_OK != ret)
    {
        printk(KERN_ERR "[%03u.",jiffies/100); 
        printk(KERN_ERR "%02us]NVIM: balong_nvim_IFCInit Fail! ret=%d\n",jiffies%100,ret);
        return OSAL_ERROR;
    }
    printk(KERN_INFO "[%03u.",jiffies/100); 
    printk(KERN_INFO "%02us]NVIM: balong_nvim_IFCInit is success!\n",jiffies%100);
    sema_init(&s_sem_getInfo, SEM_EMPTY);
    return OSAL_OK;
}

#endif
