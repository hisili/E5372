
#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */

#include <linux/slab.h>
//#include "BSP.h"
#include "bsp_udi_drv.h"



#ifdef CONFIG_USB_OPEN
#include "jconfig.h"
#include "uw_device_cdc_acm.h"

static BSP_S32 udiAdpNcmInit(VOID);
static BSP_S32 udiAdpIccInit(VOID);
extern BSP_U8* BSP_AcmGetDevNameByType(BSP_U32 u32Type);

/*modified for lint e752
static BSP_S32 udiAdpUartInit(VOID); */
static BSP_S32 udiAdpAcmInit(VOID);
BSP_S32 BSP_AcmClose(BSP_S32 s32AcmDevId);
BSP_S32 BSP_AcmWrite(BSP_S32 s32AcmDevId, BSP_U8* pBuf, BSP_U32 u32Size);
BSP_S32 BSP_AcmRead(BSP_S32 s32AcmDevId, BSP_U8* pBuf, BSP_U32 u32Size);
BSP_S32 BSP_AcmIoctl(BSP_S32 s32AcmDevId, BSP_S32 s32Cmd, BSP_S32 s32Arg);
BSP_S32 BSP_AcmOpen(BSP_S32 s32AcmType, BSP_S32 s32Flags, BSP_S32 s32Mode);
#endif



static BSP_S32 udiAdpIccInit(VOID);
/*
static BSP_S32 udiAdpUartInit(VOID);
*/


/* 各模块特性值定义 */
#define UDI_USB_ACM_CAPA		(UDI_CAPA_BLOCK_READ | UDI_CAPA_BLOCK_WRITE | UDI_CAPA_READ_CB | UDI_CAPA_WRITE_CB)
#define UDI_USB_NCM_NDIS_CAPA	(UDI_CAPA_READ_CB | UDI_CAPA_BUFFER_LIST)
#define UDI_USB_NCM_CTRL_CAPA	(UDI_CAPA_READ_CB | UDI_CAPA_CTRL_OPT)
#define UDI_ICC_CAPA				(UDI_CAPA_BLOCK_READ | UDI_CAPA_BLOCK_WRITE | UDI_CAPA_READ_CB | UDI_CAPA_WRITE_CB)
#define UDI_UART_CAPA			(UDI_CAPA_BLOCK_READ | UDI_CAPA_BLOCK_WRITE)


/* 各模块初始化函数定义 */
VOID* g_udiInitFuncTable[(BSP_U32)UDI_DEV_MAX+1] = 
{

#ifdef CONFIG_USB_OPEN
	/* ACM Init */
	udiAdpAcmInit,

	/* NCM Init */
	udiAdpNcmInit,
#endif
	/* ICC Init */
	udiAdpIccInit,

	/* UART Init */
	/* udiAdpUartInit, */
	
	/* Must Be END */
	NULL
};

/*** eric **/

#ifdef CONFIG_USB_OPEN
/**************************************************************************
  ACM 适配实现(可能需要讨论设备形态的动态切换)
**************************************************************************/
static BSP_U32 sg_AcmTypeTbl[UDI_USB_ACM_MAX+1]=
{
    DEVICE_FD_CTRL,
    DEVICE_FD_ATCOM,
    DEVICE_FD_SHELL,
    DEVICE_FD_OMCOM,
    DEVICE_FD_3G_OMCOM,
    DEVICE_FD_MODEM,
	DEVICE_FD_GPS,
	DEVICE_FD_3G_GPS,
	DEVICE_FD_3G_PCVOICE,
	DEVICE_FD_PCVOICE,
	//DEVICE_FD_3G_ATCOM,
	//DEVICE_FD_3G_MODEM,
#ifdef CONFIG_SYN_HSFC_HSIC
    DEVICE_FD_HSIC_ACM0,
    DEVICE_FD_HSIC_ACM1,
    DEVICE_FD_HSIC_ACM2,
    DEVICE_FD_HSIC_ACM3,
    DEVICE_FD_HSIC_ACM4,
    DEVICE_FD_HSIC_ACM5,
    DEVICE_FD_HSIC_ACM6,
    DEVICE_FD_HSIC_ACM7,
    DEVICE_FD_HSIC_ACM8,
    DEVICE_FD_HSIC_ACM9,
    DEVICE_FD_HSIC_ACM10,
    DEVICE_FD_HSIC_ACM11,
    DEVICE_FD_HSIC_ACM12,
    DEVICE_FD_HSIC_ACM13,
	DEVICE_FD_HSIC_ACM14,
#endif
    DEVICE_FD_NUM
};

#define UDI_GET_ACM_FD(type) sg_AcmTypeTbl[(BSP_U32)type]

static BSP_S32 udiAcmAdpOpen(UDI_OPEN_PARAM *param, UDI_HANDLE handle)
{
	BSP_U8* pstrName;
	BSP_U32 u32Type;
	BSP_S32 s32Fd;

	UDI_UNUSED_PARAM(handle);

	u32Type = UDI_GET_DEV_TYPE(param->devid);
	s32Fd = 0;
      
	if (s32Fd == -1)
	{
		pstrName = 0;
		printk(KERN_ERR "BSP_MODU_UDI open %s fail, ret:%d line:%d\n", pstrName, s32Fd, __LINE__);
		return ERROR;
	}

	(BSP_VOID)BSP_UDI_SetPrivate(param->devid, (VOID*)s32Fd);
	return OK;
}

static BSP_S32 udiAdpAcmInit(VOID)
{
	UDI_DRV_INTEFACE_TABLE* pDrvInterface = NULL;
	BSP_U32 u32Cnt;

	/* 构造回调函数指针列表 */
	pDrvInterface = kmalloc(sizeof(UDI_DRV_INTEFACE_TABLE), GFP_KERNEL);
	if (NULL == pDrvInterface)
	{
		printk(KERN_ERR "BSP_MODU_UDI NO Mem, line:%d\n", __LINE__);
		return ERROR;
	}
	memset((VOID*)pDrvInterface, 0, sizeof(UDI_DRV_INTEFACE_TABLE));

	/* 只设置需要实现的几个接口 */


	/* 3个 ACM 都使用同一套驱动函数指针 */
	for (u32Cnt = UDI_USB_ACM_CTRL; u32Cnt < UDI_USB_ACM_MAX; u32Cnt++)
	{
		(BSP_VOID)BSP_UDI_SetCapability((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, u32Cnt), UDI_USB_ACM_CAPA);
		(BSP_VOID)BSP_UDI_SetInterfaceTable((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, u32Cnt), pDrvInterface);
	}

	return OK;
}

/**************************************************************************
  NCM 适配实现
**************************************************************************/
static BSP_U32 sg_chnNcmDataHandleTbl[BSP_NCM_MAX_DEV_NUM] = {0};

#ifdef CONFIG_SYN_HSFC_HSIC
extern BSP_S32 BSP_NcmGetTypeIdx(BSP_U32 u32NcmType);
BSP_S32 udiNCMAdpGetDevId(BSP_U32 u32ChanId)
{
    BSP_U32 u32NcmType;
    switch(u32ChanId)
    {
        case UDI_USB_NCM_NDIS:
        case UDI_USB_NCM_CTRL:
            u32NcmType = DEVICE_FD_NCM;
            break;
        case UDI_USB_HSIC_NCM0:
        case UDI_USB_HSIC_NCM1:
        case UDI_USB_HSIC_NCM2:
            u32NcmType = DEVICE_FD_HSIC_NCM0+u32ChanId-UDI_USB_HSIC_NCM0;
            break;
        default:
            printk(KERN_ERR "udiNCMAdpGetDevId ERROR u32ChanId %d, line:%d\n", u32ChanId,__LINE__);
            return -1;
    }
    
    return BSP_NcmGetTypeIdx(u32NcmType);
}
#endif


UDI_HANDLE BSP_UDI_NCM_DataChnToHandle(BSP_U32 u32Chn)
{
    return sg_chnNcmDataHandleTbl[u32Chn];
}

static BSP_S32 udiNcmAdpOpen(UDI_OPEN_PARAM *param, UDI_HANDLE handle)
{
	BSP_U32 u32NcmDevId;
	BSP_U32 u32Ret;
	BSP_U32 u32ChanId;
	
	UDI_UNUSED_PARAM(handle);

	u32ChanId = UDI_GET_DEV_TYPE(param->devid);
    
#ifdef CONFIG_SYN_HSFC_HSIC
       u32NcmDevId = udiNCMAdpGetDevId(u32ChanId);
#endif

	u32Ret = 0;
	(BSP_VOID)BSP_UDI_SetPrivate(param->devid, (VOID*)u32NcmDevId);
#ifdef CONFIG_SYN_HSFC_HSIC
    sg_chnNcmDataHandleTbl[u32NcmDevId] = handle;    /*lint !e732*/ 
#else
    sg_chnNcmDataHandleTbl[u32ChanId] = handle;    /*lint !e732*/ 
#endif
	return (BSP_S32)u32Ret;
}

static BSP_S32 udiNcmAdpCtrlOpen(UDI_OPEN_PARAM *param, UDI_HANDLE handle)
{
	BSP_U32 u32NcmDevId;
	BSP_U32 u32Ret;

	UDI_UNUSED_PARAM(handle);

	u32Ret = 0;
	
	return (BSP_S32)u32Ret;
}

static BSP_S32 udiNcmAdpClose(VOID* pPrivate)
{
	return 0;
}

static BSP_S32 udiNcmAdpCtrlClose(VOID* pPrivate)
{
	return 0;
}

static BSP_S32 udiNcmAdpWrite(VOID* pPrivate, VOID* pMemObj, BSP_S32 s32Size)
{
	UDI_UNUSED_PARAM(s32Size);
	return 0;
}

static BSP_S32 udiAdpNcmInit(VOID)
{
	UDI_DRV_INTEFACE_TABLE* pDrvDataInterface = NULL;
	UDI_DRV_INTEFACE_TABLE* pDrvCtrlInterface = NULL;
#ifdef CONFIG_SYN_HSFC_HSIC
       BSP_U32 u32Cnt;
#endif

	/* 构造回调函数指针列表(数据通道) */
	pDrvDataInterface = kmalloc(sizeof(UDI_DRV_INTEFACE_TABLE), GFP_KERNEL);
	if (NULL == pDrvDataInterface)
	{
		printk(KERN_ERR "BSP_MODU_UDI NO Mem, line:%d\n", __LINE__);
		goto ADP_NCM_INIT_ERR;
	}
	memset((VOID*)pDrvDataInterface, 0, sizeof(UDI_DRV_INTEFACE_TABLE));

	/* 只设置需要实现的几个接口(数据通道) */
	pDrvDataInterface->udi_open_cb = (UDI_OPEN_CB_T)udiNcmAdpOpen;
	pDrvDataInterface->udi_close_cb = (UDI_CLOSE_CB_T)udiNcmAdpClose;
	pDrvDataInterface->udi_write_cb = (UDI_WRITE_CB_T)udiNcmAdpWrite;


	/* 设置函数指针(数据通道) */
	(BSP_VOID)BSP_UDI_SetCapability(UDI_NCM_NDIS_ID, UDI_USB_NCM_NDIS_CAPA);
	(BSP_VOID)BSP_UDI_SetInterfaceTable(UDI_NCM_NDIS_ID, pDrvDataInterface);

#ifdef CONFIG_SYN_HSFC_HSIC
        for(u32Cnt=UDI_USB_HSIC_NCM0;u32Cnt<UDI_USB_NCM_MAX;u32Cnt++)
        {
            (BSP_VOID)BSP_UDI_SetCapability((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, u32Cnt), UDI_USB_NCM_NDIS_CAPA);
            (BSP_VOID)BSP_UDI_SetInterfaceTable((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, u32Cnt), pDrvDataInterface);
        }
#endif

	/* 构造回调函数指针列表(控制通道) */
	pDrvCtrlInterface = kmalloc(sizeof(UDI_DRV_INTEFACE_TABLE), GFP_KERNEL);
	if (NULL == pDrvCtrlInterface)
	{
		printk(KERN_ERR "BSP_MODU_UDI NO Mem, line:%d\n", __LINE__);
		goto ADP_NCM_INIT_ERR;
	}
	memset((VOID*)pDrvCtrlInterface, 0, sizeof(UDI_DRV_INTEFACE_TABLE));

	/* 构造回调函数指针列表(控制通道) */
	pDrvCtrlInterface->udi_open_cb = (UDI_OPEN_CB_T)udiNcmAdpCtrlOpen;
	pDrvCtrlInterface->udi_close_cb = (UDI_CLOSE_CB_T)udiNcmAdpCtrlClose;
	pDrvCtrlInterface->udi_write_cb = (UDI_WRITE_CB_T)udiNcmAdpWrite;
	
	/* 设置函数指针(数据通道) */
	(BSP_VOID)BSP_UDI_SetCapability(UDI_NCM_CTRL_ID, UDI_USB_NCM_CTRL_CAPA);
	(BSP_VOID)BSP_UDI_SetInterfaceTable(UDI_NCM_CTRL_ID, pDrvCtrlInterface);

	return OK;

ADP_NCM_INIT_ERR:
	if (NULL != pDrvDataInterface)
	{
		kfree(pDrvDataInterface);
	}
	if (NULL != pDrvCtrlInterface)
	{
		kfree(pDrvCtrlInterface);
	}

	return ERROR;
}
#endif

/**************************************************************************
  UART 适配实现
**************************************************************************/
#if 0
static BSP_U8* sg_UartNameTbl[]=
{
	"/tyCo/0",
	"/tyCo/1",
	NULL
};
#define UDI_GET_UART_NUM() (sizeof(sg_UartNameTbl)/sizeof(BSP_U8*)-1)

static BSP_S32 udiUartAdpOpen(UDI_OPEN_PARAM *param, UDI_HANDLE handle)
{
    
	BSP_U8* pstrName;
	BSP_U32 u32Type;
	BSP_S32 s32Fd;

	UDI_UNUSED_PARAM(handle);

	u32Type = UDI_GET_DEV_TYPE(param->devid);
	if (u32Type > UDI_GET_UART_NUM()-1)
	{
		printk(KERN_ERR "BSP_MODU_UDI Invalid UART Num:%d, line:%d\n", u32Type, __LINE__);
		return ERROR;
	}

	pstrName = sg_UartNameTbl[u32Type];
	s32Fd = open((char*)pstrName, O_RDWR, 0);
	if (s32Fd <= 0)
	{
		printk(KERN_ERR "BSP_MODU_UDI open %s fail, ret:%d line:%d\n", pstrName, s32Fd, __LINE__);
		return ERROR;
	}

	(BSP_VOID)BSP_UDI_SetPrivate(param->devid, (VOID*)s32Fd);
	return OK;
}

static BSP_S32 udiAdpUartInit(VOID)
{
	
	UDI_DRV_INTEFACE_TABLE* pDrvInterface = NULL;
	BSP_U32 u32Cnt;

	/* 构造回调函数指针列表 */
	pDrvInterface = kmalloc(sizeof(UDI_DRV_INTEFACE_TABLE), GFP_KERNEL);
	if (NULL == pDrvInterface)
	{
		printk(KERN_ERR "BSP_MODU_UDI NO Mem, line:%d\n", __LINE__);
		return ERROR;
	}
	memset((VOID*)pDrvInterface, 0, sizeof(UDI_DRV_INTEFACE_TABLE));

	/* 只设置需要实现的几个接口 */
	pDrvInterface->udi_open_cb = udiUartAdpOpen;
	pDrvInterface->udi_close_cb = (UDI_CLOSE_CB_T)close;
	pDrvInterface->udi_write_cb = (UDI_WRITE_CB_T)write;
	pDrvInterface->udi_read_cb = (UDI_READ_CB_T)read;
	pDrvInterface->udi_ioctl_cb = (UDI_IOCTL_CB_T)ioctl;

	/* 3个 ACM 都使用同一套驱动函数指针 */
	for (u32Cnt = 0; u32Cnt < UDI_GET_UART_NUM(); u32Cnt++)
	{
		(BSP_VOID)BSP_UDI_SetCapability((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_UART, u32Cnt), UDI_USB_ACM_CAPA);
		(BSP_VOID)BSP_UDI_SetInterfaceTable((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_UART, u32Cnt), pDrvInterface);
	}
	
	return OK;
}
#endif




/**************************************************************************
  ICC 适配实现
**************************************************************************/
static BSP_U32 sg_chnHandleTbl[ICC_CHAN_NUM_MAX] = {0};

UDI_HANDLE BSP_UDI_ICC_ChnToHandle(BSP_U32 u32Chn)
{
	return sg_chnHandleTbl[u32Chn];
}

static BSP_S32 udiIccAdpOpen(UDI_OPEN_PARAM *param, UDI_HANDLE handle)
{
	BSP_U32 u32ChanId;
	ICC_CHAN_ATTR_S *pstOpenParam;

	u32ChanId = UDI_GET_DEV_TYPE(param->devid);
	(BSP_VOID)BSP_UDI_SetPrivate(param->devid, (VOID*)u32ChanId);

	/* 从param 中解析出各个参数 */
	pstOpenParam = (ICC_CHAN_ATTR_S *)param->pPrivate;
	sg_chnHandleTbl[u32ChanId] = handle;     /*lint !e732*/

	return BSP_ICC_Open(u32ChanId, pstOpenParam);
}

static BSP_S32 udiAdpIccInit(VOID)
{
	UDI_DRV_INTEFACE_TABLE* pDrvInterface = NULL;
	BSP_U32 u32Cnt;

	/* 构造回调函数指针列表 */
	pDrvInterface = kmalloc(sizeof(UDI_DRV_INTEFACE_TABLE), GFP_KERNEL);
	if (NULL == pDrvInterface)
	{
		printk(KERN_ERR "BSP_MODU_UDI NO Mem, line:%d\n", __LINE__);
		return ERROR;
	}
	memset((VOID*)pDrvInterface, 0, sizeof(UDI_DRV_INTEFACE_TABLE));

	/* 只设置需要实现的几个接口 */
	pDrvInterface->udi_open_cb = (UDI_OPEN_CB_T)udiIccAdpOpen;
	pDrvInterface->udi_close_cb = (UDI_CLOSE_CB_T)BSP_ICC_Close;
	pDrvInterface->udi_write_cb = (UDI_WRITE_CB_T)BSP_ICC_Write;
	pDrvInterface->udi_read_cb = (UDI_READ_CB_T)BSP_ICC_Read;
	pDrvInterface->udi_ioctl_cb = (UDI_IOCTL_CB_T)BSP_ICC_Ioctl;

	/* 几个 ICC 都使用同一套驱动函数指针 */
	for (u32Cnt = 0; u32Cnt < ICC_CHAN_NUM_MAX; u32Cnt++)
	{
		(BSP_VOID)BSP_UDI_SetCapability((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_ICC, u32Cnt), UDI_ICC_CAPA);
		(BSP_VOID)BSP_UDI_SetInterfaceTable((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_ICC, u32Cnt), pDrvInterface);
	}

	return OK;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */




