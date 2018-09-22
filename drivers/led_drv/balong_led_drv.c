/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  balong_led_drv.c
*
*   作    者 :  j00179452
*
*   描    述 :  LED_DRV 核间通信 A核模块
* 
*************************************************************************/
#include <mach/common/bsp_version.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/device.h>
#include "BSP.h"
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include "balong_led_drv.h"
#include <linux/BSP_UniformDrv.h>
#include <generated/FeatureConfig.h>
#if ( FEATURE_MMI_TEST == FEATURE_ON ) 
#include "../../arch/arm/mach-balong/mmi.h"
#endif
#if (FEATURE_E5_LED == FEATURE_ON)

/*全局变量，保存APP传入的点灯参数*/
LED_IOCTL_ARG  g_stLedIoctlArg[LED_ID_MAX] = {0};

extern BSP_U32 g_E5_Version_Index;
extern E5_Product_version_info g_E5_Version_Info[];

/*****************************************************************************
* 函 数 名  : balong_led_open
*
* 功能描述  : 
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int balong_led_open(struct inode *inode, struct file *file)
{
    if ((NULL != inode) && (NULL != file))
    {
        ;
    }
    
    return BSP_OK;
}
/*****************************************************************************
* 函 数 名  : balong_led_ioctl
*
* 功能描述  : LED_DRV 点灯接口函数
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int balong_led_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
    unsigned long arg)
{
    BSP_S32 ret;
    LED_ID id = 0;
    LED_IOCTL_ARG pstArg;
    memset(&pstArg,0,sizeof(LED_IOCTL_ARG));
    
    /*入参判断*/
    if ((NULL != inode) && (NULL != file))
    {
        ;
    }
#if ( FEATURE_E5 == FEATURE_ON ) && (defined(VERSION_V3R2))
    else if ( mmi_test_mode )
    {
        /*如果在mmi测试模式下则不处理*/
        return BSP_OK;
    }
#endif
    if(NULL == arg)
    {
        return BSP_ERROR;
    } 
    
    if((cmd < LED_IOCTL_SET) || (cmd >= LED_IOCTL_CMD_MAX))
    {
        return BSP_ERROR;
    } 

    if (copy_from_user((LED_IOCTL_ARG*)(&pstArg), arg, sizeof(LED_IOCTL_ARG)))
    {
        printk("LED_DRV:copy_from_user failed!\n");
        return BSP_ERROR;
    }
    
    if((pstArg.led_id < LED_ID_NETWORK) || (pstArg.led_id >= LED_ID_MAX))
    {
        return BSP_ERROR;
    }
                  
    if(LED_IOCTL_SET == cmd)
    {
        /*点灯参数通过IFC传给C核*/
        ret = balong_led_SendArg(cmd,&pstArg);
        if(BSP_OK != ret)
        {
            printk("LED_DRV: Balong Led SendArg Fail! ret=0x%x\n", ret, 0, 0, 0, 0, 0);
            return BSP_ERROR;
        }

        /*故障态点灯参数保存*/
        if(LED_STATE_ALL_BLINK == pstArg.led_state)
        {          
            for(id=LED_ID_NETWORK;id<=LED_ID_POWER;id++)
            {
                g_stLedIoctlArg[id].led_state = pstArg.led_state;
                g_stLedIoctlArg[id].led_id = id;
                g_stLedIoctlArg[id].led_color = pstArg.led_color;
                g_stLedIoctlArg[id].second_led_color = pstArg.second_led_color;
                g_stLedIoctlArg[id].duration = pstArg.duration;
                g_stLedIoctlArg[id].second_duration = pstArg.second_duration;
            }
        }
        /*正常态点灯参数保存*/
        else 
        {           
            g_stLedIoctlArg[pstArg.led_id].led_state = pstArg.led_state;
            g_stLedIoctlArg[pstArg.led_id].led_id = pstArg.led_id;
            g_stLedIoctlArg[pstArg.led_id].led_color = pstArg.led_color;
            g_stLedIoctlArg[pstArg.led_id].second_led_color = pstArg.second_led_color;
            g_stLedIoctlArg[pstArg.led_id].duration = pstArg.duration;
            g_stLedIoctlArg[pstArg.led_id].second_duration = pstArg.second_duration;
        }
       
    }
    /*点灯状态获取*/
    else if(LED_IOCTL_GET == cmd)
    {
        pstArg.led_state = g_stLedIoctlArg[pstArg.led_id].led_state;
        pstArg.led_color = g_stLedIoctlArg[pstArg.led_id].led_color;
        pstArg.second_led_color = g_stLedIoctlArg[pstArg.led_id].second_led_color;
        pstArg.duration = g_stLedIoctlArg[pstArg.led_id].duration;
        pstArg.second_duration = g_stLedIoctlArg[pstArg.led_id].second_duration; 

        if(copy_to_user(arg, &pstArg, sizeof(LED_IOCTL_ARG)))
        {
            printk("LED_DRV:copy_to_user failed!\n");
            return BSP_ERROR;
        }
    }
    else
    {
        return BSP_ERROR;
    }
    
    return BSP_OK;
}
/*****************************************************************************
* 函 数 名  : balong_led_release
*
* 功能描述  : 
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int balong_led_release(struct inode *inode, struct file *file)
{
    if ((NULL != inode) && (NULL != file))
    {
        ;
    }
    
    return BSP_OK;
}

/*指示灯操作函数数据结构*/
static const struct file_operations balong_led_fops = {
    .owner = THIS_MODULE,
    .ioctl = balong_led_ioctl,
    .open = balong_led_open,
    .release = balong_led_release,
};


/*balong_led_miscdev作为调用misc_register函数的参数，
用于向linux内核注册指示灯misc设备。
*/
static struct miscdevice balong_led_miscdev = {
    .name = "led",
    .minor = MISC_DYNAMIC_MINOR,/*动态分配子设备号（minor）*/
    .fops = &balong_led_fops,
};

/*****************************************************************************
* 函 数 名  : balong_led_init
*
* 功能描述  : LED_DRV A核模块初始化
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
static int __init balong_led_init(void)
{
    if((ERROR_INDEX != g_E5_Version_Index)&&(LED == g_E5_Version_Info[g_E5_Version_Index].E5_Screen_Type))
    {
        int ret = 0;

    ret = misc_register(&balong_led_miscdev);
    if (0 != ret)
    {
        printk("LED_DRV:misc register is failed!\n");
        return ret;
    }
    }
    return BSP_OK;
}

module_init(balong_led_init);

/*****************************************************************************
* 函 数 名  : balong_led_SendArg
*
* 功能描述  : LED_DRV模块核间通信
*             将APP下发的点灯参数通过IFC传到C核
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int balong_led_SendArg(unsigned int cmd,LED_IOCTL_ARG *pstLedArg)
{
    BSP_S32 ret;
    LED_IFC_MSG_STRU stLedIFCMsg = {0};
    
    /*消息体变量赋值*/
    stLedIFCMsg.u32ModuleId = IFC_MODULE_BSP;
    stLedIFCMsg.u32FuncId = IFCP_FUNC_LED_CTRL_SET;
    stLedIFCMsg.ulLedCmd = cmd;
    stLedIFCMsg.stLedIoctlArg.led_state = pstLedArg->led_state;
    stLedIFCMsg.stLedIoctlArg.led_id = pstLedArg->led_id;
    stLedIFCMsg.stLedIoctlArg.led_color = pstLedArg->led_color;
    stLedIFCMsg.stLedIoctlArg.second_led_color = pstLedArg->second_led_color;
    stLedIFCMsg.stLedIoctlArg.duration = pstLedArg->duration;
    stLedIFCMsg.stLedIoctlArg.second_duration = pstLedArg->second_duration;

    /*通过IFC将消息发出*/
    ret = BSP_IFC_Send((BSP_VOID*)&stLedIFCMsg, sizeof(LED_IFC_MSG_STRU));
    if(BSP_OK != ret)
    {
        printk("LED_DRV: ACPU IFC Send Fail! ret=0x%x\n", ret, 0, 0, 0, 0, 0);
        return BSP_ERROR;
    }

    return BSP_OK;
}

#endif

/*V3R2 E5 BREATH_LED begin*/
#if (FEATURE_BREATH_LIGHT == FEATURE_ON)


/*记录当前LED应该处于何种状态，当用户开启LED时恢复现场用*/
unsigned int g_ledLastState = BREATH_LED_STATE_ON;

/*假开机时临时开启LED控制权用*/
int led_enable_once = false;

/*省电功能用的结构体，内含LED是否被使能*/
NV_POWER_SAVE_TYPE g_powerControl;

extern BSP_U32 NVM_Read(BSP_U16 usID,BSP_VOID *pItem,BSP_U32 ulLength);
extern BSP_U32 NVM_Write(BSP_U16 usID,BSP_VOID *pItem,BSP_U32 ulLength);

#if defined(FEATURE_SHORT_ON_OFF)
/*****************************************************************************
* 函 数 名  : balong_breath_enable_once
*
* 功能描述  : 临时允许APP控制LED（仅在5s开机中使用，为了在NV不使能时，LED的状态假开机和真开机基本一致）
*
* 输入参数  : true:开启  false：关闭
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
void balong_breath_temp_enable(int flag)
{
    if( true == flag )
    {
        led_enable_once = flag;
    }
    else
    {
        led_enable_once = false;
    }
}
#endif

/*****************************************************************************
* 函 数 名  : balong_breath_get_status
*
* 功能描述  : 读取NV,返回当前LED是否使能
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : LED_ENABLE :  使能
*             LED_DISABLE:  不使能
*             BSP_ERROR:    NV读取错误
* 其它说明  : 无
*
*****************************************************************************/
int balong_breath_get_status()
{
    int ret = 0;
    
    /*读取LED呼吸灯是否使能NV*/
    ret = NVM_Read( NV_POWER_SAVE,  &g_powerControl, sizeof(g_powerControl) );
    if( NV_OK != ret  )
    {
        printk(KERN_ERR "\r\n[ breath led ] breath led read NV failed ID = 0x%x.,ret =%d;\r\n", NV_POWER_SAVE,ret);
        return BSP_ERROR;
    }

    return g_powerControl.ulLEDEnable;
}

/*****************************************************************************
* 函 数 名  : balong_breath_set_status
*
* 功能描述  : 设置使能呼吸灯的NV
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int balong_breath_set_status( int led_status)
{
    int ret = 0;

    if( LED_ENABLE != led_status && LED_DISABLE != led_status )
    {
        return BSP_ERROR;
    }

    /*读取LED呼吸灯是否使能NV*/
    ret = balong_breath_get_status();
    if( BSP_ERROR == ret  )
    {
        return BSP_ERROR;
    }

    g_powerControl.ulLEDEnable = led_status;
    
    /*变更LED呼吸灯是否使能NV*/
    ret = NVM_Write( NV_POWER_SAVE,  &g_powerControl, sizeof(g_powerControl) );
    if( NV_OK != ret  )
    {
        printk(KERN_ERR "\r\n[ breath led ] breath led write NV failed ID = 0x%x.,ret =%d;\r\n", NV_POWER_SAVE,ret);
        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : balong_breath_led_enable
*
* 功能描述  : 使能呼吸灯
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int balong_breath_led_enable()
{
    int ret = 0;

    if( LED_ENABLE == g_powerControl.ulLEDEnable )
    {
        return BSP_OK;
    }
    
    ret = balong_breath_set_status(LED_ENABLE);
    if( BSP_ERROR == ret )
    {
        return BSP_ERROR;
    }

    /*根据APP传递下来的值，给予LED适当的初始状态*/
    balong_breath_led_ioctl(NULL, NULL, BREATH_LED_ID_POWER, g_ledLastState);
    
    return BSP_OK;
}
/*****************************************************************************
* 函 数 名  : balong_breath_led_disable
*
* 功能描述  : 不使能呼吸灯
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int balong_breath_led_disable()
{
    int ret = 0;

    if( BSP_FALSE == g_powerControl.ulLEDEnable )
    {
        return BSP_OK;
    }
    
    ret = balong_breath_set_status(LED_DISABLE);
    if( BSP_ERROR == ret )
    {
        return BSP_ERROR;
    }

    /*熄灭LED*/
    balong_breath_led_ioctl(NULL, NULL, BREATH_LED_ID_POWER, BREATH_LED_STATE_OFF);

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : balong_breath_led_read
*
* 功能描述  : 读取LED是否使能，使能返回1，不使能返回0
*
* 输入参数  : size_t count
* 输出参数  : int __user *led_state
*
* 返 回 值  : size_t:    读取的字节数
*
* 其它说明  : 无
*
*****************************************************************************/
size_t balong_breath_led_read(struct file *file, int __user *led_state, size_t count)
{
    int ret = 0;

    if ( (NULL != file)) /*for pclint*/
    {
        ;
    }
	
    ret = balong_breath_get_status();
    copy_to_user(led_state,&ret,sizeof(int));
    return sizeof(int);
}

/*****************************************************************************
* 函 数 名  : balong_breath_led_open
*
* 功能描述  : 
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int balong_breath_led_open(struct inode *inode, struct file *file)
{
    if ((NULL != inode) && (NULL != file)) /*for pclint*/
    {
        ;
    }
    
    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : balong_breath_led_SendArg
*
* 功能描述  : LED_DRV模块核间通信
*             将APP下发的点灯参数通过IFC传到C核
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int balong_breath_led_SendArg(unsigned int led_id, unsigned int led_state)
{
    BSP_U32 ret; /*clean lint e713*/
    BREATH_LED_IFC_MSG_STRU stLedIFCMsg = {0};
    
    /*消息体变量赋值*/
    stLedIFCMsg.u32ModuleId = IFC_MODULE_BSP;
    stLedIFCMsg.u32FuncId = IFCP_FUNC_BREATH_LED_CTRL;
    stLedIFCMsg.ulLedId = led_id;
    stLedIFCMsg.ulLedState = led_state;
	
    /*通过IFC将消息发出*/
    ret = BSP_IFC_Send((BSP_VOID*)&stLedIFCMsg, sizeof(BREATH_LED_IFC_MSG_STRU));
    if(BSP_OK != ret)
    {
        printk("------breath led Acpu IFC send fail! ret=0x%x\n", ret);
        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : balong_breath_led_ioctl
*
* 功能描述  : LED_DRV 点灯接口函数
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int balong_breath_led_ioctl(struct inode *inode, struct file *file, unsigned int led_id,
    unsigned int led_state)
{
    BSP_S32 ret;
    
	/*底软调用,这两个参数都为空*/
    if ((NULL == inode) && (NULL == file))
    {
        //for lint
        printk(KERN_ERR "\r\n[ breath led ] balong_breath_led_ioctl called by drv_code!! led_state = %d\r\n",led_state);
    }
#if ( FEATURE_MMI_TEST == FEATURE_ON )	
	else //应用层调用file肯定不为空
	{
	    /*MMI测试模式下不处理APP的命令*/
        if ( mmi_test_mode )
        {
            return BSP_OK;
        }
	}
#endif

    /*入参判断*/ 
	if(BREATH_LED_ID_POWER != led_id)
	{
		return BSP_ERROR;
	}

	if(led_state >= BREATH_LED_STATE_MAX)
	{
		return BSP_ERROR;
	}
    switch( led_state )
    {
        case BREATH_LED_STATE_ENABLE:
            balong_breath_led_enable();
            break;
        case BREATH_LED_STATE_DISABLE:
            balong_breath_led_disable();
            break;
        default:
            /*如果呼吸灯不使能，则不执行动作，直接退出，但被底软调用的情况除外*/
            if( (NULL != inode) && (NULL != file) )
            {
                g_ledLastState = led_state;
                if( LED_DISABLE == g_powerControl.ulLEDEnable && false == led_enable_once)
                {
                    return BSP_OK;
                }
            }
            
			/*点灯参数通过IFC传给C核*/
            ret = balong_breath_led_SendArg(led_id, led_state);
            if(BSP_OK != ret)
            {
                printk("------balong breath led SendArg fail! ret=0x%x\n", ret);
                return BSP_ERROR;
            }
            break;
    }
    return BSP_OK;
}
/*****************************************************************************
* 函 数 名  : balong_breath_led_release
*
* 功能描述  : 
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int balong_breath_led_release(struct inode *inode, struct file *file)
{
    if ((NULL != inode) && (NULL != file)) /*for pclint*/
    {
        ;
    }
    
    return BSP_OK;
}

/*指示灯操作函数数据结构*/
static const struct file_operations balong_breath_led_fops = {
    .owner = THIS_MODULE,
    .ioctl = balong_breath_led_ioctl, /*lint !e64 */
    .open = balong_breath_led_open,
    .release = balong_breath_led_release,
    .read = balong_breath_led_read,
};


/*balong_breath_led_miscdev作为调用misc_register函数的参数，
用于向linux内核注册指示灯misc设备。
*/
static struct miscdevice balong_breath_led_miscdev = {
    .name = "breath_led",
    .minor = MISC_DYNAMIC_MINOR,/*动态分配子设备号（minor）*/
    .fops = &balong_breath_led_fops,
};

/*****************************************************************************
* 函 数 名  : balong_breath_led_init
*
* 功能描述  : LED_DRV A核模块初始化
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
static int __init balong_breath_led_init(void)
{
    int ret = 0;

    ret = misc_register(&balong_breath_led_miscdev);
    if (0 != ret)
    {
        printk("------breath led misc register fail!\n");
        return ret;
    }
    ret = balong_breath_get_status();
    if (BSP_ERROR == ret)
    {
        printk("------breath led read nv fail!\n");
        return ret;
    }

    if( BSP_FALSE == g_powerControl.ulLEDEnable )
    {
        /*熄灭LED*/
        balong_breath_led_ioctl(NULL, NULL, BREATH_LED_ID_POWER, BREATH_LED_STATE_OFF);
    }
    return BSP_OK;
}

module_init(balong_breath_led_init);

/*just for test*/
void BSP_LED_BreTest()
{
    /*BEGIN:y00206456 2012-04-24 Modified for pclint e82*/
    balong_breath_led_ioctl(0, 0, BREATH_LED_ID_POWER, BREATH_LED_STATE_BLINK);
    /*BEGIN:y00206456 2012-04-24 Modified for pclint e82*/
}
void BSP_LED_OnTest()
{
    /*BEGIN:y00206456 2012-04-24 Modified for pclint e82*/
     balong_breath_led_ioctl(0, 0, BREATH_LED_ID_POWER, BREATH_LED_STATE_ON);
    /*BEGIN:y00206456 2012-04-24 Modified for pclint e82*/
}
void BSP_LED_OffTest()
{
    /*BEGIN:y00206456 2012-04-24 Modified for pclint e82*/
	balong_breath_led_ioctl(0, 0, BREATH_LED_ID_POWER, BREATH_LED_STATE_OFF);
    /*BEGIN:y00206456 2012-04-24 Modified for pclint e82*/
}

#endif
/*V3R2 E5 BREATH_LED end*/


