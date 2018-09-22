/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_AUDIO.h
*
*   ��    �� :  
*
*   ��    �� :  AUDIOģ���û��ӿ��ļ�
*
*   �޸ļ�¼ :  2012��04��16��  v1.00   ����
*************************************************************************/

#ifndef    _BSP_AUDIO_H_
#define    _BSP_AUDIO_H_

#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */



/********************************************************************************************************
 �� �� ��  : BSP_AUDIO_CodecOpen
 ��������  : ��һ��audio�豸������codec���г�ʼ����
 �������  : devname���ò����̶�Ϊ"/dev/codec0"��
                           flags���ò�����ʾ��д��ʽ���̶�Ϊ0��
                           mode���ò�����ʾ�½��ļ���ʽ���̶�Ϊ0����
  �������  : �ޡ�
 �� �� ֵ  : �Ǹ�ֵ:  �豸�����
             -1������ʧ�ܡ�
********************************************************************************************************/
extern int BSP_AUDIO_CodecOpen(const char *devname, int flags, int mode);
#define DRV_CODEC_OPEN(devname, flags, mode)    BSP_AUDIO_CodecOpen(devname, flags, mode)

/********************************************************************************************************
 �� �� ��  : BSP_AUDIO_Codec_ioctl
 ��������  : codec�豸�����ֿ��ơ�
 �������  : devid���豸�����
                           cmd�������֡�
                           arg�����������
  �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
********************************************************************************************************/
extern int BSP_AUDIO_Codec_ioctl(int devid, int cmd, int arg);
#define DRV_CODEC_IOCTL(devid, cmd, arg)   BSP_AUDIO_Codec_ioctl(devid, cmd, arg)

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif    /* End #define _BSP_IPF_H_ */

