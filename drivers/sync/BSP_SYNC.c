/*************************************************************************
*   ��Ȩ����(C) 1987-2010, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_SYNC.c
*
*   ��    �� :  wangxuesong
*
*   ��    �� :  ���ļ�����IFCģ��ʵ�֣����ڹ���ģ�����Ӻ�ͬ��������
*
*   �޸ļ�¼ :  2010��4��2��  v1.00  wangxuesong  ����
*************************************************************************/
#include <mach/common/bsp_version.h>
#include <mach/DrvInterface.h>
#include <mach/BSP_SYNC_DRV.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/delay.h>

/*--------------------------------------------------------------*
 * �궨��                                                       *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * �ⲿ����ԭ��˵��                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * �ڲ�����ԭ��˵��                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * �ⲿ��������                                                 *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * ȫ�ֱ�������                                                 *
 *--------------------------------------------------------------*/

BSP_U32 g_u32syncStatus = 0;

BSP_S32 BSP_SYNC_CallBack(BSP_VOID *pMsgBody,BSP_U32 u32Len);

 /*****************************************************************************
 * �� �� ��  : BSP_SYNC_Init
 *
 * ��������  : SYNC��ʼ������
 *
 * �������  : ��
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * ����˵��  : ��
 *
 *****************************************************************************/
BSP_S32 BSP_SYNC_Init()
{
    BSP_S32 s32Ret = 0;
    
    s32Ret = BSP_IFCP_RegFunc(IFCP_FUNC_SYNC, (BSP_IFCP_REG_FUNC)BSP_SYNC_CallBack);
    if(BSP_OK != s32Ret)
    {
        printk( "SYNC init, BSP_IFC_RegFunc failed.\n");
        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : BSP_SYNC_Wait
*
* ��������  : �Ӻ˵ȴ����˲�����ɺ���
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* ����˵��  : ��
*
*****************************************************************************/
EXPORT_SYMBOL(BSP_SYNC_Wait);
EXPORT_SYMBOL(BSP_GetBuildVersion);
BSP_S32 BSP_SYNC_Wait(SYNC_MODULE_E u32Module)
{
    BSP_U32 module = 1 << (BSP_U32)u32Module;
    BSP_U32 flag = 1;
    BSP_U32 i;

    /* �ȴ���ֱ������CPU��ɳ�ʼ�� */
    for(i=0; i<BSP_SYNC_TIMEOUT; i++)
    {
        if(g_u32syncStatus & module)
        {
            break;
        }
        
        if(flag)
        {
            printk("###### module %d waiting", u32Module);
            flag = 0;
        }

        if(0 == (i%20))
        {
            printk(".", 0,0,0,0,0,0);
        }

        mdelay(10);
    }

    printk("\n", 0,0,0,0,0,0);

    if(BSP_SYNC_TIMEOUT == i)
    {
        return BSP_ERROR;
    }
    else
    {
        return BSP_OK;
    }
}

/*****************************************************************************
* �� �� ��  : BSP_SYNC_CallBack
*
* ��������  : SYNCģ���CallBack����
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_S32 BSP_SYNC_CallBack(BSP_VOID *pMsgBody,BSP_U32 u32Len)
{
    BSP_U32 module = *(BSP_U32*)pMsgBody;

    /* �����˲������״̬��1*/
    g_u32syncStatus |= (1 << module);

    return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : BSP_SYNC_Give
*
* ��������  : ����֪ͨ�Ӻ����˲����Ѿ����
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_S32 BSP_SYNC_Give(SYNC_MODULE_E module)
{
    BSP_S32 ret;
    BSP_U32 msg[3];

    msg[0] = IFC_MODULE_BSP;
    msg[1] = IFCP_FUNC_SYNC;
    msg[2] = module;
    ret = BSP_IFC_Send((BSP_VOID*)msg, sizeof(msg));
    if(BSP_OK != ret)
    {
        printk(BSP_LOG_LEVEL_ERROR, BSP_MODU_SYNC, 
            "module %d sync give,  send ifc failed. ret=0x%x\n",module,ret,0,0,0,0);
        return BSP_ERROR;
    }

    return BSP_OK;
}


