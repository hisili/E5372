/*************************************************************************
*   ��Ȩ����(C) 1987-2011, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  uartdrv.c
*
*   ��    �� :  yudongbin
*
*   ��    �� :  ���ļ�����Ϊ"uartdrv.c"
*
*   �޸ļ�¼ :  2011��9��19��  v1.00  yudongbin����
*************************************************************************/
#include <linux/semaphore.h>    /*����mutex�����ͷ�ļ�*/
#include <linux/kernel.h>       /*kmalloc,printk*/
#include <linux/kthread.h>      /*�����߳�*/
#include "BSP.h"
#include <asm/io.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/irq.h>
#include <linux/serial_core.h>

#include "uartDrv.h"

/*���Ժ궨��*/
//#define MSP_IF_IOCTL_TEST
//#define TEST_UART_LOL
/*UT,ST���Ա���*/
BSP_U32 printkflag = 0;
BSP_U32 intflag    = 0;
BSP_U32 dbgbit = 0;
BSP_U32 buf_look = 0;
BSP_U32 int_irq = 0;
BSP_U8 test_buf[5] = {1,2,3,4,5};
BSP_U8 sel_uart = 1;
BSP_U32 g_rx_size = 16;
BSP_U32 g_rx_num  = 3096;

UART_PORT_S uart_port0 = {0};
UART_CTX_S  UartCtx    = {0};
struct task_struct   *g_pstUartReadThread = NULL;




/*****************************************************************************
* �� �� ��  : BSP_UartOpen
*
* ��������  : ��uart�豸
*
* �������  : s32UartDevId  �豸����ṹָ��
* �������  : s32Flags
            : s32Mode
*
* �� �� ֵ  : �ɹ�/ʧ��
*
*****************************************************************************/

BSP_S32 BSP_UartOpen(BSP_S32 s32UartDevId, BSP_S32 s32Flags, BSP_S32 s32Mode)
{
    UART_CTX_S* pUartDevCtx = (UART_CTX_S*)s32UartDevId;  
    
    if (NULL == pUartDevCtx)
    {
        (void)printk("the pUartDevCtx is NULL line is\n");
        return BSP_ERROR;
    }
	
    if (pUartDevCtx->bOpen)
    {
        (void)printk("the pUartDevCtx is opened line is\n");
        return BSP_ERROR;
    }
    
    pUartDevCtx->bOpen = TRUE;

	/*��Ϊ�ڳ�ʼ����ʱ���Ѿ������ˣ�just ���phy���CTX������*/
    if(BSP_OK != get_uart_port((BSP_U32)pUartDevCtx))
    {
        (void)printk("BSP_UartOpen is failed line is \n"); 
	    return BSP_ERROR;
	}

    return ((BSP_S32)pUartDevCtx);
}

/*****************************************************************************
* �� �� ��  : BSP_UdiUartAdpOpen
*
* ��������  : ��uart�豸
*
* �������  : UDI_OPEN_PARAM *param, UDI_HANDLE handle
* �������  : 
          
*
* �� �� ֵ  : �����ļ����
*
*****************************************************************************/

static BSP_S32 BSP_UdiUartAdpOpen(UDI_OPEN_PARAM *param, UDI_HANDLE handle)
{
	BSP_S32 s32Fd;
    
    /*��ȡ���豸��,���������uartDrv.h����*/
   
    /* u32Type ʵ���ϵ���0 */
	s32Fd = BSP_UartOpen((BSP_S32)(&UartCtx), 0, 0);
      
	if (s32Fd == -1)
	{
		(void)printk("BSP_MODU_UDI open fail, line:%d\n", __LINE__);
		return BSP_ERROR;
	}
    /*ͬ��*/
	(BSP_VOID)BSP_UDI_SetPrivate(param->devid, (VOID*)s32Fd);

	return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : BSP_UartWrite
*
* ��������  : uart���ͽӿ�
*
* �������  : BSP_S32 s32UartDev, BSP_U8* pBuf, BSP_U32 u32Size
* �������  : ʵ��д�������
          
*
* �� �� ֵ  :ʧ��/ʵ�ʵ�����
*
*****************************************************************************/

BSP_S32 BSP_UartWrite(BSP_S32 s32UartDevCtx, BSP_U8* pBuf, BSP_U32 u32Size)
{
    UART_CTX_S* pUartDevCtx = (UART_CTX_S*) s32UartDevCtx;
    BSP_S32 ret;
    /*�ػ�����*/
#ifdef MSP_IF_IOCTL_TEST
    int i = 0;
    (void)printk("\n#########DL DATA ##########\n");

    for(i = 0; i< (BSP_S32)u32Size;i++)
    {
        (void)printk("%x ",(BSP_U8)pBuf[i]);
    }
    (void)printk("\n");
#endif

    if(TRUE != pUartDevCtx->bOpen )
    {
        (void)printk("it is not open!\n");
        return BSP_ERROR;
    }
    down(&UartCtx.MSP_Tx_Sem);
    /*����UartCtx*/
    ret = uart_send((BSP_S32)pUartDevCtx,pBuf,u32Size);
    if (ret == (BSP_S32)u32Size)
    {
       /*����UART����ҪDMA�ڴ棬�����Ļ������ͳɹ�Ҳ����ҪDRV�ͷ�*/
    }
    else
    {
        (void)printk("uart_send is failed\n");
        return BSP_ERROR;
    }
    
    up(&UartCtx.MSP_Tx_Sem);
    return (ret);
}/*lint !e529*/
/*****************************************************************************
* �� �� ��  : BSP_UartClose
*
* ��������  : �ر�uart�豸
*
* �������  : handle
* �������  : 
          
*
* �� �� ֵ  :ʧ��/�ɹ�
*
*****************************************************************************/

BSP_S32 BSP_UartClose(BSP_S32 s32UartDevCtx)
{
    UART_CTX_S *pUartDevCtx = (UART_CTX_S *)s32UartDevCtx;
    pUartDevCtx->bOpen = FALSE;
    
	return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : BSP_UartIoctl
*
* ��������  : UART ҵ����� ioctl ����
*
* �������  : s32UartDevId: �豸����ṹָ��
*             s32Cmd: ������
*             s32Arg: �������
* �������  : 
*
* �� �� ֵ  : �ɹ�/ʧ��
*
*****************************************************************************/

BSP_S32 BSP_UartIoctl(BSP_S32 s32UartDevId, BSP_S32 s32Cmd, BSP_S32 s32Arg)
{
    UART_CTX_S* pUartCtx = (UART_CTX_S*)s32UartDevId;
    
    BSP_S32 ret = BSP_OK;

    if (NULL == pUartCtx || !pUartCtx->bOpen)
    {
        (void)printk("THE PARAM IS ERROR\n");
        return BSP_ERROR;
    }

    switch(s32Cmd)
    {
            /* 1ע�����е�buf�ͷź���OK */
            case UART_IOCTL_SET_WRITE_CB:               

                pUartCtx->MSP_WriteCb = (UART_WRITE_DONE_CB_T)s32Arg;
                
                break;
            
             /*3MSP���û������buf�ĵ�ַ�����������ǵ�read�ص������������õ�*/
            case UART_IOCTL_GET_RD_BUFF:
                {

                    if (0 == s32Arg)
                    {
                        (void)printk("write UART_IOCTL_WRITE_ASYNC invalid args\n");
                        return BSP_ERROR;
                    }
                    ret = BSP_UartGetReadBuffer(s32UartDevId, (UART_WR_ASYNC_INFO*)s32Arg);
                }
                break;
            /*4���ǽ�����buf��ַ����MSP����MSP�����ô˽ӿڽ����ͷ�*/
            case UART_IOCTL_RETURN_BUFF:
                {
                    
                    if (0 == s32Arg)
                    {
                        (void)printk("write UART_IOCTL_RETURN_BUFF invalid args\n");
                        return BSP_ERROR;
                    }
                    ret = BSP_UartReturnReadBuffer(s32UartDevId, (UART_WR_ASYNC_INFO*)s32Arg);
                }
                break;
            /*6MSP�Ľ��շ��ͺ����������յ����ݺ󣬻������ô˽ӿ�OK*/
            case UART_IOCTL_SET_READ_CB:
                {                    
                    pUartCtx->MSP_ReadCb = (UART_READ_DONE_CB_T)s32Arg;

                    if(printkflag == 1)
                    {
                        (void)printk("pUartCtx->MSP_ReadCb ==  0X%x\n",(BSP_U32)pUartCtx->MSP_ReadCb);
                    }
                }
                
                break;  

            default:
                (void)printk("error cmd \n");
                break;
    }
    
    return ret;
}

/*****************************************************************************
* �� �� ��  : BSP_UdiAdpUartInit
*
* ��������  : UART ���õĳ�ʼ���ӿ�
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR
*
*****************************************************************************/
BSP_S32  BSP_UdiAdpUartInit(void) 
{
	UDI_DRV_INTEFACE_TABLE* pDrvInterface = NULL;
    
    set_rx_buf(3096,16);
    
	/*����UARTоƬ������ĳ�ʼ��*/
    if(uart_init() != OK)
    {
        (void)printk("uart_init is error\n");
        return BSP_ERROR;
    } 

    if(uart_rx_buf_init() != OK)
    {
        (void)printk("uart_rx_buf_init is error\n");
        return BSP_ERROR;
    }

    /*��ʼ�����͵Ļ����ź���*/
    sema_init(&(UartCtx.MSP_Tx_Sem), SEM_FULL);
	/* ����ص�����ָ���б� */
	/******UDI��ĳ�ʼ��*****/
	pDrvInterface = kmalloc(sizeof(UDI_DRV_INTEFACE_TABLE), GFP_KERNEL);
	if (NULL == pDrvInterface)
	{
		(void)printk("BSP_MODU_UDI NO Mem, line:%d\n", __LINE__);
		return BSP_ERROR;
	}
	memset((VOID*)pDrvInterface, 0, sizeof(UDI_DRV_INTEFACE_TABLE));

	/* ֻ������Ҫʵ�ֵļ����ӿ� */
	pDrvInterface->udi_open_cb = BSP_UdiUartAdpOpen;
	pDrvInterface->udi_close_cb = (UDI_CLOSE_CB_T)BSP_UartClose;
	pDrvInterface->udi_write_cb = (UDI_WRITE_CB_T)BSP_UartWrite;
	pDrvInterface->udi_ioctl_cb = (UDI_IOCTL_CB_T)BSP_UartIoctl;

	(BSP_VOID)BSP_UDI_SetCapability((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_UART, 0), 0);
     (void)printk("UDI_BUILD_DEV_ID is 0x%x\n",UDI_BUILD_DEV_ID(UDI_DEV_UART, 0));
	(BSP_VOID)BSP_UDI_SetInterfaceTable((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_UART, 0), pDrvInterface);

    /*��uartctx��open����0*/
    UartCtx.bOpen = FALSE;
    
    /* UDI��ĳ�ʼ������ */  
	return BSP_OK;
}
/*****************************************************************************
* �� �� ��  : BSP_UdiAdpUartUnInit
*
* ��������  : UART ���õĳ�ʼ���ӿ�
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR
*
*****************************************************************************/
void BSP_UdiAdpUartUnInit(void)
{
    uart_rx_buf_uinit();
    uart_uninit();
}

/*****************************************************************************
* �� �� ��  : uart_find_mem_from_queue
*
* ��������  : �ҵ�push��MSP�ý��սڵ�
*
* �������  : BSP_S32 s32UartDevId, BSP_U8 *buf
* �������  : 
          
*
* �� �� ֵ  :ʧ��/�ɹ�
*
*****************************************************************************/

BSP_S32 uart_realloc_read_buf(UART_CTX_S* pUartCtx, UART_READ_BUFF_INFO* pstReadBuffInfo)
{
    
    return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : creat_queue
*
* ��������  :���� ���ջ������
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR
*
*****************************************************************************/

BSP_S32 creat_queue(UART_RX_BUF_S *pHead,BSP_U8 *pDataBuf)
{
    int i = 1;
    int j = 0;
    int k ;
    UART_RX_BUF_S *pTemp = pHead;
    
    (void)printk("pTemp is 0x%x\n",(BSP_U32)pTemp);
        
    /*��������ѭ������*/ 
    for(k = 0; k < (int)g_rx_num; k++)
    {
         /*����data����*/
         pTemp->buf = (BSP_U8*)&(pDataBuf[g_rx_size*(BSP_U32)(j++)]);
         pTemp->ulSize = 0;  
                  
         /*������Ϣͷ*/
         pTemp->pstNext = (struct tagUART_RX_BUF_S *)(&pHead[(i++)%((int)g_rx_num)]);
         pTemp = pTemp->pstNext;
         
    }

    pTemp = NULL;
    
    if(printkflag == 1)
    {
        (void)printk("i is : %d\n",i);
        (void)printk("pTemp is 0x%x ?= pHead 0x%x\n",(BSP_U32)pTemp, (BSP_U32)pHead);
        (void)printk("out func creat_queue line is %d\n",__LINE__);
    }
    
    return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : uart_rx_buf_init
*
* ��������  :��ʼ�����ջ������
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR
*
*****************************************************************************/
BSP_S32 uart_rx_buf_init(void)
{
    
    /*malloc��Ϣͷ�ռ�*/
    UartCtx.pHead_RxQueue = (UART_RX_BUF_S*)kmalloc(sizeof(UART_RX_BUF_S)*g_rx_num, GFP_KERNEL);
    if(NULL == UartCtx.pHead_RxQueue)
    {
        (void)printk("kmalloc is failed\n");
        return BSP_ERROR;
    }
    
    /*�������ݵ�buf*/
    UartCtx.pDataBuf = (BSP_U8 *)kmalloc(g_rx_size * g_rx_num ,GFP_KERNEL);
    if(NULL == UartCtx.pDataBuf)
    {
        (void)printk("kmalloc is failed\n");
        (void)kfree(UartCtx.pHead_RxQueue);
        UartCtx.pHead_RxQueue = NULL;
        
        return BSP_ERROR;
    }
    
    /*��������buf*/
    if (creat_queue(UartCtx.pHead_RxQueue,UartCtx.pDataBuf) != BSP_OK)
    {
        (void)printk("craet_queue is failed\n");
        (void)kfree(UartCtx.pHead_RxQueue);
        (void)kfree(UartCtx.pDataBuf);
        UartCtx.pHead_RxQueue = NULL;
        UartCtx.pDataBuf      = NULL;
        
        return BSP_ERROR;
    }

    /*���õ�ǰ��дָ��ָ��*/
    UartCtx.pstCurrRead  = UartCtx.pHead_RxQueue;
    UartCtx.pstCurrWrite = UartCtx.pHead_RxQueue;

    return BSP_OK;        
}
/*****************************************************************************
* �� �� ��  : get_uart_port0
*
* ��������  : UART ���õĳ�ʼ���ӿ�
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR/OK
*
*****************************************************************************/
void uart_rx_buf_uinit(void)
{
    /*�ͷ��ڴ�*/
    (void)kfree(UartCtx.pHead_RxQueue);
    (void)kfree(UartCtx.pDataBuf);
    UartCtx.pHead_RxQueue = NULL;
    UartCtx.pDataBuf      = NULL;
    UartCtx.pstCurrRead   = NULL;
    UartCtx.pstCurrWrite  = NULL;
    
    return;
}

/*****************************************************************************
* �� �� ��  : get_uart_port0
*
* ��������  : UART ���õĳ�ʼ���ӿ�
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR/OK
*
*****************************************************************************/

BSP_S32 get_uart_port(BSP_U32 UartDevId)
{
     UART_CTX_S *pUartDevCtx = (UART_CTX_S *) UartDevId;
          
	 if(NULL == pUartDevCtx)
	 {
         (void)printk("the pUartDevCtx is NULL line is %d\n",__LINE__);
		 return BSP_ERROR;
	 }

	 pUartDevCtx->port = (UART_PORT_S*)(&uart_port0);
     
	 return BSP_OK;
}
/*****************************************************************************
* �� �� ��  : BSP_UartReturnReadBuffer
*
* ��������  : MSP�ͷ�RX buf�Ľӿ�
*
* �������  :BSP_S32 s32UartDevId, ACM_WR_ASYNC_INFO* pWRInfo
* �������  :
*
* �� �� ֵ  : �ɹ�/ʧ��
*
*****************************************************************************/
BSP_S32 BSP_UartReturnReadBuffer(BSP_S32 s32UartDevId, ACM_WR_ASYNC_INFO* pWRInfo)
{
	UART_CTX_S* pUartCtx = (UART_CTX_S*)s32UartDevId;
    UART_RX_BUF_S* pMemInfo = NULL;
    
	if (NULL == pUartCtx || !pUartCtx->bOpen)
	{
        (void)printk("pUartCtx & pUartCtx->bOpen is error\n");
		return BSP_ERROR;
	}
    
	pMemInfo = pWRInfo->pDrvPriv;
    pMemInfo->ulSize = 0;/*�������ǹ黹�ڴ�*/

	return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : BSP_UartGetReadBuffer
*
* ��������  : MSP ��ȡ����buf����Ϣ
*
* �������  : BSP_S32 s32UartDevId, ACM_WR_ASYNC_INFO* pWRInfo
* �������  :
*
* �� �� ֵ  : �ɹ�/ʧ��
*
*****************************************************************************/
BSP_S32 BSP_UartGetReadBuffer(BSP_S32 s32UartDevId, ACM_WR_ASYNC_INFO* pWRInfo)
{
	UART_CTX_S* pUartCtx = (UART_CTX_S*)s32UartDevId;
	UART_RX_BUF_S* pMemInfo = NULL;
	if (NULL == pUartCtx || !pUartCtx->bOpen)
	{
        (void)printk("pUartCtx & pUartCtx->bOpen is error\n");
		return BSP_ERROR;
	}

	/* ��ȡ��ɵ� buffer�ڵ� */
	pMemInfo = pUartCtx->pstCurrRead;
	pWRInfo->pBuffer = (BSP_CHAR *)pMemInfo->buf; /*������ָ�������*/
	pWRInfo->u32Size = (BSP_U32)pMemInfo->ulSize; /*���ݵĴ�С*/
	pWRInfo->pDrvPriv = (void*)pMemInfo;          /*��ǰ��Ϣͷָ��*/
    
	return BSP_OK;
}


/*****************************************************************************
* �� �� ��  : uart_irq_enable
*
* ��������  : UART�ж�ʹ��
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR
*
*****************************************************************************/
void uart_irq_enable(BSP_U32 value)
{
    /*�ж�ʹ��*/
    (void)writel(value,uart_port0.vir_addr_base + DW_UART_IER);
    return;
}

/*****************************************************************************
* �� �� ��  : uart_irq_disable
*
* ��������  : UART�ж�ȥʹ��
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR
*
*****************************************************************************/
void uart_irq_disable(void)
{
    /*��ֹ�ж�*/
    (void)writel(0,uart_port0.vir_addr_base + DW_UART_IER);
    return;
}

/*****************************************************************************
* �� �� ��  : uart_rx_chars
*
* ��������  : UART�ײ���պ���
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR
*
*****************************************************************************/

static void uart_rx_chars(void)
{
    BSP_U32 status;
	BSP_U8 ch;
	/*pclint  734*/
	BSP_U8 max_count = (BSP_U8)g_rx_size;/*һ�����ݿ�Ĵ�С*/
    BSP_U8 *writebuf = UartCtx.pstCurrWrite->buf;

    status = readl(uart_port0.vir_addr_base + DW_UART_LSR);
    while ((0 != max_count--) && (status & 0x01))
    {
        ch = (BSP_U8)readl(uart_port0.vir_addr_base + DW_UART_RBR);
		*(writebuf++) = (BSP_U8)ch;
        /*UartCtx.pstCurrWrite->ulSize��ʼֵΪ0*/
        UartCtx.pstCurrWrite->ulSize++;
        
        status = readl(uart_port0.vir_addr_base + DW_UART_LSR);

    }
    
    if((UartCtx.pstCurrWrite->pstNext == UartCtx.pstCurrRead) || 
                   (UartCtx.pstCurrWrite->pstNext->ulSize !=0))
    {
         (void)printk("buf is full\n");
    }
    else
    {
        
        UartCtx.pstCurrWrite = UartCtx.pstCurrWrite->pstNext;
    }
   
    up(&uart_port0.uart_rx_sem);
    
    return;
}

/*****************************************************************************
* �� �� ��  : uart_recv_thread
*
* ��������  : UART�ײ�����߳�
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR
*
*****************************************************************************/

static void uart_recv_thread(void)
{
    for(;;)  //b00198513 Modified for pclint e716          
    {
        down(&uart_port0.uart_rx_sem);
        
//        if(printkflag == 1)
//            printk("in 1 func uart_recv_thread line is %d\n",__LINE__);
        
        while(UartCtx.pstCurrRead->ulSize != 0)
        {
            
//            if(printkflag == 1)
//                printk("in 1 func uart_recv_thread line is %d\n",__LINE__);
            
            if(UartCtx.MSP_ReadCb)
            { 
#ifdef MSP_IF_IOCTL_TEST
                if(printkflag == 1)
                {
			ACM_WR_ASYNC_INFO pWRinfo;
			int i = 0;	  
                     (void)printk("UartCtx.MSP_ReadCb =0X%x",UartCtx.MSP_ReadCb);
                     (void)printk("UartCtx.pstCurrRead:%x\n",UartCtx.pstCurrRead);
                     
                     if(BSP_OK == BSP_UartGetReadBuffer((BSP_S32)(&UartCtx),&pWRinfo))
                     {
                         (void)printk("#####pWRinfo->u32Size==%d\n",pWRinfo.u32Size);
                         for(i = 0;i < (BSP_S32)pWRinfo.u32Size; i++)
                         {
                             (void)printk("%x ",(BSP_U8)(pWRinfo.pBuffer)[i]);
                         }
                     }
                }
#endif
//                (void)printk("\n");
//                (void)printk("%d ",(((UartCtx.pstCurrWrite
//                                           - UartCtx.pstCurrRead + RX_NUM)%RX_NUM)));
                UartCtx.MSP_ReadCb();
                UartCtx.pstCurrRead->ulSize = 0;//��������MSP��"�ͷ�"�����ڴ棬����DRV������0
                /*�ݽ����ϲ�֮����bufΪ������MSP������*/           
            }
            else //һ�������Ѿ�ע���֮�󣬾Ͳ����������֧��
            {
                if(buf_look == 0)
                {
                    (void)uart_send((BSP_S32)(&UartCtx),UartCtx.pstCurrRead->buf,UartCtx.pstCurrRead->ulSize);
                }
                UartCtx.pstCurrRead->ulSize = 0;
            }
            /*һ��buf�������֮��,ָ���Ƶ���һ��buf*/
            if(UartCtx.pstCurrRead != UartCtx.pstCurrWrite)
            {
                UartCtx.pstCurrRead = UartCtx.pstCurrRead->pstNext; 
            }
            else/*����û�������ˣ���ô�ͻ���������ѭ�����ȴ��ź����ĵ���*/
            {
                (void)printk("data is empty\n");
                break;
            }                           
        }

	}
}/*lint !e529*/

/*****************************************************************************
* �� �� ��  : uart_int
*
* ��������  : uart �жϷ������
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR
*
*****************************************************************************/
static irqreturn_t uart_int(void)
{
    BSP_U32 interrupt_id;
	BSP_U32 status;
    BSP_S32 handled = 0;

    spin_lock(&(uart_port0.lock_irq));

    status = readl(uart_port0.vir_addr_base + DW_UART_IIR);
    interrupt_id = 0x0F&status; 
    while ((interrupt_id != DW_NOINTERRUPT))
    {   /*����������Ч*/    
        if ((DW_RECEIVEAVA == interrupt_id) ||  
            (DW_RECTIMEOUT == interrupt_id))    /*chaoshi�ж�*/
        {
            uart_rx_chars();
        }
        else
        {
            (void)readl(uart_port0.vir_addr_base + DW_UART_LSR);
            (void)readl(uart_port0.vir_addr_base + DW_UART_USR);
             handled = 1;
             break;
        }

        status = readl(uart_port0.vir_addr_base + DW_UART_IIR);
        interrupt_id = 0x0F&status;   
        handled = 1;
    }

    spin_unlock(&(uart_port0.lock_irq));
     
    return IRQ_RETVAL(handled);/*lint !e64*/
}
/*****************************************************************************
* �� �� ��  : uart_tx_chars
*
* ��������  : uart �ײ㷢�ͺ���
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR
*
*****************************************************************************/

static int uart_tx_chars(BSP_U8 *p_uart_tx_buf,BSP_U32 size)
{
    BSP_U8 *buf = p_uart_tx_buf;
    BSP_S32 Cnt = (BSP_S32)size;
    BSP_U32 UsrValue = 0;
        
    /*����������Ա�֤�ܹ���MSP�����ݷ��͵�FIFO����ȥ*/

//    (void)printk("\n******DL IN SEND_DATA******\n");
    do
	{
	    UsrValue = readl(uart_port0.vir_addr_base + DW_UART_TFL);
        if (UsrValue < FIFO_TX_SIZE)
        {
             writel(*(buf++), uart_port0.vir_addr_base + DW_UART_THR);
             
#ifdef MSP_IF_IOCTL_TEST
             if(printkflag == 1)
             {

                 (void)printk("%x ",(BSP_U8)(*(buf-1)));
             }
#endif 
             --Cnt;
        }
    }while (Cnt > 0); 
//    (void)printk("\n");
	return BSP_OK;
}/*lint !e529*/
/*****************************************************************************
* �� �� ��  : uart_send
*
* ��������  : uart �ײ㷢�ͺ���
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR
*
*****************************************************************************/

BSP_S32 uart_send(BSP_S32 s32uart_dev_id, BSP_U8 *pbuf,BSP_U32 size)
{
     /*���е�д*/
     UART_CTX_S *p_uart_ctx = (UART_CTX_S *)s32uart_dev_id;
	 BSP_U8 *p_uart_tx_buf  = (BSP_U8 *)pbuf;
     /*�жϲ�����Ч��*/
     if (NULL == p_uart_tx_buf || NULL == p_uart_ctx)
     {
         (void)printk("the parm is error\n");
         return BSP_ERROR;
	 }
     /*******���з��ͣ��뿴�����ʵ��        *********/
     if(uart_tx_chars(p_uart_tx_buf,size)!= BSP_OK)
     {
         (void)printk("uart_tx_chars is FAILED,line is\n");
         return BSP_ERROR;
     }
     return ((BSP_S32)size);
}
/*****************************************************************************
* �� �� ��  : uart_write
*
* ��������  : uart �ײ��ʼ������
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR
*
*****************************************************************************/

BSP_S32 uart_init(void)
{
    BSP_U32 lcr_reg = 0;
    BSP_U32 lcr_value = 0;
    BSP_U32 enbaud   = 0;
    BSP_S32 ret      = 0;
    struct uart_port * pst_tty = NULL;

    pst_tty = (struct uart_port *)get_amba_ports();

    free_irq(pst_tty->irq,pst_tty);
    (void)printk("free_ok\n");
    
	spin_lock_init(&uart_port0.lock_irq);
    
	/*UART0���жϺ���102�ͻ���ַ0x90007000*/
	uart_port0.irq    = INT_UART0_IRQ;    
    uart_port0.phy_addr_base = UART0_PHY_ADDR_BASE;

    
	uart_port0.vir_addr_base = ioremap(uart_port0.phy_addr_base,SIZE_4K);
    
	/* ʹ�� DLL �� DLH ��д���*/
    enbaud = enbaud | DW_UART_DLAB;     
    (void)writel(enbaud,uart_port0.vir_addr_base + DW_UART_LCR);
    /* ���ò����� */
    (void)writel( DLL_BUAD_PARA, uart_port0.vir_addr_base + DW_UART_DLL);
    (void)writel( DLH_BUAD_PARA, uart_port0.vir_addr_base + DW_UART_DLH);  
    /*�ر�DLL��DLH�����*/
    enbaud &= ~DW_UART_DLAB;
    (void)writel(enbaud, uart_port0.vir_addr_base + DW_UART_LCR);

	/*LCR����****���ݳ���8λ+   ֹͣ2λ  +  У��ʹ�� + żУ��*/
    lcr_reg = lcr_reg | DW_UART_8bit;/*3*/
    #if 0
    lcr_reg = DW_UART_STOP;/*4*/
    lcr_reg = DW_UART_PEN; /*8*/
    lcr_reg = DW_UART_EPS; /*16*/
    #endif
    (void)writel(lcr_reg, uart_port0.vir_addr_base + DW_UART_LCR);
    lcr_value = readl(uart_port0.vir_addr_base + DW_UART_LCR);
    (void)printk("the lcr_reg is %x\n",lcr_value);

	/* ��ֹ FIFO���ж� */
    (void)writel(0,uart_port0.vir_addr_base + DW_UART_IER);
	//(void)writel(DW_FCR_PARA, uart_port0.vir_addr_base + DW_UART_FCR);

    //writel(0x0, uap->port.membase + DW_UART_FCR);	
	/* enable FIFO */
	(void)writel(0x1, uart_port0.vir_addr_base + DW_UART_FCR);
	/*set fifo trigger level*/
    (void)writel(0xb1,uart_port0.vir_addr_base + DW_UART_FCR);
	/*clear and reset fifo*/
    (void)writel(0xb7,uart_port0.vir_addr_base + DW_UART_FCR);
	
	/* clear line interrupt status */
    (void)readl(uart_port0.vir_addr_base + DW_UART_LSR);
	
	/*clear rcv interrupt*/
    (void)readl(uart_port0.vir_addr_base + DW_UART_RBR);
	
	/*clear iir reg*/
    (void)readl(uart_port0.vir_addr_base + DW_UART_IIR);	
	
    /*clear line busy interrupt*/
    (void)readl(uart_port0.vir_addr_base + DW_UART_USR);	
    
    /*����ʹ���жϴ���*/
	ret = request_irq(uart_port0.irq, (irq_handler_t)uart_int, 0, "BalongV?R? ISR", NULL);/*lint !e732 */
    if (ret)
	{
        (void)printk("request_irq is failed\n");
        return (BSP_S32)BSP_ERROR;
    }
    
    /*��ʼ�������ź���*/
    sema_init(&(uart_port0.uart_rx_sem), SEM_EMPTY);
    
	/*���������߳�*/
    if (!g_pstUartReadThread) 
    {
       g_pstUartReadThread = kthread_run(uart_recv_thread, NULL, "UART_RX_KTHREAD");
       ret = IS_ERR(g_pstUartReadThread) ? PTR_ERR(g_pstUartReadThread) : 0;
       if (ret)
       {
           (void)printk("kthread_run is failed!\n");
           g_pstUartReadThread = NULL;
           return BSP_ERROR;
       }
    }
	/*ʹ�� FIFO */
	/*0b 1000 0001����ˮ����1/2�����*/
	/*�����������ó�ʱ�жϣ�оƬ�������*/
    /*(void)writel(DW_FCR_PARA, uart_port0.vir_addr_base + DW_UART_FCR);*/

    (void)readl(uart_port0.vir_addr_base + DW_UART_LSR);

    (void)writel(ABLE_RX_IER, uart_port0.vir_addr_base  + DW_UART_IER);

	return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : uart_write
*
* ��������  : uart �ײ��ʼ������
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : BSP_ERROR
*
*****************************************************************************/

void uart_uninit(void)
{
	/* ��ֹ FIFO���ж� */
    (void)writel(0,uart_port0.vir_addr_base + DW_UART_IER);
	(void)writel(0,uart_port0.vir_addr_base + DW_UART_FCR); 
    /*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    free_irq((unsigned int)(uart_port0.irq),NULL);
	/*end*/
}
 
/**********************��������*******************/
/****************������UT��ST��������*************/

void set_rx_buf(BSP_U32 rx_num, BSP_U32 rx_size)
{
    g_rx_num = rx_num;
    g_rx_size =  rx_size;

}
void sel_uart_func( BSP_U8 value)
{
    sel_uart = value;
}

/**********************��������*******************/
void writecharb(BSP_U8 value)
{
    (void)writel(value, uart_port0.vir_addr_base + DW_UART_THR);    
}

void writecharbb(BSP_U8 value)
{
    (void)writeb(value, uart_port0.vir_addr_base + DW_UART_THR);    
}

void writecharl(int value)
{
    (void)writel(value, uart_port0.vir_addr_base + DW_UART_THR);    
}

/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
void set_buf(BSP_U32 value)
{
    buf_look = value;
}
/*end*/


void set_lcr(int value)
{
   (void) writel(value, uart_port0.vir_addr_base + DW_UART_LCR);
}

void get_lcr(void)
{
    BSP_U32 temp_lcr = readl(uart_port0.vir_addr_base + DW_UART_LCR);
    (void)printk("the lcr value is 0x%x\n",temp_lcr);
}

void get_ier(void)
{
    BSP_U32 i = readl(uart_port0.vir_addr_base + DW_UART_IER);
    (void)printk("\ni:0x%x\n",i);
    
}
void get_reg(void)
{
    BSP_U32 reg_lcr_value = 0;
    BSP_U32 reg_fcr_value = 0;
    BSP_U32 reg_ier_value = 0;
    reg_lcr_value = readl(uart_port0.vir_addr_base + DW_UART_LCR);
    reg_fcr_value = readl(uart_port0.vir_addr_base + DW_UART_FCR);
    reg_ier_value = readl(uart_port0.vir_addr_base + DW_UART_IER);

   (void)printk("LCR:0x%x\n",reg_lcr_value);
    (void)printk("FCR:0x%x\n",reg_fcr_value);
    (void)printk("IER:0x%x\n",reg_ier_value);
}

void set_addr_base(BSP_U32 addr)
{
    uart_port0.phy_addr_base = addr;
    uart_port0.vir_addr_base = ioremap(uart_port0.phy_addr_base,4096);
}

void set_reg_value(BSP_U32 offset,BSP_U32 value)
{
     (void)writel(value,uart_port0.vir_addr_base + value);
}

BSP_U32 get_reg_value(BSP_U32 offset)
{
     return(readl(uart_port0.vir_addr_base + offset));    
}


void test_uart_send(BSP_U8 *buf)
{
    (void)uart_send((BSP_S32)(&UartCtx),test_buf,5);
}

/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
void set_dbglvl(BSP_U32 flag1,BSP_U32 flag2)
{
    printkflag = flag1;
    intflag    = flag2;
}
/*end*/

void buf_info(void)
{
    UART_RX_BUF_S * pTmp = UartCtx.pHead_RxQueue;
    int i = 0;
    for(i = 0;i < (BSP_S32)g_rx_num;i++)
    {
        if (i%10 == 0)
        {
             (void)printk("\n"); 
        }
        (void)printk("pTmp->ulSize : %4d",pTmp->ulSize);
        pTmp = pTmp->pstNext;
    }
}

void uart_test_case(int ulcase)
{
     switch(ulcase)
     {
        case 1:/*UT BSP_UdiAdpUartInit*/
        {
              (void)printk("in case BSP_UdiAdpUartInit\n");
             (void)BSP_UdiAdpUartInit();
        }
            break;
        case 2:/*UT �ײ㺯���ķ���*/
        {
              test_uart_send(test_buf);
        }
            break;
        case 3:
        {
             (void)printk("in case BSP_UdiAdpUartInit\n");
            (void)BSP_UdiAdpUartUnInit();
            
        }
            break;
        default:
            (void)printk("error\n");
            break;
     }
    return;
}
