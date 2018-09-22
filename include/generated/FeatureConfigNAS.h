/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : FeatureConfigNAS.h
  版 本 号   :
  作    者   :
  生成日期   :
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   :
    作    者   :
    修改内容   :
  2.日    期   : 2008-09-10
    修改内容   : 问题单号:AT2D05643
  3.日    期   : 2008年10月10日
    修改内容   : 问题单号：AT2D06152（AT2D06151）, USIMM优化合入后，SMSP文件相关读写操作失败
  4.日    期   : 2008-11-17
    修改内容   : 问题单号:AT2D06780, 短信状态报告的读，删除和上报接口与终端工具需求不符问题
  5.日    期   : 2008-12-02
    修改内容   : 问题单号：AT2D07148，增加GCF测试用桩CMSTUB命令
  6.日    期   : 2009年4月7日
    修改内容   : 问题单号 AT2D06392, 不支持短信和状态报告的NV存储相关代码通过宏开关关闭
  7.日    期   : 2009-04-20
    修改内容   : 问题单号：AT2D11131/AT2D11170,H3G需求,对于不在USIM指定优先级PLMN范围内的其它网络，W的高质量PLMN排的在前面
  8.日    期   : 2009年5月10日
    修改内容   : AT2D12319, NAS R6升级；
  9.日    期   : 2009年6月18日
    修改内容   : 问题单号：AT2D12361,出覆盖区后再回来，需要大概5分钟才能重新搜到网
 10.日    期   : 2010年1月26日
    修改内容   : 问题单号AT2D16564
                 PS 域短信连发功能未启用，导致GCF用例不过；需要增加AT 命令启
                 用PS域短信连发功能，方便测试；
 11.日    期   : 2010年6月10日
******************************************************************************/
#ifndef __FEATURECONFIGNAS_H__
#define __FEATURECONFIGNAS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "FeatureConfig.h"


/* SIM卡Call Control业务 */
#define NAS_FEATURE_SIM_CALL_CTRL                           FEATURE_OFF

/* 从(U)SIM卡中读取CSMP参数 */
#define NAS_FEATURE_GET_CSMP_PARA_FROM_USIM                 FEATURE_OFF

/* SIM卡MO SMS Control业务 */
#define NAS_FEATURE_SIM_MO_SMS_CTRL                         FEATURE_OFF

/*表明是否支持短信的PP-DOWNLOAD功能*/
#define NAS_FEATURE_SMS_PP_DOWNLOAD                         FEATURE_ON

/*表明NVIM中是否能保存短信*/
#define NAS_FEATURE_SMS_NVIM_SMSEXIST                       FEATURE_ON
#define NAS_FEATURE_SMS_FLASH_SMSEXIST                      FEATURE_ON

/*表明NVIM中是否能保存短信状态报告*/
#define NAS_FEATURE_SMS_NVIM_SMSREXIST                      FEATURE_OFF

/*表明NVIM中是否能保存广播消息*/
#define NAS_FEATURE_SMS_NVIM_CBMEXIST                       FEATURE_OFF

/*表明是否支持短信的CB-DOWNLOAD功能*/
#define NAS_FEATURE_SMS_CB_DOWNLOAD                         FEATURE_OFF

/* 支持短信状态报告存到EFSMS文件*/
#define NAS_FEATURE_SMS_STATUS_REPORT_IN_EFSMS              FEATURE_ON

/* CCBS(遇忙呼叫完成)业务 */
#define NAS_FEATURE_CCBS                                    FEATURE_ON


/* 支持呼叫偏转业务 */
#define NAS_FEATURE_CALL_DEFLECTION                         FEATURE_ON

/*支持线路切换业务*/
#define NAS_FEATURE_ALS                                     FEATURE_ON

/* ECT(显式呼叫转移)业务 */
#define NAS_FEATURE_ECT                                     FEATURE_ON


/* AoC(计费通知)业务 */
#define NAS_FEATURE_AOC                                     FEATURE_OFF


/* Call Control业务 */
#define NAS_FEATURE_CALL_CONTROL                            FEATURE_ON


/* 支持呼叫中业务修改 */
#define NAS_FEATURE_IN_CALL_MODIFY                          FEATURE_ON


/* 支持语音传真交替 */
#define NAS_FEATURE_ALTER_V_F                               FEATURE_OFF


/* 声码器等语音设备由协议栈控制 */
#define NAS_FEATURE_LOCAL_VOCODER                           FEATURE_ON


/* 支持SMS(短消息业务) */
#define NAS_FEATURE_SMS                                     FEATURE_ON

/* 锁卡功能 */
#define NAS_FEATURE_SUPPORT_SIM_LOCK                        FEATURE_OFF

/* 锁运营商功能 */
#define NAS_FEATURE_SUPPORT_OPERATOR_LOCK                   FEATURE_ON

/* SAT位置报告功能 */
#define NAS_FEATURE_SUPPORT_SAT_LOCATION_STATUS_EVENT       FEATURE_OFF

/* H3G的需求 */
#define NAS_FEATURE_SUPPORT_H3G_REQ                         FEATURE_ON

/* RAU优化特性开关，目前NAS只在V1R2分支维护，而V1R2不需要合入该特性，因此用该宏隔开，如果V1R2分支不再维护，该宏可以去掉,2009-05-25 */
#define NAS_FEATURE_RAU_FOR_V2R1                            FEATURE_ON

/* Lu、Attach、Rau Reject #12后，是否启动 Available Timer 定时器，定时器超时后发起搜网 */
#define NAS_FEATURE_REGISTER_LA_NOT_ALLOW_INIT_PLMN_SEARCH  FEATURE_ON

/* 实现可配置特性默认服务域，CGCLASS命令仅支持设置为A/B模式，而且没有使用NV记录 */
#define NAS_FEATURE_AT_COMMAND_CGCLASS                      FEATURE_OFF

/* VC模块VOICE CONTROL功能 */
#define NAS_FEATURE_CS_VC                                   FEATURE_OFF

/* CS+PS并发场景，是否触发PDP MODIFY流程进行流控，防止CPU过载复位 */
#define NAS_FEATURE_CS_PS_FLOW_CONTROL_BY_PDP_MODIFY        FEATURE_OFF

/* 服务域设置为PS ONLY时，是否支持CS域短信和呼叫业务(紧急呼叫除外) */
#define NAS_FEATURE_PS_ONLY_CS_SERVICE_SUPPORT              FEATURE_OFF

#define FEATURE_AT_PORTING                                  FEATURE_OFF

/*针对modem端口增加特性宏的控制*/
#define NAS_FEATURE_MODEM                                   FEATURE_ON
/*针对黑白名单需求，增加特性宏控制*/
#define NAS_FEATURE_BLACK_WHITE_PLMN                        FEATURE_OFF

/*针对DOCK透传命令需求，增加特性宏控制*/
#define NAS_FEATURE_DOCK                                    FEATURE_OFF

/*针对SD卡自动升级需求，增加特性宏控制 */ 
#define NAS_FEATURE_SD_UPDATE                               FEATURE_ON

/*针对日本BVT问题作的修改，增加特性宏控制*/
#define NAS_FEATURE_BVT_TBAR                                FEATURE_OFF

/*针对E5的记录历史流量需求，增加特性宏控制*/
#define NAS_FEATURE_HISTORY_FLUX                            FEATURE_OFF

/*针对日本HPLMN注册被拒需求，增加特性宏控制*/
#define NAS_FEATURE_HPLMN_REG_REJ                           FEATURE_OFF

/*针对AT口密码保护需求，增加特性宏控制*/
#define NAS_FEATURE_AT_PWD                                  FEATURE_ON

/*针对SD设备密码保护需求，增加特性宏控制*/
#define NAS_FEATURE_SD_PWD                                  FEATURE_ON

/*针对按需拨号特性需求，增加特性宏控制*/
#define NAS_FEATURE_DEMAND_DIAL_MODE                        FEATURE_OFF

/*闪电卡中增加网关特性*/
#define NAS_FEATURE_GATEWAY                                 FEATURE_OFF

#define FEATURE_BALONGV7R_ADAPT_BALONGV3R1                  FEATURE_OFF


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __FEATURECONFIGNAS_H__ */
