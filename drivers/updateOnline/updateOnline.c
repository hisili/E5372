
//#define WIN32
#ifdef  WIN32
#undef  WIN32
#endif

#if(FEATURE_UPDATEONLINE == FEATURE_ON)

#ifdef  WIN32
#include <stdio.h>
#else
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <net/sock.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/netlink.h>
#include <mach/platform.h>
#include <generated/FeatureConfig.h>
#endif

#include "../mntn/excDrv.h"
#include "BSP.h"

#include "updateOnline.h"


UPDATE_INFOMATION g_update_info = {0};
FileUnitHeader g_FSHeadData[16];        /*保存单元头数据的数组*/

char *g_updateDataPah[COMPONENT_COUNT] = {FIRMWARE1_FILE_PATH_NAME,
                                          FIRMWARE2_FILE_PATH_NAME,
                                          UPDATE_ISO_FILE_PATH_NAME,
                                          WEB_UI_FILE_PATH_NAME,
                                          UPDATE_INFO_FILE_PATH_NAME};


/*************************************************
* 函数名   :update_setWebUIUpdateFlag

* 功能描述 :在DTCM中设置WebUI升级标记位

* 输入参数 :

* 输出参数 :

* 返回值   :

* 调用函数 : 

* 被调函数 :

* 修改历史 :
*************************************************/
int update_setWebUIUpdateFlag(int flag)
{
    if (WEBUI_UPDATE == flag)  /*1置位*/
    {
        writeM_u32(TCM_FOR_WEBUI_STATUS_FLAG_ADDR,TCM_FOR_WEBUI_STATUS_FLAG_VAL);
        return OSAL_OK;
    }
    else if(WEBUI_NOT_UPDATE == flag)
    {
        writeM_u32(TCM_FOR_WEBUI_STATUS_FLAG_ADDR,0);
        return OSAL_OK;
    }
    else
    {
        return OSAL_ERROR;
    }

}

/*************************************************
* 函数名   :update_UpDateInfoGet

* 功能描述 :将update_info.bin read 到 RAM BUFFER

* 输入参数 :pstUpdateInfo: RAM BUFFER 地址

* 输出参数 :

* 返回值   :

* 调用函数 

* 被调函数 :

* 修改历史 :

*************************************************/
int update_UpDateInfoGet(UPDATE_INFOMATION *pstUpdateInfo)
{
    #ifdef WIN32
    FILE* fd = NULL;
    #else
    unsigned int fd = 0;
    #endif
    char logTmpBuf[LOG_BUF_LEN] = {0};
    mm_segment_t old_fs;
    old_fs = get_fs();
    set_fs(KERNEL_DS);

    if(NULL == pstUpdateInfo)
    {
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%lu update_UpDateInfoGet: pstUpdateInfo is NULL \n",jiffies%100);
        update_record_file(logTmpBuf);
        return OSAL_ERROR;
    }
    
    fd = os_file_open(g_updateDataPah[UPDATE_INFO]);
    #ifdef WIN32
    if(NULL == fd)
    {
        return OSAL_ERROR;
    }
    #else
    if(OSAL_ERROR == fd)
    {
		memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d update_UpDateInfoGet: open file error %s \n",jiffies%100,g_updateDataPah[UPDATE_INFO]);
        update_record_file(logTmpBuf);
        set_fs(old_fs);
        return OSAL_ERROR;
    }
    #endif

    if (OSAL_ERROR  == os_file_lseek(fd,0))//指向文件首地址
    {
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d update_UpDateInfoGet: os_file_lseek  error  \n",jiffies%100);
        update_record_file(logTmpBuf);
        set_fs(old_fs);
        os_file_close(fd);
        return OSAL_ERROR;
    }

    if(OSAL_ERROR == os_file_read(fd,(char*)pstUpdateInfo,sizeof(UPDATE_INFOMATION)))
    {
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d update_UpDateInfoGet: os_file_read  error  \n",jiffies%100);
        update_record_file(logTmpBuf);    
        set_fs(old_fs);
        os_file_close(fd);
        return OSAL_ERROR;
    }
    
    #ifdef  WIN32
    printf("m_total_componets %d",pstUpdateInfo->m_total_components);
    #endif

    /*关闭文件*/
    if (OSAL_OK!= os_file_close(fd))
    {
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d update_UpDateInfoGet: os_file_close  error  \n",jiffies%100);
        update_record_file(logTmpBuf);
        set_fs(old_fs);
        return OSAL_ERROR;
    }

    set_fs(old_fs);

    memset((void*)logTmpBuf,LOG_BUF_LEN,0);
    snprintf(logTmpBuf,LOG_BUF_LEN, "%d update_UpDateInfoGet: OK \n",jiffies%100);
    update_record_file(logTmpBuf);
    return OSAL_OK;
}

/*************************************************
* 函数名   :update_UpDateInfoSet

* 功能描述 :将update_info.bin read 到 RAM BUFFER

* 输入参数 :pstUpdateInfo: RAM BUFFER 地址

* 输出参数 :

* 返回值   :

* 调用函数 

* 被调函数 :


*************************************************/
int update_UpDateInfoSet(UPDATE_INFOMATION *pstUpdateInfo)
{
/*在线升级完，NV恢复完成后，M核发送消息到A核，设置在线升级状态有问题，
  需要将打开文件的属性修改，写方式打开*/
    unsigned int fd = 0;
    char logTmpBuf[LOG_BUF_LEN] = {0};
    mm_segment_t old_fs;
    old_fs = get_fs();
    set_fs(KERNEL_DS);

    if(NULL == pstUpdateInfo)
    {
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%lu update_UpDateInfoSet: pstUpdateInfo is NULL \n",jiffies%100);
        update_record_file(logTmpBuf);
        return OSAL_ERROR;
    }
 
    fd = sys_open(g_updateDataPah[UPDATE_INFO],O_RDWR|O_CREAT,0644);
    if(OSAL_ERROR == fd)
    {
		memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d update_UpDateInfoSet: open file error %s \n",jiffies%100,g_updateDataPah[UPDATE_INFO]);
        update_record_file(logTmpBuf);
        set_fs(old_fs);
        return OSAL_ERROR;
    }

    if (OSAL_ERROR  == sys_lseek(fd,(int)0,SEEK_SET))//指向文件首地址
    {
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d update_UpDateInfoGet: os_file_lseek  error  \n",jiffies%100);
        update_record_file(logTmpBuf);
        set_fs(old_fs);
        os_file_close(fd);
        return OSAL_ERROR;
    }

    if(OSAL_ERROR == sys_write(fd,(char*)pstUpdateInfo,sizeof(UPDATE_INFOMATION)))
    {
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d update_UpDateInfoGet: os_file_read  error  \n",jiffies%100);
        update_record_file(logTmpBuf);    
        set_fs(old_fs);
        os_file_close(fd);
        return OSAL_ERROR;
    }
    
  
    /*关闭文件*/
    if (OSAL_OK!= sys_close(fd))
    {
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d update_UpDateInfoGet: os_file_close  error  \n",jiffies%100);
        update_record_file(logTmpBuf);
        set_fs(old_fs);
        return OSAL_ERROR;
    }
    set_fs(old_fs);

    memset((void*)logTmpBuf,LOG_BUF_LEN,0);
    snprintf(logTmpBuf,LOG_BUF_LEN, "%d update_UpDateInfoGet: OK \n",jiffies%100);
    update_record_file(logTmpBuf);
    return OSAL_OK;
}

/**********************************************************************************************
 * FUNCTION:
 * onlineupdate_get_onlineinfo -  
 *
 * 
 * DESCRIPTION:
 *
 * 读取UPDATE_INFO.BIN文件
 * 判断升级标志 是否满足升级条件 
 *
 **********************************************************************************************/
int onlineupdate_get_onlineinfo(void)
{    
    char logTmpBuf[LOG_BUF_LEN] = {0};
    memset(&g_update_info, 0, sizeof(g_update_info));

    /*核对参数*/
    if(OSAL_OK != update_UpDateInfoGet(&g_update_info))
    {
        printk("onlineupdate_get_onlineinfo error \n");
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d : update_UpDateInfoGet error\n",jiffies%100);
        update_record_file(logTmpBuf);
        goto ERROR_EXIT;
    }

    if((g_update_info.m_total_components == 0) || (g_update_info.m_total_components >= COMPONENT_COUNT))
    {
        printk("onlineupdate_get_onlineinfo g_update_info.m_total_components is %d\n",g_update_info.m_total_components);
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d : m_total_components  is %d\n",jiffies%100,g_update_info.m_total_components);
        update_record_file(logTmpBuf);
        goto ERROR_EXIT;
    }

    if(UPDATE_PROGRESSING != g_update_info.m_current_component_status)
    {
        printk("onlineupdate_get_onlineinfo: m_current_component_status is not updateprocessing \n");
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d : error m_current_component_status  is %d  \n",jiffies%100,g_update_info.m_current_component_status);
        printk("%d onlineupdate_get_onlineinfo: error m_current_component_status  is %d  \n",jiffies%100,g_update_info.m_current_component_status);
        update_record_file(logTmpBuf);
        goto ERROR_EXIT;
    }

    printk("m_current_component_status %0x\n",g_update_info.m_current_component_status);
    memset((void*)logTmpBuf,LOG_BUF_LEN,0);
    snprintf(logTmpBuf,LOG_BUF_LEN, "%d onlineupdate_get_onlineinfo: OK \n",jiffies%100);
    update_record_file(logTmpBuf);

    return OSAL_OK;

ERROR_EXIT:
	update_setWebUIUpdateFlag(WEBUI_NOT_UPDATE);  
    return OSAL_ERROR;
}

/*************************************************
* 函数名   :dloadFromFsGetUnitHeadData

* 功能描述 :获得升级镜像中升级的类型【bootload,bootrom,vxworks,zsp,nv,dynfs】

* 输入参数 :升级文件路径

* 输出参数 :升级类型及升级类型个数

* 返回值   :

* 调用函数 :

* 被调函数 :

* 修改历史 :

*************************************************/

int dloadFromFsGetUnitHeadData (char* binFilePath,FileUnitHeader * headData,UINT8 * headNum)
{
    #ifdef WIN32
    FILE *  fp = NULL;
    #else
    unsigned int  fp = 0;
    #endif
    
    int i=0;
    UINT32 unitTotalLen=0;
    UINT32 byteOff = FS_FILE_HEAD_LEN;
    UINT32 imageTotalLen =0;
    UINT8  unitHeadNum =0;   /*记录单元头个数的变量*/
    char logTmpBuf[LOG_BUF_LEN] = {0};

    mm_segment_t old_fs;
    
    if( (NULL == headData) || (NULL == binFilePath) || (NULL == headNum) )
    {
        printk("dloadFromFsGetUnitHeadData:the para is error!\n");
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d :headData %p,binFilePath %p,headNum %p \n",
        jiffies%100,headData,binFilePath,headNum);
        update_record_file(logTmpBuf);
        return OSAL_ERROR;
    }

   old_fs = get_fs();
   set_fs(KERNEL_DS);
   
    fp = os_file_open(binFilePath);
    #ifdef WIN32
         if(NULL == fp)
          {
            return OSAL_ERROR;
          }
    #else
         if(OSAL_ERROR == fp)
          {
            printk("dloadFromFsGetUnitHeadData: %s !\n",binFilePath);
            memset((void*)logTmpBuf,LOG_BUF_LEN,0);
            snprintf(logTmpBuf,LOG_BUF_LEN, "%d :os_file_open error binFilePath  %s\n",
            jiffies%100,binFilePath);
            update_record_file(logTmpBuf);
            set_fs(old_fs);
            return OSAL_ERROR;
          }
    #endif

   
   /*get file size*/
    imageTotalLen  = os_file_size(fp);

    if(OSAL_ERROR == imageTotalLen)
        {
            memset((void*)logTmpBuf,LOG_BUF_LEN,0);
            snprintf(logTmpBuf,LOG_BUF_LEN, "%d dloadFromFsGetUnitHeadData:os_file_size error \n",jiffies%100);
            update_record_file(logTmpBuf);
            set_fs(old_fs);
            os_file_close(fp);
            return OSAL_ERROR;
        }
    
    printk("the imageTotalLen is %d \n", (int)imageTotalLen);

    for(i=0;i<FS_MAX_HEAD_NUM;i++)
    {
        memset(&g_FSHeadData[i], 0, sizeof(FileUnitHeader));
    }

    /*将所有单元头保存到数组FSHeadData中*/
    while (imageTotalLen > byteOff )
    {
        /*将fd指向一个单元头的起始位置*/
          if( OSAL_ERROR == os_file_lseek(fp, (int)byteOff))
            {
                os_file_close(fp);
                set_fs(old_fs); 
                return OSAL_ERROR;
            }

        /*单元头个数不超过16个*/
        if (unitHeadNum >= FS_MAX_HEAD_NUM)
        {
            os_file_close(fp);
            set_fs(old_fs); 
            
            printk("unitHeadNum is %d error!\n",unitHeadNum);
            memset((void*)logTmpBuf,LOG_BUF_LEN,0);
            snprintf(logTmpBuf,LOG_BUF_LEN, "%d :unitHeadNum: %d >16  error \n",jiffies%100,
            unitHeadNum );
            update_record_file(logTmpBuf);
            return OSAL_ERROR;
        }
        
        /*通过文件系统读取单元头数据*/
        if(OSAL_ERROR == os_file_read(fp,(char*)&g_FSHeadData[unitHeadNum],sizeof(FileUnitHeader)))
          {
            memset((void*)logTmpBuf,LOG_BUF_LEN,0);
            snprintf(logTmpBuf,LOG_BUF_LEN, "%d :os_file_read:  error \n",jiffies%100);
            update_record_file(logTmpBuf);
            
            os_file_close(fp);
            set_fs(old_fs); 
            
            printk("os_file_read error!\n");
            return OSAL_ERROR;
          }
       
        /*计算下一个单元头的起始位置*/
        if (UPDATE_UNIT_HEAD_FLAG == g_FSHeadData[unitHeadNum].dwMagicNum)
        {
            unitTotalLen = g_FSHeadData[unitHeadNum].dwHeaderLen + g_FSHeadData[unitHeadNum].dwDataLen;
            unitTotalLen = ((0 == unitTotalLen%4 )?unitTotalLen :((unitTotalLen/4+1)*4)) ;
            byteOff = byteOff + unitTotalLen;//偏移跳过已处理数据
            unitHeadNum++;
        }
        else
        {
           break;
        }
    }

    os_file_close(fp);
    set_fs(old_fs); 

    *headNum = unitHeadNum;
    memcpy(headData, g_FSHeadData, sizeof(FileUnitHeader)*unitHeadNum);
    
    return OSAL_OK;
}
/**********************************************************************
*函数名称:onlineupdate_set_nv_backup_flag
*
*函数功能: 分析前后台情况 确定是否需要备份
*
*输入参数:
*
*输出结果:无
*
*返回值:
***********************************************************************/
int onlineupdate_set_nv_backup_flag(void)
{
    UINT8  HeadNum = 0;
    UINT8  ucImageID = 0; /*单元头个数变量*/
    UINT32  fSoftNum = 0;    /*前台软件个数*/
    UINT32  dSoftNum = 0;    /*后台软件个数*/
    FileUnitHeader* pFileUnitHeaderStr = NULL;
    char logTmpBuf[LOG_BUF_LEN] = {0};
    
    if( NULL == (pFileUnitHeaderStr = (FileUnitHeader*)kmalloc(sizeof(FileUnitHeader) * FS_MAX_HEAD_NUM,GFP_KERNEL)) )
    {    
        printk("onlineupdate_set_nv_backup_flag pFileUnitHeaderStr kmalloc error.\n");
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d onlineupdate_set_nv_backup_flag kmalloc error\n",jiffies%100);
        update_record_file(logTmpBuf);
        return RETURN_ERROR; 
    }
    
    /*从bin文件中获取升级镜像类型*/
    if( OSAL_OK != dloadFromFsGetUnitHeadData(g_updateDataPah[g_update_info.m_component_type], 
                                                pFileUnitHeaderStr, &HeadNum) )
    {        
        printk("onlineupdate_set_nv_backup_flag: dloadFromFsGetUnitHeadData error.\n");
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d  dloadFromFsGetUnitHeadData error\n",jiffies%100);
        update_record_file(logTmpBuf);
        goto ERROR_EXIT;
    }
    else
    {
        printk("onlineupdate_set_nv_backup_flag: dloadFromFsGetUnitHeadData get OK.\n");
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d  dloadFromFsGetUnitHeadData OK\n",jiffies%100);
        update_record_file(logTmpBuf);
    }

    for(ucImageID=0; ucImageID < HeadNum; ucImageID++)
    {
        enum  IMAGE_TYPE    image_type;     
        /*获取当前升级部件的类型*/
        image_type = ((enum IMAGE_TYPE)(pFileUnitHeaderStr + ucImageID)->dwDataType);

        ImageTypeChange((UINT32*)&image_type); 

        if ((IMAGE_CDROMISOVER == image_type)||(IMAGE_CDROMISO == image_type)||\
            (IMAGE_WEBUI == image_type)||(IMAGE_WEBUIVER == image_type))
        {
            dSoftNum++;
        }
        else
        {
            fSoftNum++; 
        }
    } 

    printk("onlineupdate_set_nv_backup_flag: fSoftNum %d,dSoftNum %d \n",fSoftNum,dSoftNum);
    memset((void*)logTmpBuf,LOG_BUF_LEN,0);
    snprintf(logTmpBuf,LOG_BUF_LEN, "%d : fSoftNum %d,dSoftNum %d\n",jiffies%100,fSoftNum,dSoftNum);
    update_record_file(logTmpBuf);

    /*置位单板软件升级标志，用于NV恢复标志位的置位*/
    if(fSoftNum >0)
    {
        (void)update_nv_backup_flag_set(NV_BACKUP);
    }
    else if (dSoftNum >0)
    {
        (void)update_nv_backup_flag_set(NV_NO_BACKUP);
    }

    if(NULL != pFileUnitHeaderStr)
    {
        kfree(pFileUnitHeaderStr);
        pFileUnitHeaderStr = NULL;
    }
    
    return RETURN_OK;

ERROR_EXIT:
    if(NULL != pFileUnitHeaderStr)
    {
        kfree(pFileUnitHeaderStr);
        pFileUnitHeaderStr = NULL;
    }
    
    (void)update_nv_backup_flag_set(NV_NO_BACKUP);

    return RETURN_ERROR;  
}

/**********************************************************************
*函数名称:set_update_reboot
*
*函数功能:
*
*输入参数:
*
*输出结果:无
*
*返回值:
***********************************************************************/
void set_update_reboot(void)
{
	unsigned int ret = 0 ; /*clean lint e713*/
  	char logTmpBuf[LOG_BUF_LEN] = {0};
	struct tagONLLINEUPDATE_VER_IFC_MSG_STRU stOnlineUpdate_IFCMsg;  //双核通信结构体

	stOnlineUpdate_IFCMsg.u32ModuleId = IFC_MODULE_BSP;
	stOnlineUpdate_IFCMsg.u32FuncId = IFCP_FUNC_ONLINE_UP_INFO_SET;   //暂时替代
	stOnlineUpdate_IFCMsg.nHookNum = SDMMC_HOOK_UPDATE_NOTIFY;

	/*通过IFC将消息发出*/   
    ret = BSP_IFC_Send((void*)&stOnlineUpdate_IFCMsg,sizeof(stOnlineUpdate_IFCMsg));
    if(OSAL_OK != ret)
    {
        printk(KERN_ERR " set_update_reboot  failure!\n");
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d set_update_reboot error\n",jiffies%100);
        update_record_file(logTmpBuf);
    }
    else
    {
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d set_update_reboot OK\n Acore Online Update OK",jiffies%100);
        update_record_file(logTmpBuf);
    }

    return ;
}

static int recv_msg(void *data, int len)
{
    char logTmpBuf[LOG_BUF_LEN] = {0};
    DEVICE_EVENT *event = (DEVICE_EVENT *)data;

    memset((void*)logTmpBuf,LOG_BUF_LEN,0);
    snprintf(logTmpBuf,LOG_BUF_LEN, "%d OnlineUpdate:==========Start==========\n",jiffies%100);
    update_record_file(logTmpBuf);

    if (DEVICE_ID_UPDATEONLINE !=event->device_id) 
    {
        printk("device_id is not DEVICE_ID_UPDATEONLINE \n");
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d OnlineUpdate:error device_id is not DEVICE_ID_UPDATEONLINE;device_id:%d \n",\
        jiffies%100,event->device_id);
        update_record_file(logTmpBuf);
        
        return RETURN_ERROR;
    }

    switch(event->event_code)
    {
        case APP_REBOOT_REQUEST:
        {
            memset((void*)logTmpBuf,LOG_BUF_LEN,0);
            snprintf(logTmpBuf,LOG_BUF_LEN, "%d OnlineUpdate: APP_REBOOT_REQUEST OK \n",jiffies%100);
            update_record_file(logTmpBuf);
            
            if(OSAL_OK != onlineupdate_get_onlineinfo())
            {
                memset((void*)logTmpBuf,LOG_BUF_LEN,0);
                snprintf(logTmpBuf,LOG_BUF_LEN, "%d OnlineUpdate: onlineupdate_get_onlineinfo ERROR \n",jiffies%100);
                update_record_file(logTmpBuf);
                printk("onlineupdate_get_onlineinfo error \n");
                return OSAL_ERROR;
            }
            
            if (OSAL_OK != onlineupdate_set_nv_backup_flag())
            {
                memset((void*)logTmpBuf,LOG_BUF_LEN,0);
                snprintf(logTmpBuf,LOG_BUF_LEN, "%d : onlineupdate_set_nv_backup_flag ERROR \n",jiffies%100);
                update_record_file(logTmpBuf);
                printk("onlineupdate_set_nv_backup_flag error \n");
                
                return OSAL_ERROR;
            }
            
            set_update_reboot();
        }break;
		
        default:
        {
            memset((void*)logTmpBuf,LOG_BUF_LEN,0);
            snprintf(logTmpBuf,LOG_BUF_LEN, "%d OnlineUpdate: event_code %d ERROR \n",jiffies%100,event->event_code);
            update_record_file(logTmpBuf);
            printk("event_code error\n");
            return RETURN_ERROR;
        }break;
    }

    return RETURN_OK;
}

void updateOnline_register(void)
{
    device_event_handler_register(DEVICE_ID_UPDATEONLINE,recv_msg);
}

/* just for test */
int updateOnline_sendmsg(void)
{
    DEVICE_EVENT event;

    event.device_id = DEVICE_ID_UPDATEONLINE;
    event.event_code = APP_REBOOT_REQUEST;

    return recv_msg(&event,0);
}

int updateOnline_GetGoOnFlag(void)
{
    return (ONLINE_GOON_MAGIC == ONLINE_GOON_FLAG);
}

 /*****************************************************************************
 * 函 数 名  : Onup_set_Status_func
 *
 * 功能描述  : 在线升级模块核间通信
 *			   设置在线升级状态
 *
 * 输入参数  : BSP_VOID *pMsgBody  消息结构体
			   BSP_U32 u32Len	消息结构体长度
 * 输出参数  : 无
 *
 * 返 回 值  : OSAL_OK:	  操作成功
 *			   OSAL_ERROR: 操作失败
 *
 * 其它说明  : 无
 *
 *****************************************************************************/
BSP_S32 Onup_set_Status_func(BSP_VOID *pMsgBody, BSP_U32 u32Len)
{
    BSP_S32 ret;
    ONUP_SET_STATUS_MSG_STRU tmp_info_stru = {0};	    
    UPDATE_INFOMATION updateinfo;
    char logTmpBuf[LOG_BUF_LEN] = {0};
            
    /*读取在线升级update_info.bin文件的信息*/
    if(OSAL_OK != update_UpDateInfoGet(&updateinfo))
    {       
        printk("Onup_set_Status_func: read update info failed!\n");        
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%d Onup_set_Status_func: read update info failed!\n",jiffies%100);
        update_record_file(logTmpBuf);

        tmp_info_stru.u32SetStatusRet = 0;//状态位设置失败
    }
    else
    {
        if(UPDATE_PROGRESSING == updateinfo.m_current_component_status)
        {        
            updateinfo.m_current_component_status = UPDATE_SUCCESSFUL_NO_DATA;  /*升级状态位：升级成功，数据已删除*/
            if(updateOnline_GetGoOnFlag())  /* 循环测试模式 */
            {
                updateinfo.m_current_component_status = UPDATE_SUCCESSFUL_NO_DATA;  /*升级状态位：升级失败，数据已删除*/
            }
        }
        else
        {
            updateinfo.m_current_component_status = UPDATE_FAILED_NO_DATA;      /*升级状态位：升级失败，数据已删除*/
        }

        /*设置在线升级状态位*/
        ret = update_UpDateInfoSet(&updateinfo);
        if(OSAL_OK != ret)
        {
            printk("update_UpDateInfoSet:error \n");
            memset((void*)logTmpBuf,LOG_BUF_LEN,0);
            snprintf(logTmpBuf,LOG_BUF_LEN, "%d Onup_set_Status_func: update_UpDateInfoSet:error!\n",jiffies%100);
            update_record_file(logTmpBuf);
            
            tmp_info_stru.u32SetStatusRet=0;    /*状态位设置失败*/
        }
        else
        {
            printk(KERN_DEBUG"update_UpDateInfoSet:successful \n");            
            memset((void*)logTmpBuf,LOG_BUF_LEN,0);
            snprintf(logTmpBuf,LOG_BUF_LEN, "%d Onup_set_Status_func: update_UpDateInfoSet:successful!\n",jiffies%100);
            update_record_file(logTmpBuf);
            
            tmp_info_stru.u32SetStatusRet=1;    /*状态位设置成功*/
        }
    }
	 
    tmp_info_stru.u32ModuleId = IFC_MODULE_BSP;
    tmp_info_stru.u32FuncId = IFCP_FUNC_ONLINE_UP_STUTASINFO_GET;
    printk("tmp_info_stru.u32SetStatusRet= %d \n",tmp_info_stru.u32SetStatusRet);
    
    /*通过IFC将消息发出*/
    ret = BSP_IFC_Send((BSP_VOID*)&tmp_info_stru, sizeof(ONUP_SET_STATUS_MSG_STRU));
    if(OSAL_OK != ret)
    {
		 printk("Onup_set_Status_func: ACPU IFC Fail!ret=%d\n", ret);
         memset((void*)logTmpBuf,LOG_BUF_LEN,0);
         snprintf(logTmpBuf,LOG_BUF_LEN, "%d Onup_set_Status_func: ACPU IFC Fail!!\n",jiffies%100);
         update_record_file(logTmpBuf);
		 
		 return OSAL_ERROR;
    }
    
    return OSAL_OK; 	 
}

 /*****************************************************************************
 * 函 数 名  : Onup_setstatus_ifc_init
 *
 * 功能描述  : 在线升级 双核通信初始化函数，设置升级状态位
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 其它说明  : 
 *
 *****************************************************************************/
BSP_S32 Onup_setstatus_ifc_init(void)
{
    BSP_U32 ret = 0;  
	
    ret = BSP_IFCP_RegFunc(IFCP_FUNC_ONLINE_UP_STUTASINFO_SET, (BSP_IFCP_REG_FUNC)Onup_set_Status_func);
    if(OSAL_OK != ret)
    {
        printk("Onup_setstatus_ifc_init: BSP_IFC_RegFunc failed.\n");
        return OSAL_ERROR;
    }

    ret = (BSP_U32)BSP_SYNC_Give(SYNC_MODULE_ONLINE);
    if(OSAL_OK != ret)
    {
        printk("Onup_setstatus_ifc_init: BSP_SYNC_Give failed.\n");
        return ret;
    }
    
    printk(KERN_DEBUG"Onup_setstatus_ifc_init: BSP_IFC_RegFunc success.\n");
    
    return OSAL_OK;
}

static int __init updateOnline_init(void)
{
	printk("updateOnline_init\n");
	updateOnline_register();
	
	/*初始化设置在线升级状态位函数*/
	printk("Onup_setstatus_ifc_init\n");
	Onup_setstatus_ifc_init();

    if(updateOnline_GetGoOnFlag())
    {
        updateOnline_sendmsg();
    }
    
    return OSAL_OK;
}

static void __exit updateOnline_exit(void)
{
	printk("updateOnline_exit\n");
}

module_init(updateOnline_init);
module_exit(updateOnline_exit);

MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("Balong updateOnline driver for Hisilicon");
MODULE_LICENSE("GPL");
#endif
