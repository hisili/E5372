/******************************************************************
* Copyright (C), 2005-2011, HISILICON Tech. Co., Ltd.             *
*                                                                 *
* File name: edmaDrv.c                                            *
*                                                                 *
* Description:                                                    *
*     EDMAC LINUX Driver                                          *
*                                                                 *
* Author:  yangcheng                                              *
*                                                                 *
* Version: v0.1                                                   *
*                                                                 *
* Function List:                                                  *
*                                                                 *
* Date:                                                           *
*                                                                 *
* History:                                                        *
*                                                                 *
*          Init.                                                  *
*******************************************************************/
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <asm/io.h>

#include <linux/scatterlist.h>
#include <linux/dma-mapping.h>

#include <mach/edmacIP.h>
#include <mach/edmacDrv.h>


#define OSDRV_MODULE_VERSION_STRING  "ANDROID_2_6_25-EDMAC @HiV3R2 SFT ACPU"
#define DRIVER_NAME  "edmac_device"

/* ��Ӧλ��1��������A����֧�ֵ�ͨ���ж� */
#define CHANNEL_INT_MASK     (0x301c)

/* edmac virtual base address */
void __iomem *g_edmac_base_addr = NULL;

/*  the edmac register pointer */
LOCAL EDMAC_REG_STRU *s_pstEDMACReg = NULL;

/*��ͨ����������ź���*/
struct completion edma_trans_complete[EDMAC_MAX_CHANNEL];

#define hiedmac_trace(level, msg...) do { \
	if ((level) >= HIDMAC_TRACE_LEVEL) { \
		printk("hiedmac_trace:%s:%d:", __FUNCTION__, __LINE__); \
		printk(msg); \
	} \
}while(0)	

/* The transfer status of each channel */
LOCAL UINT32 s_ulChannelStatus[ EDMAC_MAX_CHANNEL ] = { 0 };/*lint !e551 */

/*dma Call back Function*/
LOCAL Chan_Isr_Stru s_DmaTc1Isr[ EDMAC_MAX_CHANNEL ];
LOCAL Chan_Isr_Stru s_DmaTc2Isr[ EDMAC_MAX_CHANNEL ];
LOCAL Chan_Isr_Stru s_DmaErr1Isr[ EDMAC_MAX_CHANNEL ];
LOCAL Chan_Isr_Stru s_DmaErr2Isr[ EDMAC_MAX_CHANNEL ];
LOCAL Chan_Isr_Stru s_DmaErr3Isr[ EDMAC_MAX_CHANNEL ];

/*******************************************************************************
  ������:     LOCAL void edma_channel_isr_init(void)
  ��������:   ��ʼ������ͨ�����жϻص�����
  �������:   ��
  �������:   ��
  ����ֵ:     ��
*******************************************************************************/
LOCAL void edma_channel_isr_init(void)
{
    UINT32 index;
    for (index = 0; index < EDMAC_MAX_CHANNEL; index++)
    {
        s_DmaTc1Isr[index].chan_isr = NULL;
        s_DmaTc1Isr[index].chan_arg = 0;
        s_DmaTc1Isr[index].int_status = 0;
        
        s_DmaTc2Isr[index].chan_isr = NULL;
        s_DmaTc2Isr[index].chan_arg = 0;
        s_DmaTc2Isr[index].int_status = 0;
        
        s_DmaErr1Isr[index].chan_isr = NULL;
        s_DmaErr1Isr[index].chan_arg = 0;
        s_DmaErr1Isr[index].int_status = 0;

        s_DmaErr2Isr[index].chan_isr = NULL;
        s_DmaErr2Isr[index].chan_arg = 0;
        s_DmaErr2Isr[index].int_status = 0;

        s_DmaErr3Isr[index].chan_isr = NULL;
        s_DmaErr3Isr[index].chan_arg = 0;
        s_DmaErr3Isr[index].int_status = 0;
        
        s_ulChannelStatus[index] = DMA_SUCCESS;
        
    }
}

/*******************************************************************************
  ������:      LOCAL void edma_isr_install(UINT32 channel_id, 
                       channel_isr pFunc, UINT32 channel_arg, UINT32 int_flag)
  ��������:    ����int_flag��ע��ͨ����Ӧ���жϻص�����
  �������:    channel_id : ͨ��ID������balong_dma_channel_init�����ķ���ֵ
               pFunc : �����ߴ�����ͨ���жϻص�����
               channel_arg : pFunc�����1
               int_flag : pFunc�����2, �ж����ͣ������
  �������:    ��
  ����ֵ:      ��
*******************************************************************************/
LOCAL void edma_isr_install(UINT32 channel_id, channel_isr pFunc, UINT32 channel_arg, UINT32 int_flag)
{   
    /*0x1F,��5λȫ1����Ӧ5���ж�����*/
    UINT32 int_judge = 0x1F & int_flag;
    if (channel_id < EDMAC_MAX_CHANNEL)
    {
        if (NULL == pFunc)
        {
            s_DmaTc1Isr[channel_id].chan_isr = (channel_isr)NULL;
            s_DmaTc1Isr[channel_id].chan_arg = channel_arg;
            s_DmaTc1Isr[channel_id].int_status = int_flag;
            hiedmac_trace(1,"balong_dma_isr is NULL!\n");
        }
        else
        {
            if (int_judge & 1UL)
            {
                s_DmaTc1Isr[channel_id].chan_isr = (channel_isr)pFunc;
                s_DmaTc1Isr[channel_id].chan_arg = channel_arg;
                s_DmaTc1Isr[channel_id].int_status = int_flag;
            }
            if (int_judge & (1UL << 1))
            {
                s_DmaTc2Isr[channel_id].chan_isr = (channel_isr)pFunc;
                s_DmaTc2Isr[channel_id].chan_arg = channel_arg;
                s_DmaTc2Isr[channel_id].int_status = int_flag;
            }
            if (int_judge & (1UL << 2))
            {
                s_DmaErr1Isr[channel_id].chan_isr = (channel_isr)pFunc;
                s_DmaErr1Isr[channel_id].chan_arg = channel_arg;
                s_DmaErr1Isr[channel_id].int_status = int_flag;
            }
            if (int_judge & (1UL << 3))
            {
                s_DmaErr2Isr[channel_id].chan_isr = (channel_isr)pFunc;
                s_DmaErr2Isr[channel_id].chan_arg = channel_arg;
                s_DmaErr2Isr[channel_id].int_status = int_flag;
            }
             if (int_judge & (1UL << 4))
            {
                s_DmaErr3Isr[channel_id].chan_isr = (channel_isr)pFunc;
                s_DmaErr3Isr[channel_id].chan_arg = channel_arg;
                s_DmaErr3Isr[channel_id].int_status = int_flag;
            }
            hiedmac_trace(1,"balong_dma_isr_install!\n");
        }
    }
    else
    {
        hiedmac_trace(1,"DMA_CHANNEL_INVALID\n");
    }
}

/*******************************************************************************
  ������:      LOCAL void edmac_ip_init(void)
  ��������:    ��ʼ�����жϼĴ������ж����μĴ���
  �������:    ��
  �������:    ��
  ����ֵ:      ��
*******************************************************************************/
LOCAL void edmac_ip_init(void)
{
    /* Clear interrupt */
    s_pstEDMACReg->ulIntTC1Raw                      = CHANNEL_INT_MASK;//0xFFFFFFFF;
    s_pstEDMACReg->ulIntTC2Raw                      = CHANNEL_INT_MASK;
    s_pstEDMACReg->ulIntERR1Raw                     = CHANNEL_INT_MASK;    
    s_pstEDMACReg->ulIntERR2Raw                     = CHANNEL_INT_MASK;
	s_pstEDMACReg->ulIntERR3Raw                     = CHANNEL_INT_MASK;

    /* Enable all interrupt */
    s_pstEDMACReg->stCpuXReg[EDMAC_DT_ARM].ulIntTC1Mask   = CHANNEL_INT_MASK;
    s_pstEDMACReg->stCpuXReg[EDMAC_DT_ARM].ulIntTC2Mask   = CHANNEL_INT_MASK; 
	s_pstEDMACReg->stCpuXReg[EDMAC_DT_ARM].ulIntErr1Mask  = CHANNEL_INT_MASK;
    s_pstEDMACReg->stCpuXReg[EDMAC_DT_ARM].ulIntErr2Mask  = CHANNEL_INT_MASK; 
	s_pstEDMACReg->stCpuXReg[EDMAC_DT_ARM].ulIntErr3Mask  = CHANNEL_INT_MASK; 
}

/*******************************************************************************
  ������:      LOCAL int edma_m2m_channel_request(void)
  ��������:    �ڴ������ͨ�����䣬ͨ��12��13֮һ
  �������:    ��
  �������:    ��
  ����ֵ:      �ɹ���ͨ����
               ʧ�ܣ�����
*******************************************************************************/
LOCAL int edma_m2m_channel_request(void)
{
    int ret = 0;
    if (EDMA_CHN_FREE == balong_dma_channel_is_idle(12))
    {
        ret = 12;
    }
    else if (EDMA_CHN_FREE == balong_dma_channel_is_idle(13))
    {
        ret = 13;
    }
    else
    {
        ret = -1;
    }
    return ret;
}

 void edmac_int (void)
 {
    edma_channel_isr_init();
    edmac_ip_init();
 }    
 
/*******************************************************************************
  ������:      int balong_dma_channel_init (BALONG_DMA_REQ req, 
                          channel_isr channel_isr, UINT32 channel_arg, 
                          UINT32 int_flag)
  ��������:    ��������ŷ���ͨ����ע��ͨ���жϻص���������ʼ����������ź�����
               �������д��config�Ĵ���
  �������:    req : ���������
               channel_isr : �ϲ�ģ��ע���DMAͨ���жϴ�������NULLʱ������ע��
               channel_arg : channel_isr�����1��
                             channel_isrΪNULL������Ҫ�����������
               int_flag : channel_isr�����2, �������ж����ͣ�ȡֵ��ΧΪ
                        BALONG_DMA_INT_DONE��BALONG_DMA_INT_LLT_DONE��
                      �BALONG_DMA_INT_CONFIG_ERR��BALONG_DMA_INT_TRANSFER_ERR��
                        BALONG_DMA_INT_READ_ERR֮һ��������ϡ�
                        channel_isrΪNULL������Ҫ�����������
  �������:    ��
  ����ֵ:      �ɹ���ͨ����
               ʧ�ܣ�����
*******************************************************************************/
int balong_dma_channel_init (BALONG_DMA_REQ req, channel_isr channel_isr, UINT32 channel_arg, UINT32 int_flag)
{ /*lint !e578*/
    int ret = -1;
    switch (req)
    {
        case EDMA_MMC0:
            ret = 4;
            break;
        case EDMA_MMC1: 
            ret = 3;
            break;
        case EDMA_MMC2:
            ret = 2;
            break;  
        case EDMA_MEMORY:
            ret = edma_m2m_channel_request();
            break;
        default:
            ret = -1;
            break;
    }
      
    if (ret >= 0)
    {
    	/*begin  lint -Info 732: Loss of sign (arg. no. 1) (int to unsigned int)   */
        edma_isr_install((unsigned int)ret, channel_isr, channel_arg, int_flag);
		/*end*/
        init_completion(&edma_trans_complete[ret]);
        hiedmac_trace(1,"edma_trans_complete sem is inited!\n");

        writel(EDMAC_TRANSFER_CONFIG_REQUEST(req),(g_edmac_base_addr + 0x81c + 0x40 * ret));    
        hiedmac_trace(1,"peri_id %d has been added to config reg!\n",ret);  
    }
    return ret;   
}

/*******************************************************************************
  ������:       int balong_dma_current_transfer_address(UINT32 channel_id)
  ��������:     ���ĳͨ����ǰ������ڴ��ַ
  �������:     channel_id : ͨ��ID������balong_dma_channel_init�����ķ���ֵ
  �������:     ��
  ����ֵ:       �ɹ���ͨ����ǰ������ڴ��ַ
                ʧ�ܣ�����
*******************************************************************************/
int balong_dma_current_transfer_address(UINT32 channel_id)
{
    UINT32 ulGetAddrType = 0;
    int ret_addr = DMA_CHANNEL_INVALID;
    
    /*��ѯconfig�Ĵ����ĸ���λ��Դ��Ŀ�ĵ�ַΪ�ڴ�ʱΪ1*/
    if (channel_id < EDMAC_MAX_CHANNEL)
    {
        ulGetAddrType = EDMAC_CHANNEL_CB(channel_id).config & 0xc0000000;
        switch (ulGetAddrType)
        {
            case 0xc0000000:
            case 0x80000000:
                ret_addr = (UINT32)(s_pstEDMACReg->stCurrStatusReg[channel_id].ulCxCurrSrcAddr); /*lint !e713*/
                break;
            case 0x40000000:
                ret_addr = (UINT32)(s_pstEDMACReg->stCurrStatusReg[channel_id].ulCxCurrDesAddr); /*lint !e713*/
                break;
            default:
                break;
        }
    }
    return ret_addr;
}

/*******************************************************************************
  ������:       int balong_dma_channel_stop(UINT32 channel_id)
  ��������:     ָֹͣ����DMAͨ��
  �������:     channel_id : ͨ��ID������balong_dma_channel_init�����ķ���ֵ
  �������:     ��
  ����ֵ:       �ɹ���ͨ����ǰ������ڴ��ַ
                ʧ�ܣ�����
*******************************************************************************/
int balong_dma_channel_stop(UINT32 channel_id)
{
	/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
    int curr_addr = 0;
	/*end*/
    if (channel_id < EDMAC_MAX_CHANNEL)
    {
        s_pstEDMACReg->stTransferConfig[channel_id].lli_node_info.config &= ~(EDMAC_CHANNEL_ENABLE);
        curr_addr = balong_dma_current_transfer_address(channel_id);
        return curr_addr;
    }
    else
    {
	    hiedmac_trace(1, "Channel id is wrong!\n");
        return DMA_CHANNEL_INVALID;
    }
}

/*******************************************************************************
  ������:       int balong_dma_channel_is_idle (UINT32 channel_id)
  ��������:     ��ѯDMAͨ���Ƿ����
  �������:     channel_id : ͨ��ID������balong_dma_channel_init�����ķ���ֵ
  �������:     ��
  ����ֵ:       0 : ͨ��æµ
                1 : ͨ������
                ���� : ʧ��
*******************************************************************************/
int balong_dma_channel_is_idle (UINT32 channel_id)
{
    UINT32  ChannelState = 0;

    if (channel_id < EDMAC_MAX_CHANNEL)
    {
        ChannelState = s_pstEDMACReg->ulChannelState;
        if ((1UL << channel_id) & ChannelState)
        {
            return EDMA_CHN_BUSY;    /* The channel is active    */
        }
        else
        {
            return EDMA_CHN_FREE;    /* The channel is idle      */
        }
    }
    else
    {
        return DMA_CHANNEL_INVALID;
    }
}

/*******************************************************************************
  ������:       LOCAL irqreturn_t balong_dma_isr(int irq, void *private)
  ��������:     DMA���������жϷ�������жϲ���ʱ�����ã������ж����ĸ�
                ͨ���������жϣ�������Ӧͨ�����жϴ������
  �������:     irq : �жϺţ�����request_irq����ע���жϷ������ʱ��req����ֵ
                private : ����request_irq����ע���жϷ������ʱ��dev����ֵ
  �������:     ��
  ����ֵ:       IRQ_HANDLED ���ж��Ѿ�������
*******************************************************************************/
LOCAL irqreturn_t balong_dma_isr(int irq, void *private)
{
    UINT32 ulChannelIntStatus       = s_pstEDMACReg->stCpuXReg[EDMAC_DT_ARM].ulIntState;    /* Read the current interrupt status */
    UINT32 ulChannelIntTC1Status    = s_pstEDMACReg->stCpuXReg[EDMAC_DT_ARM].ulIntTC1;
    UINT32 ulChannelIntTC2Status    = s_pstEDMACReg->stCpuXReg[EDMAC_DT_ARM].ulIntTC2;
    UINT32 ulChannelIntErr1Status   = s_pstEDMACReg->stCpuXReg[EDMAC_DT_ARM].ulIntErr1;
    UINT32 ulChannelIntErr2Status   = s_pstEDMACReg->stCpuXReg[EDMAC_DT_ARM].ulIntErr2;
    UINT32 ulChannelIntErr3Status   = s_pstEDMACReg->stCpuXReg[EDMAC_DT_ARM].ulIntErr3;
    UINT32 ulCurrentBit             = 0;
    int    i;
    
    hiedmac_trace(5,"%s ----ulChannel_Int_Status_Reg = 0x%X \n", __func__, ulChannelIntStatus);
    /* decide which channel has trigger the interrupt*/
    for (i = 0; i < EDMAC_MAX_CHANNEL; i++)
    {
        ulCurrentBit = 1UL << i;
        if (ulChannelIntStatus & ulCurrentBit)
        {       
            s_ulChannelStatus[i] = (UINT32)DMA_TRXFER_ERROR;
                   
            if (ulChannelIntTC1Status & ulCurrentBit)
            {
                /*save the current channel transfer status to g_ulChannelStatus[i]*/
                s_ulChannelStatus[i] = DMA_SUCCESS;

                /* Clear TC / Link_TC Interrupt ��ǰͨ��ԭʼ�ж�*/
                s_pstEDMACReg->ulIntTC1Raw = ulCurrentBit;
	            if(s_DmaTc1Isr[i].chan_isr != NULL)
                {
                    (s_DmaTc1Isr[i].chan_isr)(s_DmaTc1Isr[i].chan_arg, s_DmaTc1Isr[i].int_status);
                }
                else
                {
                    /*�ͷŵ�ǰͨ����������ź���*/
                    complete(&edma_trans_complete[i]);
                }
                hiedmac_trace(5, "channel transfer success!\n");
            }
            else if (ulChannelIntTC2Status & ulCurrentBit)
            {
                s_ulChannelStatus[i] = DMA_SUCCESS;
                s_pstEDMACReg->ulIntTC2Raw = ulCurrentBit;
                if(s_DmaTc2Isr[i].chan_isr != NULL)
                {
                    (s_DmaTc2Isr[i].chan_isr)(s_DmaTc2Isr[i].chan_arg, s_DmaTc2Isr[i].int_status);
                }
				hiedmac_trace(5, "LLI transfer success!\n");
			}          
            else if (ulChannelIntErr1Status & ulCurrentBit)
            {
                s_ulChannelStatus[i] = (UINT32)DMA_TRXFER_ERROR;
				/* Clear the Error / Link_Error interrupt */
				s_pstEDMACReg->ulIntERR1Raw = ulCurrentBit;
                if(s_DmaErr1Isr[i].chan_isr != NULL)
                {
                    (s_DmaErr1Isr[i].chan_isr)(s_DmaErr1Isr[i].chan_arg, s_DmaErr1Isr[i].int_status);
                }
				hiedmac_trace(5, "channel %d transfer config_error1!\n", i);
            }
			else if (ulChannelIntErr2Status & ulCurrentBit)
			{
				s_ulChannelStatus[i]             = (UINT32)DMA_TRXFER_ERROR;
				/* Clear the Error / Link_Error interrupt */
				s_pstEDMACReg->ulIntERR2Raw        = ulCurrentBit;
                if(s_DmaErr2Isr[i].chan_isr != NULL)
                {
                    (s_DmaErr2Isr[i].chan_isr)(s_DmaErr2Isr[i].chan_arg, s_DmaErr2Isr[i].int_status);
                }
				hiedmac_trace(5, "channel %d transfer data_error2\n", i);
			}
			else if (ulChannelIntErr3Status & ulCurrentBit)
			{
                s_ulChannelStatus[i]             = (UINT32)DMA_TRXFER_ERROR;
				/* Clear the Error / Link_Error interrupt */
				s_pstEDMACReg->ulIntERR3Raw    = ulCurrentBit;
                if(s_DmaErr3Isr[i].chan_isr != NULL)
                {
                    (s_DmaErr3Isr[i].chan_isr)(s_DmaErr3Isr[i].chan_arg, s_DmaErr3Isr[i].int_status);
                }
                hiedmac_trace(5, "channel %d transfer read_lli_error3!\n", i);
            }
            else
            {
                hiedmac_trace(5,"Error in balong_dma_isr!\n");
				return IRQ_HANDLED; 
            }  /* Handle this interrupt */             
        } 
    }/*end of for(i=0,i< DMA_MAX_CHANNELS;i++)*/
	return IRQ_HANDLED;
}

/*******************************************************************************
  ������:      int balong_dma_channel_set_config (UINT32 channel_id,
                       UINT32 direction, UINT32 burst_width, UINT32 burst_len)
  ��������:    ����ʽDMA����ʱ�����ñ���������ͨ��������
               ��ʽDMA����ʱ������Ҫʹ�ñ�������
  �������:    channel_id : ͨ��ID������balong_dma_channel_init�����ķ���ֵ
               direction : DMA���䷽��, ȡֵΪBALONG_DMA_P2M��BALONG_DMA_M2P��
                           BALONG_DMA_M2M֮һ
               burst_width��ȡֵΪ0��1��2��3����ʾ��burstλ��Ϊ8��16��32��64bit
               burst_len��ȡֵ��Χ0~15����ʾ��burst����Ϊ1~16
  �������:    ��
  ����ֵ:      �ɹ���0
               ʧ�ܣ�����
*******************************************************************************/
int balong_dma_channel_set_config (UINT32 channel_id, UINT32 direction, UINT32 burst_width, UINT32 burst_len)
{
    UINT32 chanConfig = 0;
    chanConfig = readl(g_edmac_base_addr + 0x81c + 0x40 * channel_id);
    if((channel_id < EDMAC_MAX_CHANNEL) && (burst_width <= 3) && (burst_len <= 15))
    {
        chanConfig &= (UINT32)0x1F0; /*0x1F0,����balong_dma_channel_init����������ã�bit9��δ��*/
        switch (direction)
        {
            case BALONG_DMA_P2M:
                chanConfig |= P2M_CONFIG;
                break;
            case BALONG_DMA_M2P:
                chanConfig |= M2P_CONFIG;
                break;
            case BALONG_DMA_M2M:
                chanConfig |= M2M_CONFIG;
                break;
            default:
                break;
        }
        chanConfig |= EDMAC_BASIC_CONFIG(burst_width, burst_len);
        writel(chanConfig, g_edmac_base_addr + 0x81c + 0x40 * channel_id);
        hiedmac_trace(1,"stChannelConfing->config = 0x%X!\n", chanConfig);
        return DMA_SUCCESS;
    }
    return DMA_CONFIG_ERROR;
}

/*******************************************************************************
  ������:      int balong_dma_channel_start (UINT32 channel_id, UINT32 src_addr, 
                       UINT32 des_addr, UINT32 len)
  ��������:    ����һ��ͬ��DMA����, DMA������ɺ󣬲ŷ���
               ʹ�ñ�����ʱ������Ҫע���жϴ�����
  �������:    channel_id��ͨ��ID,����balong_dma_channel_init�����ķ���ֵ
               src_addr�����ݴ���Դ��ַ�������������ַ
               des_addr�����ݴ���Ŀ�ĵ�ַ�������������ַ
               len�����ݴ��䳤�ȣ���λ���ֽڣ�һ�δ������ݵ���󳤶���65535�ֽ�
  �������:    ��
  ����ֵ:      �ɹ���0
               ʧ�ܣ�����
*******************************************************************************/
int balong_dma_channel_start (UINT32 channel_id, UINT32 src_addr, UINT32 des_addr, UINT32 len)
{
    unsigned long edma_ret = 0; /*clean e713*/
    UINT32 reg_value = 0;
    
    hiedmac_trace(3,"--------------------------edma_start!\n"); 
    reg_value = readl(g_edmac_base_addr + 0x81c + 0x40 * channel_id);
    hiedmac_trace(1,"---------pre stChannelConfing->config = 0x%X\n",reg_value);
  
    if (channel_id < EDMAC_MAX_CHANNEL)
    {
        s_ulChannelStatus[channel_id] = (UINT32)DMA_NOT_FINISHED;

        EDMAC_CHANNEL_CB(channel_id).config  = EDMAC_CHANNEL_DISABLE; 
        
        EDMAC_CHANNEL_CB(channel_id).lli       = 0;
        EDMAC_CHANNEL_CB(channel_id).bindx     = 0;
        EDMAC_CHANNEL_CB(channel_id).cindx     = 0;
        EDMAC_CHANNEL_CB(channel_id).cnt1      = 0;
        EDMAC_CHANNEL_CB(channel_id).cnt0      = len;
        EDMAC_CHANNEL_CB(channel_id).src_addr  = src_addr;
        EDMAC_CHANNEL_CB(channel_id).des_addr  = des_addr;
     
        EDMAC_CHANNEL_CB(channel_id).config   = reg_value | EDMAC_CHANNEL_ENABLE;

        reg_value = readl(g_edmac_base_addr + 0x81c + 0x40 * channel_id);
        hiedmac_trace(1,"--------later stChannelConfing->config = 0x%X\n",reg_value);
        edma_ret = wait_for_completion_timeout(&edma_trans_complete[channel_id], EDMA_DATA_TIMEOUT);
        if (!edma_ret) 
        {
            hiedmac_trace(1,"edma_trans_complete is timeout!\n");
            return DMA_TRXFER_ERROR;
        }
        hiedmac_trace(1,"edma_trans_complete!\n");
        return DMA_SUCCESS;
    }
    else
    {
        hiedmac_trace(1,"DMA_CHANNEL_INVALID!\n");
        return DMA_CHANNEL_INVALID;
    }    
}

/*******************************************************************************
  ������:      int balong_dma_channel_async_start (UINT32 channel_id, 
                unsigned int src_addr, unsigned int des_addr, unsigned int len)
  ��������:    ����һ���첽DMA���䡣����DMA����󣬾ͷ��ء����ȴ�DMA������ɡ�
               ʹ�ñ�����ʱ��ע���жϴ��������жϴ������д���DMA��������¼�
               ���ߣ���ע���жϴ�������ʹ��balong_dma_channel_is_idle������ѯ
               DMA�����Ƿ����
  �������:    channel_id��ͨ��ID,����balong_dma_channel_init�����ķ���ֵ
               src_addr�����ݴ���Դ��ַ�������������ַ
               des_addr�����ݴ���Ŀ�ĵ�ַ�������������ַ
               len�����ݴ��䳤�ȣ���λ���ֽڣ�һ�δ������ݵ���󳤶���65535�ֽ�
  �������:    ��
  ����ֵ:      �ɹ���0
               ʧ�ܣ�����
*******************************************************************************/
int balong_dma_channel_async_start (UINT32 channel_id, unsigned int src_addr, unsigned int des_addr, unsigned int len)
{
    UINT32 reg_value = 0;
    
    hiedmac_trace(3,"--------------------------edma_start!\n");
    reg_value = readl(g_edmac_base_addr + 0x81c + 0x40 * channel_id);
    hiedmac_trace(1,"---------pre stChannelConfing->config = 0x%X\n",reg_value);
    
    if (channel_id < EDMAC_MAX_CHANNEL)
    {
        s_ulChannelStatus[channel_id] = (UINT32)DMA_NOT_FINISHED;

        EDMAC_CHANNEL_CB(channel_id).config  = EDMAC_CHANNEL_DISABLE; 
        
        EDMAC_CHANNEL_CB(channel_id).lli       = 0;
        EDMAC_CHANNEL_CB(channel_id).bindx     = 0;
        EDMAC_CHANNEL_CB(channel_id).cindx     = 0;
        EDMAC_CHANNEL_CB(channel_id).cnt1      = 0;
        EDMAC_CHANNEL_CB(channel_id).cnt0      = len;
        EDMAC_CHANNEL_CB(channel_id).src_addr  = src_addr;
        EDMAC_CHANNEL_CB(channel_id).des_addr  = des_addr;
     
        EDMAC_CHANNEL_CB(channel_id).config   = reg_value | EDMAC_CHANNEL_ENABLE;
     
        return DMA_SUCCESS;
    }
    else
    {
        hiedmac_trace(1,"DMA_CHANNEL_INVALID\n");
        return DMA_CHANNEL_INVALID;
    }
}

/*******************************************************************************
  ������:      BALONG_DMA_CB *balong_dma_channel_get_lli_addr(UINT32 channel_id)
  ��������:    ��ȡָ��DMAͨ����������ƿ����ʼ��ַ
  �������:    channel_id��ͨ��ID,����balong_dma_channel_init�����ķ���ֵ
  �������:    ��
  ����ֵ:      �ɹ���0
               ʧ�ܣ�����
*******************************************************************************/
BALONG_DMA_CB *balong_dma_channel_get_lli_addr (UINT32 channel_id)
{
    if (channel_id < EDMAC_MAX_CHANNEL)
    {
        UINT32 CB_Offset = 0x0800 + 0x40 * channel_id;
        /*BEGIN:y00206456 2012-04-26 Modified for pclint e124*/
        return (BALONG_DMA_CB *)((unsigned long)g_edmac_base_addr + CB_Offset);
        /*END:y00206456 2012-04-26 Modified for pclint e124*/
    }
    else
    {
        return NULL;
    }
}

/*******************************************************************************
  ������:      int balong_dma_channel_lli_start (UINT32 channel_id)
  ��������:    ������ʽDMA���䡣����ʽDMA�����нڵ㴫�䶼ȫ����ɺ�ŷ��ء�
               ��ʽDMA��ÿ���ڵ����������䳤��Ϊ65535�ֽڡ�
               ע�⣺���ô˺���ǰ���������ú�������ƿ顣
  �������:    channel_id��ͨ��ID,����balong_dma_channel_init�����ķ���ֵ
  �������:    ��
  ����ֵ:      �ɹ���0
               ʧ�ܣ�����
*******************************************************************************/
int balong_dma_channel_lli_start (UINT32 channel_id)
{    
    UINT32 reg_value = 0;
    UINT32 edma_ret = 0;
    
    hiedmac_trace(3,"--------------------------edma_start!\n");
    reg_value = readl(g_edmac_base_addr + 0x81c + 0x40 * channel_id);
    hiedmac_trace(1,"-----------pre_stChannelConfing->config = 0x%X\n",reg_value);
    if (channel_id < EDMAC_MAX_CHANNEL)
    {
        s_ulChannelStatus[channel_id] = (UINT32)DMA_NOT_FINISHED; 
        
        EDMAC_CHANNEL_CB(channel_id).config = reg_value | EDMAC_CHANNEL_ENABLE;  

        reg_value = readl(g_edmac_base_addr + 0x81c + 0x40 * channel_id);
        hiedmac_trace(1,"---------later stChannelConfing->config = 0x%X\n",reg_value);
        
        edma_ret = wait_for_completion_timeout(&edma_trans_complete[channel_id], EDMA_DATA_TIMEOUT);
        if (!edma_ret) 
        {
        	hiedmac_trace(1,"edma_trans_complete is timeout!");
            return DMA_FAIL;
        }
        hiedmac_trace(1,"edma_trans_complete!\n");
        return DMA_SUCCESS;
    }
    else
    {
        hiedmac_trace(1,"DMA_CHANNEL_INVALID\n");
        return DMA_CHANNEL_INVALID;
    }    
}

/*******************************************************************************
  ������:      int balong_dma_channel_lli_start (UINT32 channel_id)
  ��������:    ������ʽDMA���䣬Ȼ���������أ����ȴ�DMA������ɡ�
               ��ʽDMA��ÿ���ڵ����������䳤��Ϊ65535�ֽڡ�
               ע�⣺���ô˺���ǰ���������ú�������ƿ顣
  �������:    channel_id��ͨ��ID,����balong_dma_channel_init�����ķ���ֵ
  �������:    ��
  ����ֵ:      �ɹ���0
               ʧ�ܣ�����
*******************************************************************************/
int balong_dma_channel_lli_async_start (UINT32 channel_id)
{
    UINT32 reg_value = 0;
   
    hiedmac_trace(3,"--------------------------edma_start!\n");
    reg_value = readl(g_edmac_base_addr + 0x81c + 0x40 * channel_id);
    hiedmac_trace(1,"-----------pre_stChannelConfing->config = 0x%X\n",reg_value);
    if (channel_id < EDMAC_MAX_CHANNEL)
    {
        s_ulChannelStatus[channel_id] = (UINT32)DMA_NOT_FINISHED; 
        
        EDMAC_CHANNEL_CB(channel_id).config = reg_value | EDMAC_CHANNEL_ENABLE;  

        reg_value = readl(g_edmac_base_addr + 0x81c + 0x40 * channel_id);
        hiedmac_trace(1,"--------later stChannelConfing->config = 0x%X\n",reg_value);
        return DMA_SUCCESS;
    }
    else
    {
        hiedmac_trace(1,"DMA_CHANNEL_INVALID\n");
        return DMA_CHANNEL_INVALID;
    }   
}

#ifdef DMA_TEST
#define DMA_M2M_TEST_LENGHT  (64*1024-1)
char *src_buf = NULL;
char *des_buf = NULL;
dma_addr_t  edma_src_addr  = 0; 
dma_addr_t  edma_des_addr  = 0; 

int edmac_verify_data_blk(char* src_buf, char* des_buf)
{/*lint !e578*/
    int count = DMA_M2M_TEST_LENGHT;
    char *temp_src = src_buf;
    char *temp_des = des_buf;
    while (count--)
    {
        if (*temp_des++ != *temp_src++)
        {
            hiedmac_trace(3,"-----edma_verify_data failed!----index=%d\n", (DMA_M2M_TEST_LENGHT - count));
            return DMA_TRXFER_ERROR;
        }
    }
    hiedmac_trace(1,"%s---des_buf =%c%c%c%c%c%c%c\n", __func__, *((char*)des_buf + 0), 
            *((char*)des_buf + 1), *((char*)des_buf + 2), *((char*)des_buf + 3), 
            *((char*)des_buf + 4), *((char*)des_buf + 5), *((char*)des_buf + 6));
    hiedmac_trace(3,"-----edma_verify_data SUCCESS!\n");
    return DMA_SUCCESS;
}

/*M2Mͬ��DMA���䣬��ע���жϺ������ȴ�EDMA��������ź���*/
int edmac_test_channel(BALONG_DMA_REQ req, UINT32 direction)
{
    int    ret_id = 0;   
    UINT32 bur_width = 2;
    UINT32 bur_len = 3;
    UINT32 byte_len = DMA_M2M_TEST_LENGHT;
    char  *temp = NULL;
    int    count = 0;
    if((NULL == src_buf) || (NULL == des_buf))
    {
        return DMA_FAIL;
    }
    
    temp = src_buf;
    while (count < DMA_M2M_TEST_LENGHT)
    {
        *temp++ = 'a' + count%20;
        ++count;
    }
	src_buf[DMA_M2M_TEST_LENGHT - 1]='\0';

    temp = des_buf;
    count = 0;
    while (count < DMA_M2M_TEST_LENGHT)
	{
		*temp++ = 'b' + count%20;
		++count;
	}
    des_buf[DMA_M2M_TEST_LENGHT - 1]='\0';

    hiedmac_trace(1,"%s---src_buf =%c%c%c%c%c%c%c\n", __func__, *((char*)src_buf + 0), 
                    *((char*)src_buf + 1), *((char*)src_buf + 2), *((char*)src_buf + 3), 
                    *((char*)src_buf + 4), *((char*)src_buf + 5), *((char*)src_buf + 6));
    hiedmac_trace(1,"%s---des_buf =%c%c%c%c%c%c%c\n", __func__, *((char*)des_buf + 0), 
                    *((char*)des_buf + 1), *((char*)des_buf + 2), *((char*)des_buf + 3), 
                    *((char*)des_buf + 4), *((char*)des_buf + 5), *((char*)des_buf + 6));
    
    ret_id = balong_dma_channel_init(req, NULL, 0, 0); 
    if (ret_id < 0)
    {
        hiedmac_trace(1,"-----------error ret_id = 0x%X\n",ret_id);
        return DMA_CHANNEL_INVALID;
    }
    
    //����config�Ĵ��� 
    /*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int)) */
    if (balong_dma_channel_set_config((unsigned int)ret_id, direction, bur_width, bur_len))
    {
        hiedmac_trace(1,"balong_dma_channel_set_config failed!\n");
        return DMA_CONFIG_ERROR;
    }
    /*end*/

	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    if (balong_dma_channel_start((unsigned int)ret_id, (UINT32)edma_src_addr, (UINT32)edma_des_addr , byte_len))
    {
        hiedmac_trace(1,"balong_dma_channel_start FAILED!\n");
        return DMA_TRXFER_ERROR;
    }
    /*end*/
    if(edmac_verify_data_blk(src_buf,des_buf))
    {
        return DMA_TRXFER_ERROR;
    }
    hiedmac_trace(1,"balong_dma_channel_start SUCCESS!\n\n");
    return DMA_SUCCESS;        
}

/*�첽DMA���䣬�ϲ�ģ���ѯDMA�����Ƿ����*/
int edmac_test_channel_async(BALONG_DMA_REQ req, UINT32 direction)
{
    int    ret_id = 0;
    UINT32 bur_width = 2;
    UINT32 bur_len = 3;
    UINT32 ask_is_idle_loop = 0;

    UINT32 byte_len = DMA_M2M_TEST_LENGHT;
    char *temp = NULL;
	int count = 0;
    if ((NULL == src_buf) || (NULL == des_buf))
    {
        return DMA_FAIL;
    }
    
	temp = src_buf;
	while (count < DMA_M2M_TEST_LENGHT)
	{
		*temp++ = 'a' + count%20;
		++count;
	}
	src_buf[DMA_M2M_TEST_LENGHT - 1]='\0';

    temp = des_buf;
    count = 0;
    while (count < DMA_M2M_TEST_LENGHT)
	{
		*temp++ = 'c' + count%20;
		++count;
	}
	des_buf[DMA_M2M_TEST_LENGHT - 1]='\0';
    hiedmac_trace(1,"%s---src_buf =%c%c%c%c%c%c%c\n", __func__, *((char*)src_buf + 0), 
                    *((char*)src_buf + 1), *((char*)src_buf + 2), *((char*)src_buf + 3), 
                    *((char*)src_buf + 4), *((char*)src_buf + 5), *((char*)src_buf + 6));
    hiedmac_trace(1,"%s---des_buf =%c%c%c%c%c%c%c\n", __func__, *((char*)des_buf + 0), 
                    *((char*)des_buf + 1), *((char*)des_buf + 2), *((char*)des_buf + 3), 
                    *((char*)des_buf + 4), *((char*)des_buf + 5), *((char*)des_buf + 6));
    ret_id = balong_dma_channel_init(req, NULL, 0, 0); //�Ƿ�Ҫ��ʼ���жϻص�����?
    if (ret_id < 0)
    {
        hiedmac_trace(1,"-----------error ret_id = 0x%X\n",ret_id);
        return DMA_CHANNEL_INVALID;
    }
    
    //����config�Ĵ���
    /*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    if (balong_dma_channel_set_config((unsigned int)ret_id, direction, bur_width, bur_len))
    {
        hiedmac_trace(1,"balong_dma_channel_set_config failed!\n");
        return DMA_CONFIG_ERROR;
    }
    /*end*/

	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    if (balong_dma_channel_async_start((unsigned int)ret_id, (UINT32)edma_src_addr, (UINT32)edma_des_addr, byte_len))
    {
        hiedmac_trace(1,"balong_dma_channel_async_start failed!\n");
        return DMA_CONFIG_ERROR;
    }
    /*end*/
	
    for (;;)
    {
        hiedmac_trace(1,"--------------------pool channel async start!");
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
        if (!balong_dma_channel_is_idle((unsigned int)ret_id))        
		/*end*/
		{
            ask_is_idle_loop++;
            ssleep(2);
            if (ask_is_idle_loop < 0x100000)
            {
                continue;
            }
            else
            {
                hiedmac_trace(1,"edma_trans_complete is timeout!");
                return DMA_TRXFER_ERROR;
            }
        }
        else
        {
            hiedmac_trace(1,"edma_trans_complete-----ask_is_idle_loop=0x%X!\n",ask_is_idle_loop);
            if (edmac_verify_data_blk(src_buf,des_buf))
            {
                return DMA_TRXFER_ERROR;
            }
            hiedmac_trace(1,"DMA_CHANNEL_ASYNC_TEST_SUCCESS!\n\n");
            return DMA_SUCCESS;
        }
    } 
}

void data_trans_complete(UINT32 channel_arg, UINT32 int_status)
{
    hiedmac_trace(1,"edma_trans_complete!\n");
    hiedmac_trace(1,"DMA_CHANNEL_TEST_SUCCESS!\n");
}
    
/*�첽DMA���䣬�ϲ�ģ��ע���жϴ�����򣬴���DMA��������¼�*/
int edmac_test_channel_async_int(BALONG_DMA_REQ req, UINT32 direction)
{
    int    ret_id = 0;
    UINT32 bur_width = 2;
    UINT32 bur_len = 3;

    UINT32 byte_len = DMA_M2M_TEST_LENGHT;
    char *temp = NULL;
	int count = 0;
    if ((NULL == src_buf) || (NULL == des_buf))
    {
        return DMA_FAIL;
    }
    
	temp = src_buf;
	while (count < DMA_M2M_TEST_LENGHT)
	{
		*temp++ = 'a' + count%20;
		++count;
	}
	src_buf[DMA_M2M_TEST_LENGHT - 1]='\0';
    
    temp = des_buf;
    count = 0;
    while (count < DMA_M2M_TEST_LENGHT)
	{
		*temp++ = 'd' + count%20;
		++count;
	}
	des_buf[DMA_M2M_TEST_LENGHT - 1]='\0';
    hiedmac_trace(1,"%s---src_buf =%c%c%c%c%c%c%c\n", __func__, *((char*)src_buf + 0), 
                    *((char*)src_buf + 1), *((char*)src_buf + 2), *((char*)src_buf + 3), 
                    *((char*)src_buf + 4), *((char*)src_buf + 5), *((char*)src_buf + 6));
    hiedmac_trace(1,"%s---des_buf =%c%c%c%c%c%c%c\n", __func__, *((char*)des_buf + 0), 
                    *((char*)des_buf + 1), *((char*)des_buf + 2), *((char*)des_buf + 3), 
                    *((char*)des_buf + 4), *((char*)des_buf + 5), *((char*)des_buf + 6));
    
    ret_id= balong_dma_channel_init(req, (channel_isr)data_trans_complete, 0, BALONG_DMA_INT_DONE); //�Ƿ�Ҫ��ʼ���жϻص�����?
    if (ret_id < 0)
    {
        hiedmac_trace(1,"-----------error ret_id = 0x%X\n",ret_id);
        return DMA_CHANNEL_INVALID;
    }
    
    //����config�Ĵ��� 
    /*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    if (balong_dma_channel_set_config((unsigned int)ret_id, direction, bur_width, bur_len))
	/*end*/
    {	
    	hiedmac_trace(1,"balong_dma_channel_set_config failed!\n");
		return DMA_CONFIG_ERROR;
    }
    /*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    if (balong_dma_channel_async_start((unsigned int)ret_id, (UINT32)edma_src_addr, (UINT32)edma_des_addr, byte_len))
	/*end*/
	{
        hiedmac_trace(1,"DMA_CHANNEL_TEST_FAILED!\n");
        return DMA_TRXFER_ERROR;
    }
    /*Warning 648: (Warning -- Overflow in computing constant for operation:
         'unsigned multiplication')*/
	/*lint -e648*/
    mdelay(200);/*lint !e62*/
	/*lint +e648*/
    if (edmac_verify_data_blk(src_buf,des_buf))
    {
        return DMA_TRXFER_ERROR;
    }
    hiedmac_trace(1,"DMA_CHANNEL_ASYNC_INIT_TEST_SUCCESS!\n\n");
    return DMA_SUCCESS;       
}

/*M2M������test*/
#define EDMA_TEST_LLI_NUM               (7)
#define EDMA_TEST_LLI_BLOCK_SIZE        (1024 * 64 - 1)

LOCAL EDMA_LLI_ALLOC_ADDRESS_STRU  s_edma_alloc_address  = {0, 0};

DMA_SIMPLE_LLI_STRU  stDmaLLI[EDMA_TEST_LLI_NUM];
UINT8 *              pucSour        = NULL;
UINT8 *              pucDest        = NULL;  
dma_addr_t           edma_buf_phys  = 0;

void edma_clear_data( UINT8 *pucBuffer, UINT32 ulSize )
{
	if (pucBuffer && ulSize)
    {
        while( ulSize-- )
        {
            *pucBuffer = 0xAA;
            pucBuffer++;
        }
    }
	hiedmac_trace(3,"\rEDMA clear data success!\n" );
}

void edma_build_data( UINT8 *pucBuffer, UINT32 ulSize, UINT8 ucFirstData )
{
    UINT8   ucData = ucFirstData;//pclint 734
    if (pucBuffer && ulSize)
    {
        while( ulSize-- )
        {
            *pucBuffer++ = ucData++;
        }
    }
	hiedmac_trace(3,"\rEDMA build data success!\n" );
}

int edma_verify_data( UINT8 * pucBuffer, UINT32 ulSize, UINT8 ucFirstData )
{
    UINT8   ucData = ucFirstData;    
    if( pucBuffer && ulSize )
    {
        while( ulSize-- )
        {
            if(  *pucBuffer != ucData )
            {
                printk("------ edma_verify_data failed!\n");
				return -1;
            }
            pucBuffer++;
            ucData++;
        }
		printk("------ edma_verify_data success!\n");
        return 0;
    }
    return -1;
}

void edma_buff_init( void )
{
    UINT32 i = 0;   
    pucSour = dma_alloc_coherent(NULL, (EDMA_TEST_LLI_NUM + 2) * EDMA_TEST_LLI_BLOCK_SIZE, &edma_buf_phys, GFP_DMA|__GFP_WAIT);	
    if (NULL == pucSour)
    {
    	hiedmac_trace(1,"pucSour alloc failed!\n");
        return ;
    }

    pucDest = pucSour + 2 * EDMA_TEST_LLI_BLOCK_SIZE;
    hiedmac_trace(1,"\r\nbuffer Sour = 0x%X, Dest = 0x%X\n", (UINT32)pucSour, (UINT32)(pucDest));
    hiedmac_trace(1,"\r\n------------edma_buf_phys = 0x%X\n", edma_buf_phys);
    /* Build  lli list */
    for (i = 0; i < EDMA_TEST_LLI_NUM; i++)
    {
        stDmaLLI[ i ].ulSourAddr = edma_buf_phys + (i & 0x1) * EDMA_TEST_LLI_BLOCK_SIZE ; /*�����ַ*/
        stDmaLLI[ i ].ulDestAddr = edma_buf_phys + (i + 2) * EDMA_TEST_LLI_BLOCK_SIZE; /*�����ַ*/
        stDmaLLI[ i ].ulLength   = EDMA_TEST_LLI_BLOCK_SIZE;
        stDmaLLI[ i ].ulConfig   = BALONG_DMA_SET_CONFIG(23, BALONG_DMA_M2M, 2, 7);
    }

    /* Prepare the transfer data */
    edma_clear_data(pucDest, EDMA_TEST_LLI_NUM * EDMA_TEST_LLI_BLOCK_SIZE);
    edma_build_data(pucSour, EDMA_TEST_LLI_BLOCK_SIZE, 6);
    edma_build_data(pucSour + EDMA_TEST_LLI_BLOCK_SIZE, EDMA_TEST_LLI_BLOCK_SIZE, 0x3a);

}

void edma_buff_exit( void )
{
    if (NULL != pucSour)
    {
        dma_free_coherent(NULL, (EDMA_TEST_LLI_NUM + 2) * EDMA_TEST_LLI_BLOCK_SIZE, pucSour, edma_buf_phys);
        pucSour = NULL;
        pucDest = NULL;
    }
}

/*M2Mͬ����ʽDMA���䣬��ע���жϺ������ȴ�EDMA��������ź���*/
int edmac_test_lli(BALONG_DMA_REQ req, UINT32 direction)
{
    dma_addr_t  edma_addr    = 0; 
    int         ret_id       = 0;
    int         ret          = 0;
    UINT32      i            = 0;
    BALONG_DMA_CB *pstNode   = NULL;
    BALONG_DMA_CB *psttemp   = NULL;
    BALONG_DMA_CB *FirstNode   = NULL;
    
    edma_buff_init();
    /*��������*/
    pstNode = (BALONG_DMA_CB *)dma_alloc_coherent(NULL,(EDMA_TEST_LLI_NUM * sizeof(BALONG_DMA_CB)),
                                                                   &edma_addr, GFP_DMA|__GFP_WAIT);  
    hiedmac_trace(1,"---------------edma_addr = 0x%X\n",edma_addr);
    s_edma_alloc_address.s_alloc_virt_address = (UINT32)pstNode;
	s_edma_alloc_address.s_alloc_phys_address = (UINT32)edma_addr; 
    if (NULL == pstNode)
    {		
        hiedmac_trace(3,"LII list init is failed!\n"); 
        edma_buff_exit();
    	return DMA_MEMORY_ALLOCATE_ERROR;
    }

    FirstNode = pstNode;
    psttemp = pstNode;
    for (i = 0; i < EDMA_TEST_LLI_NUM; i++)
    {
        psttemp->lli = BALONG_DMA_SET_LLI(edma_addr + (i+1) * sizeof(BALONG_DMA_CB), ((i < EDMA_TEST_LLI_NUM - 1)?0:1));
        psttemp->config = stDmaLLI[i].ulConfig;
        psttemp->src_addr = stDmaLLI[i].ulSourAddr;  /*�����ַ*/
        psttemp->des_addr = stDmaLLI[i].ulDestAddr;  /*�����ַ*/
        psttemp->cnt0 = stDmaLLI[i].ulLength;
        psttemp->bindx = 0;
        psttemp->cindx = 0;
        psttemp->cnt1  = 0;

        psttemp++;
    }
    
    hiedmac_trace(3,"LII list init is success!\n"); 

    /*����ͨ������ʼ����������ź���*/
    ret_id = balong_dma_channel_init(req, NULL, 0, 0);
    if (ret_id < 0)
    {
        hiedmac_trace(1,"-----------error ret_id = 0x%X\n",ret_id);
        return DMA_CHANNEL_INVALID;
    }

    /*��ȡ�׽ڵ�Ĵ�����ַ*/
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    psttemp = balong_dma_channel_get_lli_addr((unsigned int)ret_id);
	/*end*/
    if (NULL == psttemp)
    {
        hiedmac_trace(1,"---balong_dma_channel_get_lli_addr failed!\n");
        return DMA_CHANNEL_INVALID;
    }
    hiedmac_trace(1,"-----------BALONG_DMA_CB LLI = 0x%X!\n",(UINT32)psttemp);

    /*�����׽ڵ�Ĵ���*/
    psttemp->lli = FirstNode->lli;
    psttemp->config = FirstNode->config & 0xFFFFFFFE;
    psttemp->src_addr = FirstNode->src_addr;  /*�����ַ*/
    psttemp->des_addr = FirstNode->des_addr;  /*�����ַ*/
    psttemp->cnt0 = FirstNode->cnt0;
    psttemp->bindx = 0;
    psttemp->cindx = 0;
    psttemp->cnt1  = 0;   

    /*����EDMA���䣬�ȴ���������ź����ͷź󷵻�*/
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    if (balong_dma_channel_lli_start((unsigned int)ret_id))
	/*end*/
    {
        hiedmac_trace(1,"balong_dma_channel_lli_start FAILED!\n");
        dma_free_coherent(NULL,(EDMA_TEST_LLI_NUM * sizeof(BALONG_DMA_CB)),
                            (void*)s_edma_alloc_address.s_alloc_virt_address, edma_addr);
        edma_buff_exit();
        return DMA_TRXFER_ERROR;
    }
    /* Verify the transfer data */
    for( i = 0; i < EDMA_TEST_LLI_NUM; i++ )
    {
        UINT8* temp_dest = pucDest + i * EDMA_TEST_LLI_BLOCK_SIZE;
		ret = edma_verify_data( temp_dest, EDMA_TEST_LLI_BLOCK_SIZE, (i & 0x1) ? 0x3a : 0x6 );
        if( !ret )
        {
            break;
        }
		 printk( "\r\nFirst Verify, d[0] = 0x%X, d[1] = 0x%X, d[2] = 0x%X, d[3] = 0x%X, d[4] = 0x%X, d[5] = 0x%X\r\n ",
               *((UINT8 *)temp_dest + 0 ),
               *((UINT8 *)temp_dest + 1 ),
               *((UINT8 *)temp_dest + 2 ),
               *((UINT8 *)temp_dest + 3 ),
               *((UINT8 *)temp_dest + 4 ),
               *((UINT8 *)temp_dest + 5 ));
    }   
    
     /*��֤���������Ժ����ͷ��ڴ�*/
    dma_free_coherent(NULL,(EDMA_TEST_LLI_NUM * sizeof(BALONG_DMA_CB)),
                            (void*)s_edma_alloc_address.s_alloc_virt_address, edma_addr);
    edma_buff_exit();
    
    if (ret)
    {
        hiedmac_trace(5,"balong_dma_channel_lli_start FAILED!\n");
        return DMA_TRXFER_ERROR;
    }
    hiedmac_trace(5,"balong_dma_channel_lli_start SUCCESS!\n\n");
    return DMA_SUCCESS;  
}

/*�첽��ʽDMA���䣬�ϲ�ģ��ע���жϴ�����򣬴���DMA��������¼�*/
int edmac_test_async_lli(BALONG_DMA_REQ req, UINT32 direction)
{
    dma_addr_t  edma_addr    = 0; 
    int         ret_id       = 0;
    int         ret          = 0;
    UINT32      i            = 0;
    BALONG_DMA_CB *pstNode   = NULL;
    BALONG_DMA_CB *psttemp   = NULL;
    BALONG_DMA_CB *FirstNode   = NULL;
    
    edma_buff_init();
    /*��������*/
    pstNode = (BALONG_DMA_CB *)dma_alloc_coherent(NULL,(EDMA_TEST_LLI_NUM * sizeof(BALONG_DMA_CB)),
                                                                   &edma_addr, GFP_DMA|__GFP_WAIT);       
    s_edma_alloc_address.s_alloc_virt_address = (UINT32)pstNode;
	s_edma_alloc_address.s_alloc_phys_address = (UINT32)edma_addr; 
    if (NULL == pstNode)
    {		
        hiedmac_trace(3,"LII list init is failed!"); 
        edma_buff_exit();
    	return DMA_MEMORY_ALLOCATE_ERROR;
    }

    FirstNode = pstNode;
    psttemp = pstNode;
    for (i = 0; i < EDMA_TEST_LLI_NUM; i++)
    {
        psttemp->lli = BALONG_DMA_SET_LLI(edma_addr + (i+1) * sizeof(BALONG_DMA_CB), ((i < EDMA_TEST_LLI_NUM - 1)?0:1));
        psttemp->config = stDmaLLI[i].ulConfig;
        psttemp->src_addr = stDmaLLI[i].ulSourAddr;  /*�����ַ*/
        psttemp->des_addr = stDmaLLI[i].ulDestAddr;  /*�����ַ*/
        psttemp->cnt0 = stDmaLLI[i].ulLength;
        psttemp->bindx = 0;
        psttemp->cindx = 0;
        psttemp->cnt1  = 0;

        psttemp++;
    }
    hiedmac_trace(3,"LII list init is success!"); 

    /*����ͨ����ע��ͨ���жϻص�����*/
    ret_id = balong_dma_channel_init(req, (channel_isr)data_trans_complete, 0, BALONG_DMA_INT_DONE);
	if (ret_id < 0)
    {
        hiedmac_trace(1,"-----------error ret_id = 0x%X\n",ret_id);
        return DMA_CHANNEL_INVALID;
    }

    /*��ȡ�׽ڵ�Ĵ�����ַ*/
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    psttemp = balong_dma_channel_get_lli_addr((unsigned int)ret_id);
	/*end*/
    if (NULL == psttemp)
    {
        hiedmac_trace(1,"---balong_dma_channel_get_lli_addr failed!\n");
        return DMA_CHANNEL_INVALID;
    }

    /*�����׽ڵ�Ĵ���*/
    psttemp->lli = FirstNode->lli;
    psttemp->config = FirstNode->config & 0xFFFFFFFE;
    psttemp->src_addr = FirstNode->src_addr;  /*�����ַ*/
    psttemp->des_addr = FirstNode->des_addr;  /*�����ַ*/
    psttemp->cnt0 = FirstNode->cnt0;
    psttemp->bindx = 0;
    psttemp->cindx = 0;
    psttemp->cnt1  = 0;  

    /*����EDMA����󼴷��أ���ͨ����ѯͨ���Ƿ�busy��ȷ�������Ƿ����*/
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    if (balong_dma_channel_lli_async_start((unsigned int)ret_id))
	/*end*/
    {
        hiedmac_trace(1,"balong_dma_channel_lli_async_start FAILED!\n");
        dma_free_coherent(NULL,(EDMA_TEST_LLI_NUM * sizeof(BALONG_DMA_CB)),
                          (void*)s_edma_alloc_address.s_alloc_virt_address, edma_addr); 
        edma_buff_exit();
        return DMA_TRXFER_ERROR;
    }
    /*Warning 648: (Warning -- Overflow in computing constant for operation:
           'unsigned multiplication')*/
    /*lint -e648*/
    mdelay(200);/*lint !e62*/
	/*lint +e648*/
    
    /* Verify the transfer data */
    for( i = 0; i < EDMA_TEST_LLI_NUM; i++ )
    {
        UINT8* temp_dest = pucDest + i * EDMA_TEST_LLI_BLOCK_SIZE;
		ret = edma_verify_data( temp_dest, EDMA_TEST_LLI_BLOCK_SIZE, (i & 0x1) ? 0x3a : 0x6 );
        if( !ret )
        {
            break;
        }
		 printk( "\r\nFirst Verify, d[0] = 0x%X, d[1] = 0x%X, d[2] = 0x%X, d[3] = 0x%X, d[4] = 0x%X, d[5] = 0x%X\r\n ",
               *((UINT8 *)temp_dest + 0 ),
               *((UINT8 *)temp_dest + 1 ),
               *((UINT8 *)temp_dest + 2 ),
               *((UINT8 *)temp_dest + 3 ),
               *((UINT8 *)temp_dest + 4 ),
               *((UINT8 *)temp_dest + 5 ) );
    }   
    
    dma_free_coherent(NULL,(EDMA_TEST_LLI_NUM * sizeof(BALONG_DMA_CB)),
                          (void*)s_edma_alloc_address.s_alloc_virt_address, edma_addr);  
    edma_buff_exit();
    
    if (ret)
    {
        hiedmac_trace(5,"balong_dma_channel_lli_start FAILED!\n");
        return DMA_TRXFER_ERROR;
    }
    hiedmac_trace(5,"balong_dma_channel_lli_start SUCCESS!\n\n");
    return DMA_SUCCESS; 
}

void edmac_test_all_func(UINT32 cnt)
{
    UINT32 index = 0;
    hiedmac_trace(5,"yangcheng 3!\n\n");    
    for (index = 0; index < cnt; index++)
    {
        edmac_test_channel(EDMA_MEMORY, BALONG_DMA_M2M);
        edmac_test_channel_async(EDMA_MEMORY, BALONG_DMA_M2M);
        edmac_test_channel_async_int(EDMA_MEMORY, BALONG_DMA_M2M);
        edmac_test_lli(EDMA_MEMORY, BALONG_DMA_M2M); 
        edmac_test_async_lli(EDMA_MEMORY, BALONG_DMA_M2M);
    }
}

#endif /*#ifdef DMA_TEST*/

/*******************************************************************************
  ������:       LOCAL int __devinit balong_dma_suspend (struct platform_device *dev)
  ��������:     ϵͳ�µ�ǰ�����ã����ڱ���DMAӲ����״̬��������һЩ��Ҫ�Ĵ���
                ȷ������suspend��resume���̺�DMA����������������
  �������:     dev����ʾ��ǰ�豸
  �������:     ��
  ����ֵ:       �ɹ���0
                ʧ�ܣ���0
*******************************************************************************/
LOCAL int __devinit balong_dma_suspend (struct platform_device *dev,struct pm_message pmmsg)
{   
    int ret = 0;
	return ret;
}

/*******************************************************************************
  ������:       LOCAL int __devinit balong_dma_resume(struct platform_device *dev)
  ��������:     ϵͳ�µ�ǰ�����ã����ڱ���DMAӲ����״̬��������һЩ��Ҫ�Ĵ���
                ȷ������suspend��resume���̺�DMA����������������
  �������:     dev����ʾ��ǰ�豸
  �������:     ��
  ����ֵ:       �ɹ���0
                ʧ�ܣ���0
*******************************************************************************/
LOCAL int __devinit balong_dma_resume(struct platform_device *dev)
{
	int ret = 0;
	return ret;
}

/*******************************************************************************
  ������:       static void __devinit balong_dma_shutdown (struct platform_device *dev)
  ��������:     �ڹػ������б����ã���һЩ��Ҫ�Ĺػ�ǰ����
  �������:     dev����ʾ��ǰ�豸
  �������:     ��
  ����ֵ:       ��
*******************************************************************************/
static void __devinit balong_dma_shutdown (struct platform_device *dev)
{	
	 return;
}

/*******************************************************************************
  ������:       LOCAL int __devinit balong_dma_probe(struct platform_device *pdev)
  ��������:     ��DMAC�ļĴ��������ַӳ��������ַ��ע���жϷ������
                DMAȫ�ּĴ������ڴ˳�ʼ������vxWorks DMA���������г�ʼ����
  �������:     pdev��ָ��DMAC�豸��ָ�롣
  �������:     ��
  ����ֵ:       �ɹ���0
                ʧ�ܣ���0
*******************************************************************************/
LOCAL int __devinit balong_dma_probe(struct platform_device *pdev)
{
    int ret = 0;

#ifdef DMA_TEST
    src_buf= (char*)dma_alloc_coherent(NULL,DMA_M2M_TEST_LENGHT * sizeof(char*),&edma_src_addr, GFP_DMA|__GFP_WAIT); 
    des_buf= (char*)dma_alloc_coherent(NULL,DMA_M2M_TEST_LENGHT * sizeof(char*),&edma_des_addr, GFP_DMA|__GFP_WAIT); 
#endif

    hiedmac_trace(1,"edmac probe begin!\n");
    g_edmac_base_addr = ioremap(HI_EDMAC_BASE, sizeof(EDMAC_REG_STRU));
    s_pstEDMACReg = (EDMAC_REG_STRU *)g_edmac_base_addr;
    
    edmac_int();

    hiedmac_trace(1,"edmac request irq = %d\n", BALONG_V3R2_EDMAC_IRQ);
    if(request_irq(BALONG_V3R2_EDMAC_IRQ, balong_dma_isr, IRQF_DISABLED, "Balong_V3R2_EDMAC", NULL))
    {
        hiedmac_trace(1,"EDMA Irq request failed!\n");
        ret = -1;
        return ret;
	}

    hiedmac_trace(3, "edmac probe OK!\n");
    return ret;
}

LOCAL int __devexit balong_dma_remove(struct platform_device *pdev)
{
#ifdef DMA_TEST
    dma_free_coherent(NULL,DMA_M2M_TEST_LENGHT * sizeof(char*),(void*)src_buf, edma_src_addr);
    dma_free_coherent(NULL,DMA_M2M_TEST_LENGHT * sizeof(char*),(void*)des_buf, edma_des_addr);
    src_buf = NULL;
    des_buf = NULL;
#endif
    free_irq(BALONG_V3R2_EDMAC_IRQ,NULL);
	return 0;
}

LOCAL struct platform_driver edmac_driver = {
	.probe = balong_dma_probe,
	.remove = __devexit_p(balong_dma_remove),
	.shutdown = balong_dma_shutdown,//--------------
	.suspend = balong_dma_suspend,//--------------
	.resume = balong_dma_resume,//--------------
	.driver = {
		.name = DRIVER_NAME,
		.owner  = THIS_MODULE,
		.pm     = NULL,
	},
};

LOCAL struct platform_device edmac_device =
{
    .name = DRIVER_NAME,
    .id       = 1,
    .num_resources = 0,  
};

LOCAL int __init balong_dma_init(void)
{
    int retval;

    hiedmac_trace(1,"balong_edmac_module_init+\n");
    retval = platform_device_register(&edmac_device);
    if (retval)
    {
        hiedmac_trace(1,"hisilicon platform_device_register failed!\n");
        return retval;
    }

    retval = platform_driver_register(&edmac_driver);
    
    if (retval)
    {
        hiedmac_trace(1,"hisilicon platform devicer register Failed!\n");
        platform_device_unregister(&edmac_device);
        return retval;
    }
    
    hiedmac_trace(1,"balong_edmac_module_init-\n");
    return retval;
}

LOCAL void __exit balong_dma_exit(void)
{
    platform_driver_unregister(&edmac_driver);
    platform_device_unregister(&edmac_device);
}

subsys_initcall(balong_dma_init);
module_exit(balong_dma_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_VERSION("HI_VERSION=" OSDRV_MODULE_VERSION_STRING);
