/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_AUDIO.h
*
*   作    者 :  
*
*   描    述 :  AUDIO模块用户接口文件
*
*   修改记录 :  2012年04月16日  v1.00   创建
*************************************************************************/

#ifndef    _BSP_AUDIO_H_
#define    _BSP_AUDIO_H_

#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */



/********************************************************************************************************
 函 数 名  : BSP_AUDIO_CodecOpen
 功能描述  : 打开一个audio设备，并对codec进行初始化。
 输入参数  : devname：该参数固定为"/dev/codec0"。
                           flags：该参数表示读写方式，固定为0。
                           mode：该参数表示新建文件方式，固定为0。。
  输出参数  : 无。
 返 回 值  : 非负值:  设备句柄；
             -1：操作失败。
********************************************************************************************************/
extern int BSP_AUDIO_CodecOpen(const char *devname, int flags, int mode);
#define DRV_CODEC_OPEN(devname, flags, mode)    BSP_AUDIO_CodecOpen(devname, flags, mode)

/********************************************************************************************************
 函 数 名  : BSP_AUDIO_Codec_ioctl
 功能描述  : codec设备命令字控制。
 输入参数  : devid：设备句柄。
                           cmd：命令字。
                           arg：命令参数。
  输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int BSP_AUDIO_Codec_ioctl(int devid, int cmd, int arg);
#define DRV_CODEC_IOCTL(devid, cmd, arg)   BSP_AUDIO_Codec_ioctl(devid, cmd, arg)

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif    /* End #define _BSP_IPF_H_ */

