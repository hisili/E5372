/******************************************************************************

                  ��Ȩ���� (C), 2011-2022, ��Ϊ�������޹�˾

******************************************************************************
  �� �� ��   : huawei_proc.c
  �� �� ��   :
  ��    ��   : 
  ��������   : 
  ����޸�   :
  ��������   : 
  �����б�   :

  �޸���ʷ   :
  ��    ��   :
  ��    ��   :
  �޸�����   :
<���ⵥ��>     <��  ��>    <�޸�ʱ��>   <�汾> <�޸�����>
******************************************************************************/

#include "huawei_proc.h"

#define MAX_MSG_LENGTH 128
static struct proc_dir_entry *wifinet_dir;
static struct proc_dir_entry *ssid2_file;
static char msg[MAX_MSG_LENGTH];

unsigned int ssid2_pdev = 0;

/*�ַ���������ת��*/
typedef unsigned int dsat_num_item_type;
typedef  unsigned char      byte;  
typedef enum
{                       
  ATOI_OK,              /*  conversion complete             */
  ATOI_NO_ARG,          /*  nothing to convert              */
  ATOI_OUT_OF_RANGE     /*  character or value out of range */
} atoi_enum_type;
#define MAX_VAL_NUM_ITEM 0xFFFFFFFF /*  Max value of a numeric AT parm     */
#define  UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) )
#define DEC 10
/*****************************************************************************
 �� �� ��  : dsatutil_atoi
 ��������  : ʵ���ַ���������ת��Ϊ����
 �������  : val_arg_ptr ----- ָ�����ڴ洢ת�����
             s----- ԭʼ���ַ���
             r------����
 �������  : ��
 �� �� ֵ  : �ɹ���ʧ��


    �޸�����   : �����ɺ���

*****************************************************************************/
atoi_enum_type huawei_atoi
(
    dsat_num_item_type *val_arg_ptr,     
    const byte *s,     
    unsigned int r    
)
{
    atoi_enum_type err_ret = ATOI_NO_ARG;
    byte c;
    dsat_num_item_type val, val_lim, dig_lim;
    
    val = 0;
    val_lim = (dsat_num_item_type) ((unsigned int)MAX_VAL_NUM_ITEM / r);
    dig_lim = (dsat_num_item_type) ((unsigned int)MAX_VAL_NUM_ITEM % r);

    while ( (c = *s++) != '\0')
    {
        if (c != ' ')
            {
                c = (byte) UPCASE (c);
                if (c >= '0' && c <= '9')
                    {
                        c -= '0';
                    }
                else if (c >= 'A')
                    {
                        c -= 'A' - DEC;
                    }
                else
                    {
                        err_ret = ATOI_OUT_OF_RANGE;  /*  char code too small */
                        break;
                    }
            
                if (c >= r || val > val_lim
                || (val == val_lim && c > dig_lim))
                    {
                        err_ret = ATOI_OUT_OF_RANGE;  /*  char code too large */
                        break;
                    }
                else
                    {
                        err_ret = ATOI_OK;            /*  arg found: OK so far*/
                        val = (dsat_num_item_type) (val * r + c);
                    }
            }
    *val_arg_ptr =  val;
    }
  
    return err_ret;

}

static int wifinet_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = strlen(msg);

    if (off >= len)
    {
        return 0;
    }

    if (count > len - off)
    {
        count = len - off;
    }

    memcpy(page + off, msg + off, count);
    return off + count;
}

static int wifinet_write(struct file *file, const char __user *buffer, unsigned long len, void *data)
{

    if (len >= MAX_MSG_LENGTH) 
    {
        printk("wifinet_write: len(%d) is large!\n",len);
        return -ENOSPC;
    }

    if (copy_from_user(msg, buffer, len )) 
    {
        printk("wifinet_write: copy_from_user error!\n");
        return -EFAULT;
    }

    msg[len] = '\0';

    huawei_atoi(&ssid2_pdev, msg, 10);   //���ַ���ת��Ϊʮ��������
    
    printk("%s(): ---------ssid2_pdev = 0x%x\n", __func__, ssid2_pdev);

    return len;
}

int wifiproc_init(void)
{
    wifinet_dir = proc_mkdir("wifinet", NULL);
    if (!wifinet_dir) 
    {
        printk(KERN_ERR "Can't create /proc/wifinet\n");
        return -1;
    }

    ssid2_file = create_proc_entry("ssid2", 0666, wifinet_dir);
    if (!ssid2_file) 
    {
        printk(KERN_ERR "Can't create /proc/wifinet/ssid2\n");
        remove_proc_entry("wifinet", NULL);
        return -1;
    }

    ssid2_file->read_proc = wifinet_read;
    ssid2_file->write_proc = wifinet_write;

    return 0;
}

void wifiproc_exit(void)
{
    remove_proc_entry("ssid2", wifinet_dir);
    remove_proc_entry("wifinet", NULL);
}


