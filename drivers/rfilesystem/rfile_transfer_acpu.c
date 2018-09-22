/*************************************************************************
*   ��Ȩ����(C) 1987-2004, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_FILE_IF.C
*
*   ��    �� :  Y00182216
*
*   ��    �� :  DRV �ṩ��OM�Ľӿ�
*
*   �޸ļ�¼ :  2012��4��11��  v1.00  Y00182216  ����
*1.
*************************************************************************/
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/kallsyms.h>
#include <linux/semaphore.h>    /*����mutex�����ͷ�ļ�*/
#include <linux/kernel.h>       /*kmalloc,printk*/
#include <linux/kthread.h>      /*�����߳�*/
#include <asm/io.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/err.h>
#include <linux/module.h>
#include "BSP.h"
#include "rfile_transfer_acpu.h"
#include "rfilesystem_logunit.h"

#ifndef __u8
#define __u8 (unsigned char)
#endif

#if 1
/*
 * Utility procedures to print a buffer in hex/ascii
 */
static void
tty_print_hex (register unsigned char * out, const unsigned char * in, int count)
{
	register unsigned char next_ch;
	static const char hex[] = "0123456789ABCDEF";

	while (count-- > 0) {
		next_ch = *in++;
		*out++ = hex[(next_ch >> 4) & 0x0F];
		*out++ = hex[next_ch & 0x0F];
		++out;
	}
}

static void
tty_print_char (register unsigned char * out, const unsigned char * in, int count)
{
	register unsigned char next_ch;

	while (count-- > 0) {
		next_ch = *in++;

		if (next_ch < 0x20 || next_ch > 0x7e)
			*out++ = '.';
		else {
			*out++ = next_ch;
			if (next_ch == '%')   /* printk/syslogd has a bug !! */
				*out++ = '%';
		}
	}
	*out = '\0';
}

//out is 1, in is 0
static void
tty_print_buffer (const char *name, const unsigned char *buf, int count,int inout_flag)
{
#define CLEN 32
	unsigned char line[CLEN*4+4];

	if (name != NULL)
	{
		if(inout_flag)
		printk("[%s,>>>count=%d]", name, count);
		else
		printk("[%s,<<<count=%d]", name, count);
	}

	while (count > CLEN) {
		memset (line, 32, CLEN*3+1);
		tty_print_hex (line, buf, CLEN);
		tty_print_char (&line[CLEN * 3], buf, CLEN);
		printk("%s\n", line);
		count -= CLEN;
		buf += CLEN;
	}

	if (count > 0) {
		memset (line, 32, CLEN*3+1);
		tty_print_hex (line, buf, count);
		tty_print_char (&line[CLEN * 3], buf, count);
		printk("%s\n", line);
	}
}
#endif
typedef struct
{
    BSP_U32 ulModuleId;
    BSP_U32 ulFuncId;
    BSP_U32 ulFlag;
}RFILE_IFC_MSG;

extern uint32_t s_ulhsicEnumComplete;

#if defined (FEATURE_FLASH_LESS)
extern struct semaphore rfile_sema;
extern struct semaphore VOSModule_sema;
struct task_struct   *g_RfileInitThread = NULL;
struct task_struct   *g_VOSModuleInitThread = NULL;
extern unsigned int NVM_Init(unsigned int ulOption);
#ifdef BOOT_OPTI_BUILDIN
extern int VOS_ModuleInit(void);
#endif

����flashless,������PWRCTRL_SleepInitialʱ nv��û����ɳ�ʼ����
������Ҫ�Ƴٶ�ȡNV�ļ��еĵ͹������ã����������������rfile_init_cb�е���*/
#if ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
extern void PWRCTRL_ReInitialDfs_inRfile(void);
#endif
/*ends*/
/*Added by l00212112, 20120910,starts
������padԭ�д����У�����ȡnv 36ʱ��nvm_init��û�����У�
*/
extern void BSP_USB_ReGetModeNv(void);
/*ends*/


RFILECONTEXT_A rFileContext_a;

int rfile_ifc_cp2ap_ack(void)
{
    int ret = 0;
	unsigned int ulAck = 0xaaaa;
	ret = udi_write(rFileContext_a.rFileHsic9Fd,&ulAck,sizeof(ulAck));
	if (ret != sizeof(ulAck))
	{
	    printk("a:cp2ap ack error\n");
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, NULL, rFileContext_a.rFileHsic9Fd, FLASHLESS_FILEOPER_WRITE, ret, 0);
	}
	printk("ACK: AA AA 00 00\n");
	return 0;
}

void rfile_ifc_regfunc(void)
{
    int ret = 0;
    ret = BSP_IFCP_RegFunc(IFCP_FUNC_RFILE_CP2AP_SEM,(BSP_IFCP_REG_FUNC)rfile_ifc_cp2ap_ack);
    if(0 != ret)
    {
        printk("C:BSP_IFCP_RegFunc is failed\n");
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_ICC, "rfile_ifc_regfunc Fail!", 0, 0, RFILE_OPER_RESULT_FAIL, 0);
        return;
    }
    printk("BSP_IFCP_RegFunc\n");
}

void rfile_event_ap_notify_cp_init(void)
{
    RFILE_IFC_MSG pMsg = {0};
	pMsg.ulModuleId = IFC_MODULE_BSP;
	pMsg.ulFuncId  = IFCP_FUNC_RFILE_AP_NOTIFY_CP_INIT;
	pMsg.ulFlag    = 1;
	if(0 != BSP_IFC_Send((BSP_VOID*)&pMsg, sizeof(RFILE_IFC_MSG)))
    {
        printk("FRILE: IFC Send Fail!\n");
        return;
    }
    return ;
}

void rfile_a_give_c_sem(void)
{
    RFILE_IFC_MSG pMsg = {0};
	pMsg.ulModuleId = IFC_MODULE_BSP;
	pMsg.ulFuncId  = IFCP_FUNC_RFILE_SEM;
	pMsg.ulFlag    = 1;
	//printk(KERN_DEBUG"a->c sem\n");
	if(0 != BSP_IFC_Send((BSP_VOID*)&pMsg, sizeof(RFILE_IFC_MSG)))
    {
        printk("FRILE: IFC Send Fail!\n");
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_ICC, "FRILE: IFC Send Fail!", 0, 0, RFILE_OPER_RESULT_FAIL, 0);
        return;
    }
    return ;
}
/*************************************************************************
 �� �� ��   : rfile_transfer_init
 ��������   : rfile_transfer_init
 �������   : 
 �������   :
 �� �� ֵ   :  
 ��    ��   : 2012��4��11��
*************************************************************************/
int rfile_transfer_init(void)
{
    /*��ģ��ȷ����USB��ʼ��֮��*/
	int  ret = 0;
			
	printk("A:rfile_transfer_init start \n");

    printk("begain notify cp init\n");
    rfile_event_ap_notify_cp_init();
	printk("A:rfile_transfer_init ok\n");
	
	rFileContext_a.attr.enChanMode  = ICC_CHAN_MODE_PACKET;
	rFileContext_a.attr.u32Priority = 0;
	rFileContext_a.attr.u32TimeOut  = 10000;
	rFileContext_a.attr.event_cb    = NULL;
	rFileContext_a.attr.read_cb     = rfile_icc_read_cb;//ICC�Ļص�
	rFileContext_a.attr.write_cb    = NULL;
	rFileContext_a.attr.u32FIFOInSize  = 16*1024;
	rFileContext_a.attr.u32FIFOOutSize = 16*1024;

	rFileContext_a.rFileIccParam.devid = UDI_ICC_GUOM5_ID;
	rFileContext_a.rFileIccParam.pPrivate = &rFileContext_a.attr;
	/*��ICCͨ��*/
	printk("open udi ICC 1111111111111111111111111111111\n");
	rFileContext_a.rFileIccFd = udi_open(&rFileContext_a.rFileIccParam);
	if (rFileContext_a.rFileIccFd <= 0)
	{
	    printk("open ICC failed:[0x%x]\n",rFileContext_a.rFileIccFd);
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, "open ICC failed!", rFileContext_a.rFileIccFd, FLASHLESS_FILEOPER_OPEN, RFILE_OPER_RESULT_FAIL, 0);
        return (int)-1;
	}
	
	/*��HSIC9ͨ��*/
	rFileContext_a.rFileHsic9Param.devid = UDI_ACM_HSIC_ACM9_ID;
	printk("open udi HSIC 222222222222222222222222222222\n");
	rFileContext_a.rFileHsic9Fd	= udi_open(&rFileContext_a.rFileHsic9Param);
	if (rFileContext_a.rFileHsic9Fd <= 0)
	{
	    printk("A:rfile_transfer_init acm open fail: [0x%x]\n",rFileContext_a.rFileHsic9Fd);
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, " acm open fail!", rFileContext_a.rFileHsic9Fd, FLASHLESS_FILEOPER_OPEN, RFILE_OPER_RESULT_FAIL, 0);
        return (int)-1;
    }
    /*ע���HSIC�ص�*/
	printk("udi)ioctl 333333333333333333333333333333333333\n");
	ret = udi_ioctl(rFileContext_a.rFileHsic9Fd, ACM_IOCTL_SET_READ_CB, (int)rfile_hsic_read_cb);
	if (0 != ret)
	{
	    printk("A:rfile_transfer_init cb register fail: [0x%x]\n", ret);
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, "register fail", rFileContext_a.rFileHsic9Fd, FLASHLESS_FILEOPER_SET, ret, 0);
        return (int)-1;
	}

    /*���ٷ��ͻ���*//*�����HSIC��������������û�п��ٿռ�*/
	rFileContext_a.sendBuf = (void*)kmalloc(TRANS_MAX_SIZE, GFP_KERNEL);
	if (NULL == rFileContext_a.sendBuf)
	{
        printk("malloc is failed\n");
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_LOCAL, "malloc is failed", 0, 0, RFILE_OPER_RESULT_FAIL, 0);
		return (int)-1;
	}
    rfile_ifc_regfunc();
	sema_init(&(rFileContext_a.ap2mpSem),1);
	sema_init(&(rFileContext_a.mp2apSem),1);
	rFileContext_a.hsicOpened = 1;
	rFileContext_a.iccOpened  = 1;
    FLASHLESS_CMD_ADD_LOG(RFS_EVENT_LOCAL, "rfile transfer init ok", 0, FLASHLESS_FILEOPER_INIT, RFILE_OPER_RESULT_OK, 0);

	return 0; //�Ժ�Ҫ�ú�	
}
/*************************************************************************
 �� �� ��   : rfile_hsic_read_cb
 ��������   : rfile_hsic_read_cb
 �������   : 
 �������   :
 �� �� ֵ   :  
 ��    ��   : 2012��4��11��
*************************************************************************/
void rfile_hsic_read_cb(void)
{
    int ret;
	int tmpframelen = 0;
	int i = 28;
	unsigned int u32size;
    ACM_WR_ASYNC_INFO  stCtlParam;
    unsigned char *p = NULL;
    RFILE_MNTN_HEAD *q = NULL;
	//printk("rfile_hsic_read_cb\n");
	if ((1 != rFileContext_a.hsicOpened) || (1 != rFileContext_a.iccOpened))
	{
        printk("hsic is not Opened or icc is not Opened\n");
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, "hsic or icc not Opened", 0, 0, rFileContext_a.hsicOpened, rFileContext_a.iccOpened);
	    return;
	}

	/*��ȡ���е�buf*/
    ret = udi_ioctl(rFileContext_a.rFileHsic9Fd, ACM_IOCTL_GET_RD_BUFF, &stCtlParam);
    if ( 0 != ret )
    {
        printk(" Get hsic buffer failed%d!\n",ret);
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, "Get buffer failed", rFileContext_a.rFileHsic9Fd, FLASHLESS_FILEOPER_SET, ret, 0);
        return;
    }
	if (NULL == stCtlParam.pBuffer)
    {
        printk(" stCtlParam.pBuffer is   %d!",stCtlParam.pBuffer);
		ret = udi_ioctl(rFileContext_a.rFileHsic9Fd, ACM_IOCTL_RETURN_BUFF, &stCtlParam);
	    if ( 0 != ret )
	    {
	        printk(" Get hsic buffer failed  %d!",ret);
               FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, "Get buffer failed", rFileContext_a.rFileHsic9Fd, FLASHLESS_FILEOPER_SET, ret, 0);

	        return;
	    }
        return;
    }
	
	tmpframelen = stCtlParam.u32Size;
	
    if (0 == stCtlParam.u32Size)
    {
        printk("stCtlParam.u32Size is error%d\n",stCtlParam.u32Size);
		ret = udi_ioctl(rFileContext_a.rFileHsic9Fd, ACM_IOCTL_RETURN_BUFF, &stCtlParam);
	    if ( 0 != ret )
	    {
	        printk(" Get hsic buffer failed  %d!",ret);
               FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, "Get buffer failed", rFileContext_a.rFileHsic9Fd, FLASHLESS_FILEOPER_SET, ret, 0);

	        return;
	    }
		return;
	}
		
    p = (unsigned char *)stCtlParam.pBuffer;
    q = stCtlParam.pBuffer;

//    tty_print_buffer("ttyUSB8",p,tmpframelen,0);
#if 0  //ydb

    while(i--)
    {
        printk("%#x ",*p++);
	}
	printk("\n");
#endif
    do
    {
	/*����ICC�ķ��ͣ�����Ӧ��Ϣ����*/
	    u32size = udi_write(rFileContext_a.rFileIccFd,stCtlParam.pBuffer,stCtlParam.u32Size);
		if (u32size != stCtlParam.u32Size)
		{
	        printk("ICC write failed : the size is %d\n",u32size);
               FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, "ICC write failed", rFileContext_a.rFileIccFd, FLASHLESS_FILEOPER_WRITE, u32size, stCtlParam.u32Size);

		}
    }while(u32size != stCtlParam.u32Size);
	ret = udi_ioctl(rFileContext_a.rFileHsic9Fd, ACM_IOCTL_RETURN_BUFF, &stCtlParam);
    if ( 0 != ret )
    {
        printk(" Get hsic buffer failed  %d!",ret);
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, "Get hsic buffer failed", rFileContext_a.rFileHsic9Fd, FLASHLESS_FILEOPER_SET, ret, 0);
        return;
    }
	//printk("rfile_hsic_read_cb is over\n");
	return;
}
/*************************************************************************
 �� �� ��   : rfile_icc_write_cb
 ��������   : rfile_icc_write_cb
 �������   : 
 �������   :
 �� �� ֵ   :  
 ��    ��   : 2012��4��11��
*************************************************************************/
void rfile_icc_write_cb(void)
{
	if ((1 != rFileContext_a.hsicOpened) || (1 != rFileContext_a.iccOpened))
	{
        printk("hsic is not Opened or icc is not Opened\n");
            FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, "hsic or icc not Opened", 0, FLASHLESS_FILEOPER_SET, rFileContext_a.hsicOpened, rFileContext_a.iccOpened);
		return;
	}
    up(&rFileContext_a.ap2mpSem);
}
/*************************************************************************
 �� �� ��   : rfile_icc_read_cb
 ��������   : rfile_icc_read_cb
 �������   : 
 �������   :
 �� �� ֵ   :  
 ��    ��   : 2012��4��11��
*************************************************************************/
void rfile_icc_read_cb(unsigned int chenalId,unsigned int u32size)
{
    unsigned int readSize;
    ACM_WR_ASYNC_INFO  stCtlParam;
    RFILE_MNTN_HEAD *p = NULL;
	unsigned char *pp = NULL;
	int i = 28;
	int tmpframelen = u32size;
    int rewrite_times = 3;
	
	
	//printk(KERN_DEBUG"icc read cb\n");
	
	if ((1 != rFileContext_a.hsicOpened) || (1 != rFileContext_a.iccOpened))
	{
        printk("hsic is not Opened or icc is not Opened\n");
        rfile_a_give_c_sem();
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_RECEIVEAP, "hsic or icc not Opened", 0, FLASHLESS_FILEOPER_SET, rFileContext_a.hsicOpened, rFileContext_a.iccOpened);
		return;
	}
		
	/*ICC��ȡ����*/
	readSize = udi_read(rFileContext_a.rFileIccFd,rFileContext_a.sendBuf,u32size);
	if (readSize != u32size)
	{
	    printk("udi_read 's size is error readSize:%d VS u32size:%d\n",readSize,u32size);
        rfile_a_give_c_sem();
         FLASHLESS_CMD_ADD_LOG(RFS_EVENT_ICC, "udi_read error", rFileContext_a.rFileIccFd, FLASHLESS_FILEOPER_READ, readSize, u32size);

		return;
	}
	//printk(KERN_DEBUG"icc read ok\n");
	pp = (unsigned char *)rFileContext_a.sendBuf;

	if (i > readSize)
	{
        i = readSize;
	}

	while(i--)
	{
        //printk(KERN_DEBUG"%#x ",*pp++);
	}
	//printk(KERN_DEBUG"\n");
	
	/*HSIC�������ݸ�AP*/
	stCtlParam.pBuffer = rFileContext_a.sendBuf;
	stCtlParam.u32Size = u32size;

   // tty_print_buffer("ttyUSB8",rFileContext_a.sendBuf,u32size,1);
#if 0
	printk("totalFrames %d\n",p->totalFrames);
	printk("numbis %d\n",p->frameNumb);
	printk("in rfile_icc_read_cb 0x%x\n",p->reserve);
	
    if (p->reserve != 0x55AA)
    {
		printk("in rfile_icc_read_cb opsPid is %d\n",p->opsPid);
		printk("in rfile_icc_read_cb totalFrames is %d\n",p->totalFrames);
		printk("in rfile_icc_read_cb frameNumb is %d\n",p->frameNumb);
		printk("in rfile_icc_read_cb curFrameLen is %d\n",p->curFrameLen);
		printk("in rfile_icc_read_cb 0x%x\n",p->reserve);
	}

#endif
    do
    {
        readSize = udi_write(rFileContext_a.rFileHsic9Fd,rFileContext_a.sendBuf,u32size);
        rewrite_times--;
    }
    while((readSize != u32size)&&(rewrite_times > 0));
    
    if (readSize != u32size)
	{
	    printk("udi_write 's size is error readSize:%d VS u32size:%d\n",readSize,u32size);
        FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, "udi_write error", rFileContext_a.rFileHsic9Fd, FLASHLESS_FILEOPER_WRITE, readSize, u32size);
        rfile_a_give_c_sem();
		return;
	}
	//printk(KERN_DEBUG"hsic send is ok\n");
    rfile_a_give_c_sem();
	
	return;
}


uint32_t rfile_init_cb(void)
{
    int ret;
	for(;;)
	{
		down(&(rfile_sema));
		rfile_transfer_init();

	    ret = NVM_Init(0);
	    if (0 != ret)
	    {
	        printk("\r NVM_Init fail, result = 0x%x \n", ret);
	    }	

		printk("\r NVM_Init ok, result = 0x%x \n", ret);

		up(&(VOSModule_sema));
		return 0;
	}
}

uint32_t VOSModule_init_cb(void)
{
    int ret; 
 	for(;;)
	{
		down(&(VOSModule_sema));
		VOS_ModuleInit();

		printk("\r VOS_ModuleInit ok, result = 0x%x \n", ret);
/*Added by l00212112, 20120910,starts
������padԭ�д����У�����ȡnv 36ʱ��nvm_init��û�����У�
*/
        BSP_USB_ReGetModeNv();
/*ends*/

����flashless,������PWRCTRL_SleepInitialʱ nv��û����ɳ�ʼ����
������Ҫ�Ƴٶ�ȡNV�ļ��еĵ͹������ã����������������rfile_init_cb�е���*/
#if ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
        PWRCTRL_ReInitialDfs_inRfile();
#endif
/*ends*/
		return 0;
	}
}

void rfile_init_thread(void)
{
	g_RfileInitThread = kthread_run(rfile_init_cb,NULL,"rfile_init");
	g_VOSModuleInitThread = kthread_run(VOSModule_init_cb,NULL,"VOSModule_init");
}

subsys_initcall(rfile_init_thread);
#endif
