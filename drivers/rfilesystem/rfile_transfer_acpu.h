#ifndef __RFILE_TRANSFER_ACPU_H__
#define __RFILE_TRANSFER_ACPU_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************
  宏定义
**************************************************************************/
#define TRANS_MAX_SIZE (1024)
#define UDI_ACM_HSIC_ID  27
/**************************************************************************
  结构体定义
**************************************************************************/
typedef struct RFILECONTEXT_A_TAG
{
    int               iccOpened;
	int               hsicOpened;
	UDI_HANDLE        rFileIccFd;
	UDI_HANDLE        rFileHsic9Fd;
	ICC_CHAN_ATTR_S   attr;
	UDI_OPEN_PARAM    rFileIccParam;
	UDI_OPEN_PARAM    rFileHsic9Param;
	void *            sendBuf;
	struct semaphore  ap2mpSem; 
	struct semaphore  mp2apSem;
}RFILECONTEXT_A;

typedef struct
{
	int opsType;
	int pathLength;
	int modeLength;
}RFILE_OPEN_HEAD;

typedef struct
{
	unsigned int opsPid;
	int totalFrames;
	int frameNumb;
	int curFrameLen;
	int reserve;
}RFILE_MNTN_HEAD;
/**************************************************************************
  函数声明
**************************************************************************/
void rfile_hsic_read_cb(void);
void rfile_icc_write_cb(void);
void rfile_icc_read_cb(unsigned int chenalId,unsigned int u32size);

#ifdef __cplusplus
}
#endif

#endif   // __RFILE_TRANSFER_ACPU_H__

