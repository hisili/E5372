
#ifndef _MMI_H
#define _MMI_H
/*******************************************************
�궨��
*******************************************************/
/**********************���ⵥ�޸ļ�¼*****************************************************
��    ��              �޸���                     ���ⵥ��                   �޸�����
***************************************************************************************/
/* MMIʹ��power ����������ʱ��(ms),�������ΰ��µ�ʱ�������� */
/*begin modified by wanghaijie for mmitest*/
#define MMI_POWER_TIMER 1000 
/*end modified by wanghaijie for mmtest*/
/*��������ʱ�������,�������3000�������Ϊ����������*/
#define MMI_KEY_TIME_DEF (3000) 

/* BEIGN ,Added by xuchao,2012/3/21 */
#if ( FEATURE_E5 == FEATURE_ON ) && (defined(VERSION_V3R2))
#define en_NV_Item_SW_VERSION_FLAG 36
#endif

#if ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))\
&& (defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))
#define en_NV_Item_SW_VERSION_FLAG 36
#endif /*((defined (CHIP_BB_6920ES)...*/
/* END   ,Added by xuchao,2012/3/21 */

#define NV_FACTORY_INFO_I 114
#define NV_FACTORY_INFO_I_SIZE 78
#define MMI_TEST_FLAG_OFFSET 24
#define MMI_TEST_FLAG_SUCCESS "ST P"
#define MMI_TEST_FLAG_FAILED "ST F"
#define MMI_TEST_BEGIN "MMI TEST"
#define MMI_TEST_SUCCESS "MMI TEST SUCCESS!"
#define USIM_TEST_PASS "SIM PASS"
#define USIM_TEST_FAIL "SIM FAIL"

#define MMI_TEST_READY "MMI TEST READY"
#define MMI_TEST_READY_TIMEOUT ( 5*60*1000 )  //ms 


/* ����ǰ���¶���ʾ�ַ�
    ͨ������ʾ"ADC Pass"
    ʧ�ܣ���ʾ"ADC Fail" 
*/
#if((FEATURE_ON == FEATURE_E5376_MMI) || (FEATURE_ON == FEATURE_E5371_MMI))
#define MMI_ADC_TEST_PASS "ADC Pass"
#define MMI_ADC_TEST_FAIL "ADC Fail"
#define BSP_MMI_ADC_TIMEOUT (2000)
#define MMI_ADC_STATUS_QUERY (0x01)
#define X_POINT  32
#define Y_POINT  58
#define SLEEP_TIME 20
#define MSG_LEN 4
#define MSG_SECOND 2
#define MSG_THIRD  3
#endif

#define MMI_TEST_TIMEOUT ( 5*60*1000 )  //ms 

#define MMI_TEST_MNT_DIR "/mnt/sdcd0"
#define MMI_TEST_SD_DIR "/dev/block/mmcblk%d"
#define MMI_TEST_SD_DIR_P1 "/dev/block/mmcblk%dp1"

#define MMI_NV_MAGIC 0x1234ABCD

#define CARD_OK     0x00  /*USIM������������ʹ��*/
#define CARD_ERROR 0x01  /*USIM���쳣�������޿�*/
#define BSP_MMI_USIM_TIMEOUT (2000)
#define MMI_USIM_STATUS_QUERY (0x01)

/*******************************************************
��Ļ��ʾ��״̬
*******************************************************/
typedef enum _OLED_DIS_STA
{
    OLED_DIS_BLACK = 0,
    OLED_DIS_WHITE,
    OLED_DIS_IMAGE,
    OLED_DIS_MAX    
}OLED_DIS_STA;

/*******************************************************
���������¼��ṹ��
*******************************************************/
typedef struct _KEY_TEST_EVENT
{
    unsigned char pwr_key_cnt;
    unsigned char wps_key_cnt;
    unsigned char rst_key_cnt;
    unsigned char reserved;
    unsigned int timeout;
    unsigned int key_timeout;
    struct completion mmi_test_start;	
    struct completion key_to_test;	
} KEY_TEST_EVENT;

/*******************************************************
MMI����ǰ���¶�״̬
MMI_ADC_TEMP_OK  : �¶����� 
MMI_ADC_TEMP_FAIL: �¶�����
*******************************************************/
#if((FEATURE_ON == FEATURE_E5371_MMI))
typedef enum _MMI_ADC_STA
{
    MMI_ADC_TEMP_OK       = 0,
    MMI_ADC_TEMP_FAIL     = 1,
    MMI_ADC_TEMP_MAX,
}MMI_ADC_STA;
#endif /* FEATURE_ON == FEATURE_E5371_MMI */

typedef enum _USIM_EXIST_STA
{
    MMI_USIM_NOT_EXIST =0,
	MMI_USIM_EXIST     =1,
	MMI_USIM_MAX,
}MMI_USIM_STA;

/*******************************************************
MMI�����ļ���״̬
*******************************************************/
typedef enum _MMI_FILE_STA
{
    MMI_SD_NOT_INIT = 0,
    MMI_FILE_NOT_EXIST,
    MMI_FILE_OK,
    MMI_FILE_MAX,
}MMI_FILE_STA;

extern KEY_TEST_EVENT mmi_test_event;
extern unsigned int mmi_test_mode;
extern void oledUpdateDisplay(int UpdateStatus);
#endif






