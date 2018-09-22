/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcCommon.c
* Description:
*                sleep managerment
*
* Author:   	Íôçâ
* Date:          2011-09-28
* Version:       1.0
*
*
*
* History:
* Author:	Íôçâ
* Date:			2011-09-28
* Description:	Initial version
*
*******************************************************************************/
#include <generated/FeatureConfig.h>

typedef unsigned long long jiff;

#define NULL ((void *)0)

#define restrict __restrict__

#define BUFFSIZE (64*1024)
#define DPM_CLIENT_ID_MASK  0x3
#define MAX_MSG_DFS (10)
#define DFS_OK (0)
#define DFS_ERROR (-1)
#define DFS_MSG_OK (0)
#define DFS_MSG_ERROR (-1)
#define DFS_NO_WAIT (0)
#define DFS_WAIT_FOREVER (-1)
#define DFS_NULL (0)
#define DFS_FASE (0L)
#define DFS_TRUE (1L)
#define DFS_NV_OK (0)
#define DFS_EXEC_OK (0)

#define DFS_VOTE_MSG 1
#define DFS_TIMEOUT_MSG 2

#define OSAL_MSG_PRI_NOMAL  0
#define OSAL_MSG_PRI_URGENT  1

#define DFS_ACPU (0)
#define DFS_CCPU (1)
#define DFS_PROFILE_NUM   3  //133/133/133/67ÏÈ²»ÓÃ


typedef struct
{
	unsigned int ulMsgName;
	unsigned int ulClient;
	unsigned int enVoteProfile;
	unsigned int enVoteType;
	unsigned int ulCoreID;
}DFS_MSG_STRU;


typedef struct 
{
	unsigned int module_id;
	unsigned int func_id;
	unsigned int result;
	DFS_MSG_STRU pstVoteMsg;
}DFS_IFC_MSG_STRU;


typedef enum
{
	DFS_SUCCESS = 0,
	DFS_FAILURE,
	DFS_DPM_IS_STOP = 0x100,
	DFS_CLIENT_NAME_IS_EXIST,
	DFS_CLIENT_REGISTER_IS_FULL,
	DFS_PARA_MALLOC_ERROR,
	DFS_PARA_INVAILD,
	DFS_PARA_NULL,
	DFS_MMI_REG_BEFORE,
	DFS_MMI_REG_FULL
}DFS_ERROR_ID_ENUM;

typedef enum
{
	DFS_VOTE_CLEAN = 0,
	DFS_VOTE_SET = 1,
	DFS_VOTE_TYPE_BUTT
}DFS_VOTE_TYPE_ENUM;

typedef struct
{
	unsigned short usProfileUpLimit;
	unsigned short usProfileDownLimit;
}DFS_PROFILELIMIT_STRU;

typedef struct
{
	unsigned int ulDFSFunEnableFlag;
	unsigned int ulTimerLen;
	unsigned short usProfileUpTime;
	unsigned short usProfileDownTime;
	unsigned int ulProfileNum;
	DFS_PROFILELIMIT_STRU astThresHold[DFS_PROFILE_NUM];
}DFS_CONFIGINFO_STRU;

void PWRCTRL_DfsAcpuInit(void);
void PWRCTRL_DfsIfcGetAcpuLoad(void *para, unsigned int len );
unsigned int PWRCTRL_DfsSetProfileAcpu(PWC_DFS_ID_E ulClientID, PWC_DFS_PROFILE_E enProfile);
unsigned int PWRCTRL_DfsReleaseProfileAcpu(PWC_DFS_ID_E ulClientID);

