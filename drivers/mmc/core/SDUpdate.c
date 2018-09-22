/*************************************************************************
*   版权所有(C) 2011-2030, 深圳华为技术有限公司.
*
*   文 件 名 :  SDUpdate.c
*
*
*   描    述 :  SD升级A核文件校验
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

signed char  g_verSoft[31] = {0};  /*版本信息*/

unsigned  char  g_dloadID[128];/* 升级dloadid*/  
int g_updateFileNum = 0;/*升级文件个数*/

TFFILE updateFile[UPFILENUM_MAX]={{NULL,OTHERSOFT},{NULL,OTHERSOFT}};/*存储bin文件路径*/

TFFILE verifyFile[VEFILENUM_MAX]={{NULL,OTHERSOFT},{NULL,OTHERSOFT}};/*存储符合要求的lic文件的路径*/

char  fileNameList[FILENAME_NUM_MAX][FILENAME_LEN_MAX]={0}; /*保存取自dir的文件名*/

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
*函数名称:hi_TFUP_LogPrintf
*
*函数功能:保存数据到TF卡
*
*输入参数:数据内容
*
*输出结果: 
*
*返回值:数据长度
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
*         flag: UPDATE STATE 1:设置标志位 0:清除标志位
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
    if (TFUP_RUNNING == eFlag)  /*1置位*/
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

 /***********返回值为1:LED受控,正常升级.返回值为0:LED正常,强制升级*********************/
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

/**************************设置SD升级成功标志，1:成功，0:失败*************/

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
/**************************得到TF卡升级成功标志***********************************/
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

/**********************升级类型转换**********************************/

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
*函数名称:hi_TFUP_LogInit
*
*函数功能:log日志文件初始化
*
*输入参数:无
*
*输出结果: 无
*
*返回值:无
****************************************************************/
void  hi_TFUP_LogInit (void)
{
    (void)memset(&TFUP_FnFlash[0], 0, sizeof(TFUP_FnFlash));
 
    strncat(TFUP_FnSD, LOGFILENAME, (sizeof(TFUP_FnSD)-1));  

    /*strncat(&TFUP_FnFlash[0], FLASHLOGFILENAME, (sizeof(TFUP_FnFlash)-1));*/
}

/***************************************************************
*函数名称:TFUP_ReadDataFromTF
*
*函数功能:读取TF卡中的文件数据
*
*输入参数:文件路径,缓冲区指针,偏移量,数据量,
*
*输出结果: desAddr-获取文件数据
*
*返回值:OSAL_ERROR-函数执行错误返回
*                  OSAL_OK-函数成功返回
*
****************************************************************/
int hi_TFUP_ReadDataFromTF(char *filePath,UINT8*desAddr,UINT32 offset,int count)
{
    char chr[256];
    int  fd = 0;
    mm_segment_t sd_fs;
    /*判断文件是否存在*/
    sd_fs = get_fs();
    set_fs(KERNEL_DS);
    if((fd = sys_open(filePath, O_RDONLY, S_IRUSR))<0)
    {
        set_fs(sd_fs);
        /*for pclint begin,注意linux和windows下snprintf的区别，便于移植或统一，size统一为(sizeof(chr)-1)*/
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100);
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR]TFUP_ReadDataFromTF:打开[%s]文件失败!\n",jiffies%100,filePath);
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
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR]TFUP_ReadDataFromTF:移动[%s]文件指针失败!\n",jiffies%100,filePath);
        hi_TFUP_LogPrintf(chr);

        printk(KERN_INFO "TFUP_ReadDataFromTF: fseek error!\n");
        return OSAL_ERROR;
    }

      /*获取文件中指定位置指定大小的数据*/
    if (0 > sys_read(fd, (char*)desAddr, (UINT32)count))
    {
        sys_close(fd);
        set_fs(sd_fs);
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR]TFUP_ReadDataFromTF:获取[%s]文件大小失败!\n",jiffies%100,filePath);
        hi_TFUP_LogPrintf(chr);

        printk(KERN_INFO  "TFUP_ReadDataFromTF:fread fail!\n");
        return OSAL_ERROR;
    }

     /*关闭文件*/
    if (OSAL_OK!= sys_close(fd))
    {
        set_fs(sd_fs);
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR]TFUP_ReadDataFromTF:关闭[%s]文件失败!\n",jiffies%100,filePath);
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
*函数名称:hi_TFUP_ValidateCRCWithLic
*
*函数功能:Lic文件中信息单元检验
*
*输入参数:校验文件路径,升级文件路径
*
*输出结果:无
*
*返回值:OSAL_ERROR-函数执行错误返回
*                  OSAL_OK-函数成功返回
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

    /*判断文件是否存在*/
    sd_fs = get_fs();
    set_fs(KERNEL_DS);
    if( (fd = sys_open(binFilePath, O_RDONLY, 0600))<0)
    {
        kfree(pbuffer);
        pbuffer = NULL;
        set_fs(sd_fs);
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_ValidateCRCWithLic:打开[%s]文件失败!\n",jiffies%100,binFilePath);
        hi_TFUP_LogPrintf(chr);

        printk(KERN_INFO "TFUP_ValidateCRCWithLic:Open file error!\n");
        return OSAL_ERROR;
    }

    /*获取该文件长度*/
    if( OSAL_ERROR == (ret = sys_lseek(fd, 0, SEEK_END)))
    {
        kfree(pbuffer);
        pbuffer = NULL;
        sys_close(fd);
        set_fs(sd_fs);
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_ValidateCRCWithLic:获取[%s]文件长度失败!\n",jiffies%100,binFilePath);
        hi_TFUP_LogPrintf(chr);

        printk(KERN_INFO "TFUP_ValidateCRCWithLic: ftell error!\n");
        return OSAL_ERROR;
    }
    /*文件小于5M则正常返回，为了非联编镜像的升级*/
    if(ret <= BINFADDRFORCRC)
    {
        kfree(pbuffer);
        pbuffer = NULL;
        sys_close(fd);
        set_fs(sd_fs);
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][OK] TFUP_ValidateCRCWithLic:[%s]文件小于5M，正常返回!\n",jiffies%100,binFilePath);
        hi_TFUP_LogPrintf(chr);

        printk(KERN_INFO "TFUP_ValidateCRCWithLic:File length is less than 5M!\n");
        return OSAL_OK;
    }
    /*读取bin中的数据*/
    /*由于SD卡读取数据时存在千分之几读取错误的概率，若CRC校验失败，采用重读三次的纠错机制*/
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
            snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_ValidateCRCWithLic:读取[%s]文件中数据失败!\n",jiffies%100,binFilePath);
            hi_TFUP_LogPrintf(chr);
            
            printk(KERN_INFO "TFUP_ValidateCRCWithLic:TFUP_ReadDataFromTF ERROR!\n" );
            return OSAL_ERROR;
        }
            
        /*计算bin的CRC*/
        printk(KERN_INFO "Begin CRC Fun!\n");  /*longwei tiaoshi*/
        CalculateCRC(pbuffer,LICCRCDATANUM,&binCRC);

        printk(KERN_INFO "TFUP_ValidateCRCWithLic:TFUP_ReadDataFromTF binCRC = 0x%d!\n",binCRC);/*longwei tiaoshi*/
        /*读取LIC中的CRC*/
        if(OSAL_ERROR == hi_TFUP_ReadDataFromTF(licFilePath,(UINT8 *)&licCRC,32,2))
        {
            kfree(pbuffer);
            pbuffer = NULL;
            sys_close(fd);
            set_fs(sd_fs);
            snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100);
            hi_TFUP_LogPrintf(chr);
            snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_ValidateCRCWithLic:读取[%s]文件中的CRC失败!\n",jiffies%100,binFilePath);
            hi_TFUP_LogPrintf(chr);
     
            printk(KERN_INFO "TFUP_ValidateCRCWithLic:TFUP_ReadDataFromTF ERROR!\n");
            return OSAL_ERROR;
        }

        printk(KERN_INFO "TFUP_ValidateCRCWithLic:TFUP_ReadDataFromTF licCRC = 0x%d!\n",licCRC);/*longwei tiaoshi*/
        /*CRC校验*/
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
                snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_ValidateCRCWithLic:[%s]文件与[%s]文件的CRC校验失败!\n",jiffies%100,binFilePath,licFilePath);
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
*函数名称:hi_TFUP_CheckVersionMatch
*
*函数功能:升级版本匹配
*
*输入参数:校验文件路径
*
*输出结果:无
*
*返回值:OSAL_ERROR-函数执行错误返回
*                  OSAL_OK-函数成功返回
***********************************************************************/
int hi_TFUP_CheckVersionMatch(char *licFilePath)
{
    char  chr[256];
    UINT8   bufferForLic[32]={0};
    INT8    PID[31] = {0};/*取自LIC的产品号*/
    INT8    swInfo[31] = {0};
    UINT32  verSoftNum = 0;/*当前版本软件版本号字符个数*/
    UINT32  swInfoNum =0;/*LIC软件版本号字符个数*/
    int i = 0;
    int j = 0;


    /*调用TFUP_ReadDataFromTF获得lic中的版本所有信息，保存到pbufferForLic中*/
    if(OSAL_ERROR == hi_TFUP_ReadDataFromTF(licFilePath,bufferForLic,0,32))
    {
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_CheckVersionMatch:获取[%s]中的版本信息失败!\n",jiffies%100,licFilePath);
        hi_TFUP_LogPrintf(chr);
       
        printk(KERN_INFO "TFUP_CheckVersionMatch:TFUP_ReadDataFromTF ERROR!\n");
        return OSAL_ERROR;
    }
    printk(KERN_INFO "TFUP_CheckVersionMatch: licFilePath =%s,bufferForLic = %s\n",licFilePath,bufferForLic);/*tiaoshi*/
    
    /*从pbufferForLic中找到LIC中的产品号*/
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

    /*升级软件与当前版本产品号比较*/
    if(0 != strcmp((char*)PID, (char*)g_dloadID))
    {
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_CheckVersionMatch:产品号不同，无法升级!\n",jiffies%100);
        hi_TFUP_LogPrintf(chr);

        printk(KERN_INFO "TFUP_CheckVersionMatch: PID mismatch.\n");
        return OSAL_ERROR;
    }
    /*升级文件和当前版本的软件版本号比较*/
    verSoftNum = strlen(g_verSoft);
    swInfoNum = strlen(swInfo);
    printk(KERN_INFO "verSoftNum:%d, swInfoNum:%d", verSoftNum, swInfoNum);/*longwei tiaoshi*/
    /*sunyongbo*/
    /*如果版本号没有"."则从2处开始比较*/
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
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_CheckVersionMatch:版本号错误!\n",jiffies%100);
        hi_TFUP_LogPrintf(chr);
        
        printk(KERN_INFO "TFUP_CheckVersionMatch: format is error!\n");
        return OSAL_ERROR;
    }

    if((0!=strcmp(&g_verSoft[verSoftNum+2],&swInfo[swInfoNum+2]))&&(0!=strcmp(&g_verSoft[verSoftNum+2],MAINVERSION)))
    {
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_CheckVersionMatch:不满足单板升级约束条件，无法升级!\n",jiffies%100);
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
*函数名称:hi_TFUP_GetFileType
*
*函数功能:获取文件类型
*
*输入参数:文件路径
*
*输出结果:无
*
*返回值:文件类型
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
    pextName = pfilePath +(fileNameLen- EXTNAMELEN); /*计算.bin的起始地址*/
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
*函数名称:hi_TFUP_IdentifyFile
*
*函数功能:检查升级文件和校验文件的个数及匹配情况并设置单板标志位
*
*输入参数:文件总个数
*
*输出结果:无
*
*返回值:OSAL_ERROR-函数执行错误返回
*                  OSAL_OK-函数成功返回
***********************************************************************/
int hi_TFUP_IdentifyFile(int fileNum)
{
    int     i = 0;
    int     j = 0;
    int     binNum = 0;
    int     licNum = 0;
    int     logNum = 0;
    UINT32  dataType = 0;    /*单元数据类型*/
    UINT32  fSoftNum = 0;    /*前台软件个数*/
    UINT32  dSoftNum = 0;    /*后台软件个数*/
    FILETYPE fileType;       /*定义文件类型*/
    char *pUpdateFileTmp[FILENAME_NUM_MAX]={NULL};  /*存放bin文件地址的临时数组*/
    char *pVerifyFileTmp[FILENAME_NUM_MAX]={NULL};  /*存放lic文件地址的临时数组*/
    char chr[256];
    for(i=0;i<fileNum;i++)
    {
        printk(KERN_INFO "fileNameList:%s\n", fileNameList[i]); /*longwei  tiaoshi*/
        /*获得各文件的类型*/    
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
    /*根据得到的文件类型数量进行判断*/
    if((5 == fileNum)&&(0 == logNum))
    {
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR]TFUP_IdentifyFile:文件个数为5但不包含log!\n",jiffies%100);
        hi_TFUP_LogPrintf(chr);
        
        printk(KERN_ERR "TFUP_IdentifyFile:FILE number is 5 and log is 0!\n");
        return OSAL_ERROR;
    }
    /*升级文件个数大于校验文件个数或升级文件个数大于2或升级文件个数为0,错误*/
    if((binNum>licNum)||(binNum>2)||(0 == binNum))
    {
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_IdentifyFile:升级文件个数错误或升级文件个数大于校验文件个数!\n",jiffies%100);
        hi_TFUP_LogPrintf(chr);
        
        printk(KERN_ERR "TFUP_IdentifyFile:FILE number is error!\n");
        return OSAL_ERROR;
    }

    /*升级文件与校验文件文件名校验,并放入指定的位置*/
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
                    snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_IdentifyFile:升级文件和校验文件名称不匹配!\n",jiffies%100);
                    hi_TFUP_LogPrintf(chr);
                    
                    printk(KERN_ERR "TFUP_IdentifyFile:bin and lic don't match!\n");
                    return OSAL_ERROR;
                }
            }
        }
    }
     /*升级文件类型判断并保存升级文件类型*/
    for(i=0;i<binNum;i++)
    {
        if(OSAL_ERROR == hi_TFUP_ReadDataFromTF(updateFile[i].pFileName,(UINT8 *)&dataType,UNITHEAD_TYPE_OFF,4))
        {
            snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
            hi_TFUP_LogPrintf(chr);
            snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_IdentifyFile:读取升级文件类型失败!\n",jiffies%100);
            hi_TFUP_LogPrintf(chr);
            
            return OSAL_ERROR;
        }
        
        if(OSAL_ERROR == imageTypeChange((int*)&dataType))  
        {
            snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
            hi_TFUP_LogPrintf(chr);
            snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_IdentifyFile:升级文件类型错误，大于26!\n",jiffies%100);
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
    /*单板与后台个数不大于2*/
    if (dSoftNum >= 2||fSoftNum >=2)
    {
        snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
        hi_TFUP_LogPrintf(chr);
        snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_IdentifyFile:后台或单板个数超过1!\n",jiffies%100);
        hi_TFUP_LogPrintf(chr);
        
        printk(KERN_ERR "TFUP_IdentifyFile:DASHBOARD OR FIRMWARE is over 1!\n");
        return OSAL_ERROR;
    }
    /*置位单板软件升级标志，用于NV备份标志位的置位*/
    if(fSoftNum > 0)
    {
		hi_TFUP_setTFDashFlag(false);
    }
	else
	{
		hi_TFUP_setTFDashFlag(true);
	}

    /*给升级文件全局变量赋值*/
   g_updateFileNum = binNum;
   return OSAL_OK;
}
	

/**********************************************************************
*函数名称:hi_TFUP_GetAllFileName
*
*函数功能:获取指定文件目录下所有文件名称
*
*输入参数:目录路径,文件数量
*
*输出结果:无
*
*返回值:OSAL_ERROR-函数执行错误返回
*                  OSAL_OK-函数成功返回
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


        /*轮询文件夹将所有文件名保存至全局变量*/
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
                snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] hi_TFUP_GetAllFileName:指定目录中文件个数超过5!\n",jiffies%100);
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

            /*调试输出升级目录下文件名*/    
            printk("file name: %s!\n", (char *) dir->d_name);   
            if (OSAL_ERROR == getFilePath(pDirName,(char *) dir->d_name,fileNameList[nCount]))
            {
                sys_close(fd);
                set_fs(sd_fs);
                snprintf(chr, (sizeof(chr)-1), "[%03u.",jiffies/100); 
                hi_TFUP_LogPrintf(chr);
                snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] getFilePath:合并文件路径错误!\n",jiffies%100);
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

        /*返回文件个数*/
        *pFileNumber = nCount;
        kfree(buf );  
        buf = NULL;
        return OSAL_OK;
    }
    else
    {
        printk(KERN_ERR "Open dir Failure!\n");
        /*返回文件个数*/
        *pFileNumber = nCount;
        kfree(buf );  
        buf = NULL;
        return OSAL_ERROR;
    }
   
}

/**********************************************************************

*函数名称:hi_TFUP_CheckFileValidify
*

*函数功能:检查文件的有效性
*
*输入参数:无

*

*输出结果:无

*

*返回值:OSAL_ERROR-函数执行错误返回

*   OSAL_OK-函数成功返回

***********************************************************************/
int hi_TFUP_CheckFileValidify(void)
{
    int i = 0;
    int allFileNum =0;/*dir中所有文件个数*/
    char chr[FILENAME_LEN_MAX];
    unsigned long nv_read = 0;
    struct tagHISDUPDATE_IFC_MSG_STRU  stSDUpdateIFCMsg;  //发送NV项操作标志临时结构体
    char dirName[FILENAME_LEN_MAX] = {0};
  
    strncat(dirName, READ_PATH, (sizeof(dirName)-1));
    strncpy(TFUP_FnSD, dirName, sizeof(TFUP_FnSD));
    strncat(dirName, TFDIRNAME,(sizeof(dirName)-strlen(READ_PATH)-1));  
    (void)memset(fileNameList,0,(FILENAME_LEN_MAX*FILENAME_NUM_MAX));
     
     /*log写初始化*/
    hi_TFUP_LogInit(); 
   
    /*得到TF卡指定目录中的所有文件名及总个数*/
    if(OSAL_OK != hi_TFUP_GetAllFileName(dirName,&allFileNum))
    {
        printk(KERN_ERR "All file name return error!");
        return OSAL_ERROR; 
    }    
    else
    {
        memset(chr,0,sizeof(chr));
        sprintf(chr, "[%03u.%02us][OK] TFUP_GetAllFileName:得到指定目录文件名!\n",jiffies/100,jiffies%100);
        hi_TFUP_LogPrintf(chr);       
        printk(KERN_INFO "TFUP_CheckFileValidify:TFUP_GetAllFileName is OK!\n"); 
    }
   
    /*检查升级文件和校验文件的个数及匹配情况并设置单板标志位*/ 
    if(OSAL_OK != hi_TFUP_IdentifyFile(allFileNum))
    {
        printk(KERN_ERR "check Identify error!\n");
        return OSAL_ERROR;
    } 
    else 
    {
        printk("All file name return OK\n!");
        memset(chr,0,sizeof(chr));
        sprintf(chr, "[%03u.%02us][OK] TFUP_IdentifyFile:文件个数、类型及匹配情况检查通过!\n",jiffies/100,jiffies%100);
        hi_TFUP_LogPrintf(chr);
    }
    
    for(i=0; i<g_updateFileNum; i++)
    {   
        if(FIRMWARESOFT == updateFile[i].upFileType)
        {
            printk(KERN_INFO "Check VersionMatch verifyFile:%s\n!", verifyFile[i].pFileName);
          
             /*进行版本约束检查*/    
            if(OSAL_ERROR == hi_TFUP_CheckVersionMatch(verifyFile[i].pFileName))         
            {
                printk(KERN_ERR "Check VersionMatch Error!\n");
                return OSAL_ERROR;
            }
            else
            {                    
                printk(KERN_INFO "Check VersionMatch OK!\n");
                memset(chr,0,sizeof(chr));
                sprintf(chr, "[%03u.%02us][OK] TFUP_CheckVersionMatch:单板软件满足版本约束条件!\n",jiffies/100,jiffies%100);
                hi_TFUP_LogPrintf(chr);  
            }     
        }
        
        /*进行升级文件与校验文件的CRC校验*/
        printk(KERN_INFO "Check VersionMatch updateFile:%s\n!", updateFile[i].pFileName);      
        if(OSAL_OK != hi_TFUP_ValidateCRCWithLic(verifyFile[i].pFileName,updateFile[i].pFileName))   
        {  
            printk("ValidateCRCWithLic  Error!\n");      

            /*设置LED蓝绿交替双闪，即文件错误*/
            return OSAL_ERROR;
        }
        else
        {
            printk(KERN_INFO "ValidateCRCWithLic  OK!\n");   
            sprintf(chr, "[%03u.%02us][OK] TFUP_ValidateCRCWithLic:[%s]文件的CRC校验通过!\n",jiffies/100,jiffies%100,verifyFile[i].pFileName);
            hi_TFUP_LogPrintf(chr);    
        }
    }
      
    sprintf(chr, "[%03u.%02us][OK] 通知OM，进行NV备份以及重启进入bootrom!\n",jiffies/100,jiffies%100);
    hi_TFUP_LogPrintf(chr); 
       
	(void)hi_TFUP_setTFUpdateFlag(TFUP_RUNNING);

    /*通过读取NV判断是烧片版本的升级，或者出厂版本的事件上报*/
    if(BSP_OK != DR_NV_Read(NV_ITEM_FACTORY, &nv_read, 1))
    {
        printk(KERN_ERR "DR_NV_Read failure!\n");
        return OSAL_ERROR;
    }

    if (0 == nv_read)   /*烧片版本*/
    {
        //调用OM接口NV备份、恢复、重启系统
        /*消息体变量赋值*/	
        stSDUpdateIFCMsg.u32ModuleId = IFC_MODULE_BSP;
        stSDUpdateIFCMsg.u32FuncId = IFCP_FUNC_SD_HOOK;   //暂时替代
        stSDUpdateIFCMsg.nHookNum = SDMMC_HOOK_UPDATE_NOTIFY;
        printk("hi_TFUP_CheckFileValidify:hi_sdio_send_to_SDDriver start\n");
        printk("u32ModuleId:%d\n",stSDUpdateIFCMsg.u32ModuleId);
        printk("u32FuncId  :%d\n",stSDUpdateIFCMsg.u32FuncId);
        printk("nHookNum   :%d\n",stSDUpdateIFCMsg.nHookNum);       
        if(OSAL_ERROR == hi_sdio_send_to_SDDriver((void*)&stSDUpdateIFCMsg,sizeof(stSDUpdateIFCMsg))) //将消息通过IFC传到C核SD驱动
        {
            printk(KERN_ERR "NV operation infomation is send failure!\n");
            return  OSAL_ERROR;
        }        
    }
    else    /*出厂版本*/
    {                
	    //net_link上报APP
	    printk("report_sd_state  KEY_UPDATE Start:!\n"); 
        report_sd_state(DEVICE_ID_KEY, KEY_UPDATE, "check over "); //设备ID,事件代码,说明
        printk("report_sd_state  KEY_UPDATE End:!\n");
	}
    
    (void)hi_TFUP_setTFUpdateSource(false);
    return OSAL_OK;
}

 /*****************************************************************************

 * 函数名  : hi_sdio_send_to_SDDriver
*

* 功能描述  : SD模块核间通信

* 将需要进行的NV项操作信息通过IFC传到C核

*
* 输入参数  : 

* 输出参数  : 无

*

* 返 回 值  : OSAL_OK:    操作成功

* OSAL_ERROR: 操作失败

*


*

*****************************************************************************/
int hi_sdio_send_to_SDDriver(void *pstSDLDO10Arg, int nSize)
{   
    int ret;    
    
    /*通过IFC将消息发出*/   
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
*函数功能:上报NETLINK SD卡状态
*
*输入参数:dev-设备代码
*                       nValue - 事件代码
*                       chr   -  描述
*返回值:OSAL_OK,OSAL_ERROR
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

    event->device_id = dev;//your_id 设备ID
    event->event_code = nvalue; //your_code;事件代码
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
*函数名称:hi_TFUP_CheckTask
*
*函数功能:校验任务
*
*输入参数:无
*
*输出结果:无
*
*返回值:无
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
    struct tagHISDUPDATE_VER_IFC_MSG_STRU stSDUpdate_VERIFCMsg;  //版本信息双核通信结构体

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
                sprintf(chr, "[%03u.%02us][ERROR]TFUP_CheckTask:SD卡mount文件系统失败!\n",
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
          
		/*双核通信获取当前组件号、软件版本号*/ 
        stSDUpdate_VERIFCMsg.u32ModuleId = IFC_MODULE_BSP;
	    stSDUpdate_VERIFCMsg.u32FuncId = IFCP_FUNC_SDUP_INFO_SET;   

        if(OSAL_ERROR == hi_sdio_send_to_SDDriver((void*)&stSDUpdate_VERIFCMsg,sizeof(stSDUpdate_VERIFCMsg))) /*将消息通过IFC传到C核SD驱动*/
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
            sprintf(chr, "%02us][OK] TFUP_CheckTask:文件合法性校验成功!\n",jiffies/100,jiffies%100);
            hi_TFUP_LogPrintf(chr);
            printk(KERN_INFO "TFUP_CheckTask:file check ok!\n");
        }
        else
	    {
	    	hi_TFUP_setTFUpdateFlag(TFUP_IDLE);
			
        	#if (FEATURE_E5 == FEATURE_ON)
            /*若为按键触发升级失败,OLED显示Update Failed*/
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
            snprintf(chr, (sizeof(chr)-1), "%02us][ERROR] TFUP_CheckTask:文件合法性校验失败!\n",jiffies%100);
            hi_TFUP_LogPrintf(chr);
            printk(KERN_ERR "TFUP_CheckTask:文件合法性校验失败!\n");
	    	sys_umount(READ_PATH, 0);
			g_inFatChecking = FALSE;
			BSP_PWRCTRL_SD_LowPowerEnter();
           
            return;
        }           
    }
    
}
#endif


/******************************
*应用接口函::获取SD卡状态
*
*返回值:SD_ATTATCH-1,SD_DEATCH-0
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
* 函 数 名  : balong_Ver_IFCReceive
*
* 功能描述  : SD升级模块核间通信
*             将C核的版本信息通过IFC回传
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : OSAL_OK:    操作成功
*             OSAL_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int balong_Ver_IFCReceive(void *pMsgBody,unsigned int u32Len)
{
    HISDUPDATE_VER_REV_STRU *pRev;
	
    (void)memset(g_verSoft, 0, sizeof(g_verSoft));
    (void)memset(g_dloadID, 0, sizeof(g_dloadID));
    //将IFC传入A核的消息解析
	pRev = (HISDUPDATE_VER_REV_STRU *)pMsgBody;
    strncpy(g_verSoft, (char*)pRev->CompVer, sizeof(g_verSoft));
    strncpy((char*)g_dloadID, (char*)pRev->dloadID, sizeof(g_dloadID));
    up(&s_sem_getInfo);
    printk("balong_Ver_IFCReceive:g_verSoft=%s,g_dloadID=%s\n",g_verSoft,g_dloadID);

    return OSAL_OK;
}

/*****************************************************************************
* 函 数 名  : balong_Ver_IFCInit
*
* 功能描述  : SD升级模块核间通信 接收机制
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : OSAL_OK:    操作成功
*             OSAL_ERROR: 操作失败
*
* 其它说明  : 无
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
