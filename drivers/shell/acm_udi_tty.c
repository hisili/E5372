/* (c) 2011 Jungo Ltd. All Rights Reserved. Jungo Confidential */
//#include "includes.h"
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/poll.h>
#include <linux/console.h>
#include <linux/kthread.h>
#include <linux/module.h>
//#include <jos.h>
//#include <jos_sync.h>
//#include <jusb_common.h>
//#include<jusb_core_fd.h>
//#include <cdc_fd.h>
#include "BSP.h"
#include <mach/common/bsp_memory.h>
#include "acm_udi_tty.h"

#define ASHELL_TTY_PROTECTION_TIMEOUT 200

static struct tty_driver *tty_driver;
static uint32_t acm_open_fd;
ashell_ctx_s ashell_acm_ctx = {0};

#define ACM_READY(acm)  (acm->enable)

#ifdef CONFIG_USB_G_SERIAL_CONSOLE
static void gs_console_write(struct console *cons, const char *buf, unsigned count);
void gs_console_register(ashell_ctx_s* acm_ctx);

struct tty_driver *gs_console_device(struct console *cons, int *gidx);
static int gs_console_setup (struct console *cons, char *options);
#endif

#ifdef DEBUG
#if 0
static void dump_line_state(uint16_t minor, uint16_t state)
{
    printk("ACM: #, serial state [dcd: %d, dsr: %d, brk: %d, "
        "rng: %d, frm_err: %d, par_err: %d, ovrn_err: %d]\n",
        state & (ACM_SERIAL_DCD) ? 1 : 0,
        state & (ACM_SERIAL_DSR) ? 1 : 0,
        state & (ACM_SERIAL_BREAK) ? 1 : 0,
        state & (ACM_SERIAL_RING_SIG) ? 1 : 0,
        state & (ACM_SERIAL_FRAMING_ERROR) ? 1 : 0,
        state & (ACM_SERIAL_PARITY_ERROR) ? 1 : 0,
        state & (ACM_SERIAL_OVERRUN_ERROR) ? 1 : 0);
}
#endif
#endif

extern void acm_tty_read(void *arg,char* buffer);
static void acmdev_read_done(void)
{
    ashell_ctx_s* acm_ctx = &ashell_acm_ctx;
    ACM_WR_ASYNC_INFO stRWInfo = {0};

    if (udi_ioctl(acm_open_fd, ACM_IOCTL_GET_RD_BUFF, &stRWInfo))
    {
        printk("ACM: ACM_IOCTL_GET_RD_BUFF failed\n");
        return;
    }

    acm_ctx->read_pending = stRWInfo.u32Size;
    acm_tty_read(acm_ctx,stRWInfo.pBuffer);

    if (udi_ioctl(acm_open_fd, ACM_IOCTL_RETURN_BUFF, &stRWInfo))
    {
        printk("ACM: ACM_IOCTL_RETURN_BUFF failed\n");
    }

    printk(KERN_DEBUG "ACM: acmdev_read_done\n");
}

static void acmdev_event_notify(ACM_EVT_E enEvt)
{
    ashell_ctx_s* acm_ctx = &ashell_acm_ctx;

    acm_ctx->is_acm_ready = (ACM_EVT_DEV_READY == enEvt) ? (1):(0);
    return;
}

void acm_tty_read(void *arg,char* buffer)
{
    ashell_ctx_s* acm_ctx = (ashell_ctx_s *)arg;
    struct tty_struct *tty;
    unsigned long flags;
    uint16_t off = 0;
    char *data;
    uint32_t pieces_size;

    if (!ACM_READY(acm_ctx))
    {
        printk("ACM_READY not ok\n");

        if (acm_ctx && acm_ctx->read_pending)
        {
            acm_ctx->read_pending = 0;
        }
        else
        {
            printk("ACM: not ready\n");
        }
        
        return;
    }

    tty = acm_ctx->tty;
    off = 0;
    data = buffer;
    pieces_size = acm_ctx->read_pending;

    if(NULL == tty)
    {
        printk("ACM: tty is NULL\n");
        return;
    }

    if (acm_ctx->read_pending && !acm_ctx->throttle)
    {
        off = tty_insert_flip_string(tty, (uint8_t*) data, pieces_size);
        if(off)
        {
            tty_flip_buffer_push(tty);
        }
    }
    flags = 0;
    spin_lock_irqsave(&acm_ctx->lock, flags);
    if (acm_ctx->throttle)
    {
        acm_ctx->throttled_in_read = 1;
        if (off)
        {
            acm_ctx->read_pending -= off;
            memmove(data, data + off, acm_ctx->read_pending);
        }

        spin_unlock_irqrestore(&acm_ctx->lock, flags);

       
        return;
    }
    spin_unlock_irqrestore(&acm_ctx->lock, flags);

    acm_ctx->read_pending = 0;

}

static int acm_tty_open(struct tty_struct *tty, struct file *filp)
{
    int rc = OK;

    ashell_ctx_s* acm_ctx = &ashell_acm_ctx;

    printk("enter tty open \n");
    if (!acm_ctx)
    {
        printk("ACM: #%d, tty open - no such tty\n", tty->index);
        return ERROR;
    }

    if(NULL == tty)
    {
        printk("acm_tty_open tty is NULL \n");
        return ERROR;
    }

    tty->driver_data = acm_ctx;
    tty->low_latency = 1;

    /* Get in sync with disconnect, tty_open/tty_close are already synchronized
     * by the upper TTY layer */
    printk("ACM: #%d, acm_tty_open, used is %d\n", tty->index, acm_ctx->used);
    if (acm_ctx->used++)
    {
        ASSERT(acm_ctx->tty == tty);
        goto Exit;
    }

    acm_ctx->tty = tty;

    acm_ctx->ctrlout = ACM_CONTROL_SIG_DTR | ACM_CONTROL_SIG_RTS;

Exit:
    if (rc)
    {
        acm_ctx->used--;
    }
    else
    {
        printk("ACM: #%d, opened, %d\n", tty->index, acm_ctx->used);
    }

    return OK;
}

static void acm_tty_close(struct tty_struct *tty, struct file *filp)
{
    ashell_ctx_s* acm_ctx;

    if (!tty || !tty->driver_data)
    {
        printk("ACM: #%d, tty close, file %p\n",
            tty ? tty->index : -1, filp);
        return;
    }

    acm_ctx = tty->driver_data;

    if (!acm_ctx)
    {
        printk("ACM: tty is NULL\n");
        return;
    }
    
    acm_ctx->close_in_progress = 1;
    acm_ctx->read_pending = 0;

    printk("ACM: #%d, tty close, used %d\n", tty->index, acm_ctx->used);

    ASSERT(acm_ctx->used);
    if (--acm_ctx->used)
    {
        goto Exit;
    }

    acm_ctx->ctrlout = 0;
    acm_ctx->tty = NULL;

    printk("ACM: #%d, tty udi_close\n", tty->index);

Exit:
    acm_ctx->close_in_progress = 0;
}

static void ashell_write_thread(ashell_ctx_s* acm_ctx)
{
    int read_temp;
	uint32_t write_point;
	unsigned long flags;
	int write_flag = 2;

    for(;;)
    {
        //down(&(acm_ctx->write_sem));
        wait_event((acm_ctx->ashell_write_wait), acm_ctx->is_ashell_write);
        acm_ctx->is_ashell_write = FALSE;
        flags = 0;
        spin_lock_irqsave(&acm_ctx->lock, flags);
        if(acm_ctx->send_write_point > acm_ctx->send_read_point)
        {
            read_temp = acm_ctx->send_read_point;
            acm_ctx->send_read_point = acm_ctx->send_write_point;
            write_flag = 1;
            write_point = acm_ctx->send_write_point;
        }
        else if(acm_ctx->send_write_point < acm_ctx->send_read_point)
        {
            read_temp = acm_ctx->send_read_point;
            acm_ctx->send_read_point = 0;
            write_flag = 0;
            write_point = acm_ctx->send_write_point;
            acm_ctx->send_read_point = acm_ctx->send_write_point;
        }
        spin_unlock_irqrestore(&acm_ctx->lock, flags);

        if(1 == write_flag)
        {
            (void) udi_write(acm_open_fd,acm_ctx->write_buf+read_temp,write_point-read_temp);  
        }
        else if(0 == write_flag)
        {
            (void) udi_write(acm_open_fd,acm_ctx->write_buf+read_temp,ACM_TTY_SHELL_BUFFER_SIZE-read_temp);
            (void) udi_write(acm_open_fd,acm_ctx->write_buf,write_point);	
        }

        if (acm_ctx->tty)
        {
            tty_wakeup(acm_ctx->tty);
            acm_ctx->is_tty_wakeup = 1;
            //cancel_delayed_work_sync(&acm_ctx->tty_wake_work);
        }
    }
}

int usb_mem_char_cpy(unsigned char *_dest,const unsigned char *_src,int _n, int _limit,int *_p_dst_end)
{
    int i = 0;
    unsigned char *_d = _dest;
    const unsigned char *_s = _src;
    int cpys = 0;
    int dst_end;
    dst_end = (int)_dest + _limit;

    while((i < _n) && ((int)_d < (int)dst_end))
    {
        if(*_s != '\n')
        {
            *_d ++ = *_s++;
            i++;
        }
        else
        {
            if((int)_d == ((int)dst_end-1))
            {
                *(_d-1)= '\r';
                *_d++ = '\n';
            }
            else
            {
                *_d++ = '\r';
                *_d++ = '\n';
            }
            i++;
            _s++;
        }
        cpys++;
    }

    *_p_dst_end = (int)_d;
    return cpys;
}
static int acm_tty_write(struct tty_struct *tty,const unsigned char *buf, int count)
{
    ashell_ctx_s* acm_ctx;
    unsigned long flags;
    int fst_count;
    int dst_end;
    uint32_t sec_count;
    int cpy_limit;
    int cpy_count;

    if(acm_open_fd <= 0)
    {
       printk(KERN_DEBUG "ACM: acm_open_fd is NULL\n");
       return -EINVAL; 
    }

    if(NULL == tty)
    {
        printk(KERN_DEBUG "ACM: tty is NULL\n");
        return -EINVAL;
    }
    if(NULL == buf)
    {
        printk(KERN_DEBUG "ACM: buf is NULL\n");
        return -EINVAL;
    }
    acm_ctx = tty->driver_data;
    if (!ACM_READY(acm_ctx))
    {
        /* PC 端串口没有打开直接丢数 */
        return count;
    }

    if (!count)
    {
        //printk(KERN_DEBUG "ACM: #%d, zero write count\n", tty->index);
        return count;
    }
    flags = 0;
    
    if (acm_ctx->enable)
    {
        spin_lock_irqsave(&acm_ctx->lock, flags);
        cpy_limit = ACM_TTY_SHELL_BUFFER_SIZE-acm_ctx->send_write_point;
        cpy_count = (count <= cpy_limit)?count : (cpy_limit);
        fst_count = usb_mem_char_cpy(acm_ctx->write_buf+acm_ctx->send_write_point,buf,cpy_count,cpy_limit,&dst_end);
        if(count != fst_count)
        {
            sec_count = (count - fst_count);
            sec_count = (sec_count <= acm_ctx->send_write_point)? sec_count : acm_ctx->send_write_point;
            usb_mem_char_cpy(acm_ctx->write_buf,buf + fst_count , sec_count,acm_ctx->send_write_point,&dst_end);
        }
        acm_ctx->send_write_point = dst_end-(int)acm_ctx->write_buf;	  
		acm_ctx->is_ashell_write = TRUE;
        acm_ctx->is_tty_wakeup = 0;

        spin_unlock_irqrestore(&acm_ctx->lock, flags);

        wake_up(&(acm_ctx->ashell_write_wait));
        schedule_delayed_work(&acm_ctx->tty_wake_work, ASHELL_TTY_PROTECTION_TIMEOUT);
    }

    return count;
}

static int acm_tty_write_room(struct tty_struct *tty)
{
    ashell_ctx_s* acm_ctx = tty->driver_data;
    int write_room;
    unsigned long flags;
    
    if (acm_open_fd <= 0 || !ACM_READY(acm_ctx))
    {
        //printk(KERN_DEBUG ">>>>>>>>>>>>acm_tty_write_room error");
        return ACM_TTY_SHELL_BUFFER_SIZE;
    }
    flags = 0;
    spin_lock_irqsave(&acm_ctx->lock, flags);
    if(acm_ctx->send_write_point >= acm_ctx->send_read_point)
    {
        write_room = ACM_TTY_SHELL_BUFFER_SIZE - (acm_ctx->send_write_point -acm_ctx->send_read_point);
    }
    else
    {
        write_room = acm_ctx->send_read_point - acm_ctx->send_write_point;
    }
    spin_unlock_irqrestore(&acm_ctx->lock, flags);
    return write_room;
}

int printk_test(void)
{
    for(;;)
    {
        printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
		printk(">>>>>>>>>>>>>>>>>>Ashell printk test>>>>>>>>>>>>>>>>>>> \n");
        msleep(1);
    }
}
static int acm_tty_chars_in_buffer(struct tty_struct *tty)
{
    ashell_ctx_s* acm_ctx;
    uint16_t char_in_buffer = 0;
	unsigned long flags;
    if(NULL == tty)
    {
        printk(KERN_DEBUG ">>>>>>>acm_tty_chars_in_buffer tty is NULL \n");
        return 0;
    }
    acm_ctx = tty->driver_data;
    if (!ACM_READY(acm_ctx))
    {
        printk(KERN_DEBUG ">>>>>>>acm_tty_chars_in_buffer error return \n");
        return 0;
    }
    /* TODO: check if we need to return the pending write or pending read */
    flags = 0;
    spin_lock_irqsave(&acm_ctx->lock, flags);
    if(acm_ctx->send_write_point >= acm_ctx->send_read_point)
    {
        char_in_buffer = acm_ctx->send_write_point - acm_ctx->send_read_point;
    }
    else
    {
        char_in_buffer = acm_ctx->send_write_point + ACM_TTY_SHELL_BUFFER_SIZE - acm_ctx->send_read_point;
    }
    spin_unlock_irqrestore(&acm_ctx->lock, flags);
    /* TODO: check if we need to return the pending write or pending read */
    //DBG_X(DPORT_ACM, ("ACM: #%d, tty chars in buffer %d\n", tty->index,char_in_buffer));
    return char_in_buffer;
}

static void acm_tty_throttle(struct tty_struct *tty)
{
    ashell_ctx_s* acm_ctx = tty->driver_data;
    unsigned long flags;

    printk(KERN_DEBUG "ACM: #%d, tty throttle\n", tty->index);

    if (!ACM_READY(acm_ctx))
        return;
    
    flags = 0;    
    spin_lock_irqsave(&acm_ctx->lock, flags);
    acm_ctx->throttle = 1;
    spin_unlock_irqrestore(&acm_ctx->lock, flags);
}

static void acm_tty_unthrottle(struct tty_struct *tty)
{
    ashell_ctx_s* acm_ctx = tty->driver_data;
    int throttled_in_read;
    unsigned long flags;

    printk(KERN_DEBUG "ACM: #%d, tty unthrottle\n", tty->index);

    if (!ACM_READY(acm_ctx))
        return;
    
    flags = 0;
    spin_lock_irqsave(&acm_ctx->lock, flags);
    throttled_in_read = acm_ctx->throttled_in_read;
    acm_ctx->throttled_in_read = 0;
    acm_ctx->throttle = 0;
    spin_unlock_irqrestore(&acm_ctx->lock, flags);

    if (throttled_in_read)
    {
        printk(KERN_DEBUG "ACM: #%d, reschedule read\n", tty->index);
    }
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
static int
#else
static void
#endif
acm_tty_break_ctl(struct tty_struct *tty, int state)
{
    int rc = ERROR;
    ashell_ctx_s* acm_ctx = tty->driver_data;

    printk(KERN_DEBUG "ACM: #%d, tty break ctl state %d\n", tty->index, state);

    if (!ACM_READY(acm_ctx))
        goto Exit;

    rc = OK;

Exit:

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
    return rc;
#endif
}

static int acm_tty_tiocmget(struct tty_struct *tty, struct file *file)
{
    ashell_ctx_s* acm_ctx = tty->driver_data;
    int res = TIOCM_CTS;
    unsigned long flags;

    if (!ACM_READY(acm_ctx))
        return -EINVAL;

    flags = 0;
    spin_lock_irqsave(&acm_ctx->lock, flags);
    res |= acm_ctx->ctrlout & (int) ACM_CONTROL_SIG_DTR  ? TIOCM_DTR : 0;
    res |= acm_ctx->ctrlout & (int) ACM_CONTROL_SIG_RTS  ? TIOCM_RTS : 0;

    res |= acm_ctx->ctrlin  & (ACM_SERIAL_RING_SIG) ? TIOCM_RI  : 0;
    res |= acm_ctx->ctrlin  & (ACM_SERIAL_DSR)      ? TIOCM_DSR : 0;
    res |= acm_ctx->ctrlin  & (ACM_SERIAL_DCD)      ? TIOCM_CD  : 0;
    spin_unlock_irqrestore(&acm_ctx->lock, flags);

    printk(KERN_DEBUG "ACM: #%d, get tty IO ctrl [%x] [%x:%x]\n", tty->index,
        res, acm_ctx->ctrlout, acm_ctx->ctrlin);

    return res;
}

static int acm_tty_tiocmset(struct tty_struct *tty, struct file *file,
    unsigned int set, unsigned int clear)
{
    ashell_ctx_s* acm_ctx = tty->driver_data;
    unsigned int newctrl;
    unsigned long flags;
    int rc = OK;

    if (!ACM_READY(acm_ctx))
    {
        return ERROR;
    }

    printk(KERN_DEBUG "ACM: #%d, set tty IO ctrl, set %x, clear %x\n",
        tty->index, set, clear);

    flags = 0;
    spin_lock_irqsave(&acm_ctx->lock, flags);
    newctrl = acm_ctx->ctrlout;

    newctrl &= ~(clear & TIOCM_DTR ? (int) ACM_CONTROL_SIG_DTR : 0);
    newctrl &= ~(clear & TIOCM_RTS ? (int) ACM_CONTROL_SIG_RTS : 0);
    newctrl |= set & TIOCM_DTR ? (int) ACM_CONTROL_SIG_DTR : 0;
    newctrl |= set & TIOCM_RTS ? (int) ACM_CONTROL_SIG_RTS : 0;

    if (acm_ctx->ctrlout == newctrl)
    {
        spin_unlock_irqrestore(&acm_ctx->lock, flags);
        return 0;
    }

    acm_ctx->ctrlout = newctrl;
    spin_unlock_irqrestore(&acm_ctx->lock, flags);

    return rc;
}

static void acm_tty_wakeup_protection(struct work_struct *work)
{
    ashell_ctx_s* acm_ctx = (ashell_ctx_s *)container_of(work, ashell_ctx_s, tty_wake_work);

    if((acm_open_fd > 0) && (0 == acm_ctx->is_tty_wakeup) && acm_ctx->tty)
    {
        tty_wakeup(acm_ctx->tty);
        acm_ctx->is_tty_wakeup = 1;
    }
}

#define RELEVANT_IFLAG(iflag) (iflag & (IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK))

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23)
#define TERMIOS_S struct ktermios
#else
#define TERMIOS_S struct termios
#endif

static struct tty_operations acm_ops = {
    .open =                 acm_tty_open,
    .close =                acm_tty_close,
    .write =                acm_tty_write,
    .write_room =           acm_tty_write_room,
    .throttle =             acm_tty_throttle,
    .unthrottle =           acm_tty_unthrottle,
    .chars_in_buffer =      acm_tty_chars_in_buffer,
    .break_ctl =            acm_tty_break_ctl,
    .tiocmget =             acm_tty_tiocmget,
    .tiocmset =             acm_tty_tiocmset,
};
void ashell_udi_open(void)
{
    BSP_S32 s32Fd;
    UDI_OPEN_PARAM stOpenParam;

    stOpenParam.devid = UDI_ACM_3G_GPS_ID;

    if (acm_open_fd > 0)
    {
        printk("ashell_udi_open: 3G_GPS is already opened:0x%x\n", acm_open_fd);
        return;
    }

    s32Fd = udi_open(&stOpenParam);
    if (s32Fd <= 0)
    {
        printk("BSP_ACM_TEST_000:  udi_open ACM error:0x%x\n", s32Fd);
        return;
    }
    acm_open_fd = s32Fd;
    if (udi_ioctl(acm_open_fd, ACM_IOCTL_SET_READ_CB, acmdev_read_done) != BSP_OK)
    {
        printk("udi_ioctl ACM: line:%d\n",__LINE__);
        return;
    }
    if (udi_ioctl(acm_open_fd, ACM_IOCTL_SET_EVT_CB, acmdev_event_notify) != BSP_OK)
    {
        printk("udi_ioctl ACM: line:%d\n",__LINE__);
        return;
    }
}
EXPORT_SYMBOL(ashell_udi_open);

void ashell_udi_close(void)
{
    if (acm_open_fd > 0)
    {
        if (udi_close(acm_open_fd))
        {
            printk("udi_close failed \n");
        }
        acm_open_fd = 0;
    }
}
EXPORT_SYMBOL(ashell_udi_close);

int cdc_ashell_acm_init(void)
{
    int retval;
    //ACM_SIO_CTX_S* pAcmCtx = &sg_AcmSioCtx[DEVICE_FD_SHELL-1];
    //UDI_OPEN_PARAM stOpenParam;
    //BSP_S32 s32Fd;
    //struct sched_param s = { .sched_priority = 1 };
    ashell_ctx_s* acm_ctx = &ashell_acm_ctx;
    //ashell_acm_ctx.acm_sio_ctx = pAcmCtx;
    acm_open_fd = 0;

    printk("enter cdc_ashell_acm_init devid is %d\n", UDI_ACM_3G_GPS_ID);



    tty_driver = alloc_tty_driver(ACM_TTY_MINORS);
    if (!tty_driver)
    {
        printk("ACM: Cannot alloc tty driver\n");
        return ERROR;
    }

    tty_driver->owner = THIS_MODULE;
    tty_driver->driver_name = "uw_serial";
    tty_driver->name = "uw_tty";
    tty_driver->major = ACM_TTY_MAJOR;
    tty_driver->minor_start = 0;
    tty_driver->type = TTY_DRIVER_TYPE_SERIAL;
    tty_driver->subtype = SERIAL_TYPE_NORMAL;
    tty_driver->flags = TTY_DRIVER_REAL_RAW;
    tty_driver->init_termios = tty_std_termios;
    tty_driver->init_termios.c_cflag = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
    tty_driver->init_termios.c_ispeed = 9600;
    tty_driver->init_termios.c_ospeed = 9600;
    tty_driver->num = 1;
#if 0
    LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23)
    tty_driver->flags |= TTY_DRIVER_DYNAMIC_DEV;
#endif

    tty_set_operations(tty_driver, &acm_ops);

    retval = tty_register_driver(tty_driver);
    if (retval)
    {
        printk("ACM: Init, error registering tty %d\n", retval);

        put_tty_driver(tty_driver);
        return retval;
    }
	 
	/* 创建任务锁信号量 */
	//sema_init(&(acm_ctx->write_sem), SEM_EMPTY);
	init_waitqueue_head(&(acm_ctx->ashell_write_wait));

    acm_ctx->write_task = kthread_run(ashell_write_thread,acm_ctx,"Ashell write");
    if(NULL == acm_ctx->write_task)
    {
        printk("ACM: Init, write task failed \n");
        return ERROR;
    }
    //sched_setscheduler(pAcmCtx->write_task, SCHED_RR, &s);
    acm_ctx->write_buf=(BSP_U8*)kmalloc((ACM_TTY_SHELL_BUFFER_SIZE),GFP_KERNEL);
    if(NULL == acm_ctx->write_buf)
    {
        printk("ACM: Init, write buf is NULL \n");
        return ERROR;
    }
    acm_ctx->used = 0;
    acm_ctx->is_device = 1;
    acm_ctx->send_read_point = 0;
    acm_ctx->send_write_point = 0;
    acm_ctx->is_ashell_write = 0;
    acm_ctx->segment_size = ACM_MAX_PACKET_SIZE;
    spin_lock_init(&acm_ctx->lock);

    INIT_DELAYED_WORK(&acm_ctx->tty_wake_work, (void *)acm_tty_wakeup_protection);

#ifdef CONFIG_USB_G_SERIAL_CONSOLE
    gs_console_register(acm_ctx);
#endif

    return retval;
}

void cdc_acm_uninit(void)
{
    ashell_ctx_s* acm_ctx = &ashell_acm_ctx;

    printk("ACM: Uninit\n");
    (void)tty_unregister_driver(tty_driver);
    put_tty_driver(tty_driver);

#ifdef CONFIG_USB_G_SERIAL_CONSOLE 
    (void)unregister_console(&acm_ctx->port_console); 
#endif 

    if (acm_open_fd > 0)
    {
        if (udi_close(acm_open_fd))
        {
            printk("udi_close failed \n");
        }
    }

    if(NULL != acm_ctx->write_task)
    {
        (void) kthread_stop(acm_ctx->write_task);
    }

    if(acm_ctx->write_buf)
    {
        kfree(acm_ctx->write_buf);
    }
}

void acm_udi_enable_set(void)
{
    ashell_acm_ctx.enable = TRUE;
}
EXPORT_SYMBOL(acm_udi_enable_set);

void acm_udi_disable_set(void)
{
	ashell_acm_ctx.enable = FALSE;
}
EXPORT_SYMBOL(acm_udi_disable_set);

#ifdef CONFIG_USB_G_SERIAL_CONSOLE
extern struct console* get_uart_console(void);
void gs_console_register(ashell_ctx_s* acm_ctx)
{
    struct console* uart_console;
    int ret;
    
    strcpy(acm_ctx->port_console.name,"uw_tty");
    acm_ctx->port_console.write  = gs_console_write; 
    acm_ctx->port_console.device = gs_console_device; 
    acm_ctx->port_console.setup  = gs_console_setup; 
    acm_ctx->port_console.flags  = CON_PRINTBUFFER; 
    acm_ctx->port_console.index  = 0;/*-1*/ 
    acm_ctx->port_console.data   = acm_ctx; 
    
    uart_console = get_uart_console();
    ret = unregister_console(uart_console);
    
    register_console(&acm_ctx->port_console); 

	printk("unregister_console uart ret is %d \n",ret);
}

/* This stolen/adapted from serial/sn_console.c */ 
static void gs_console_write(struct console *cons, const char *buf, unsigned count) 
{ 

    unsigned long flags = 0;
    ashell_ctx_s* acm_ctx;
    int fst_count;
    int dst_end;
    uint32_t sec_count;
    int cpy_limit;
    int cpy_count;

    acm_ctx = cons->data;

    if(acm_open_fd <= 0)
    {
       return; 
    }
    if (!(acm_ctx->enable))
    {
        /*os_printf("gs_console_write usb is not enable \n");*/
        return;
    }
    if(NULL == buf)
    {
        printk(KERN_DEBUG "gs_console_write buf is NULL\n");
        return;
    }
    /* somebody really wants this output, might be an 
     * oops, kdb, panic, etc.  make sure they get it. */ 
    flags = 0;
    if (spin_is_locked(&acm_ctx->lock)) { 

        int got_lock = 0; 
        int counter; 
 
        /* 
         * We attempt to determine if someone has died with the 
         * lock. We wait ~20 secs after the head and tail ptrs 
         * stop moving and assume the lock holder is not functional 
         * and plow ahead. If the lock is freed within the time out 
         * period we re-get the lock and go ahead normally. We also 
         * remember if we have plowed ahead so that we don't have 
         * to wait out the time out period again - the asumption 
         * is that we will time out again. 
         */ 
        for (counter = 0; counter < 150; mdelay(125), counter++) { 
            if (!spin_is_locked(&acm_ctx->lock) || acm_ctx->stole_lock) { 
                if (!acm_ctx->stole_lock) 
                {
                    break; 
                }
                spin_lock_irqsave(&acm_ctx->lock, flags); 
                got_lock = 1; 
                break; 
            } 
            #if 0
            /* still locked */ 
            if ((get != port->port_write_buf.buf_get) 
                || (put != port->port_write_buf.buf_put)) { 
                put = port->port_write_buf.buf_put; 
                get = port->port_write_buf.buf_get; 
                counter = 0; 
            } 
            #endif
        } 
        /* Make space by flushing any waiting output */ 
        if (acm_ctx->enable)
        {
            cpy_limit = ACM_TTY_SHELL_BUFFER_SIZE-acm_ctx->send_write_point;
            cpy_count = (count <= (unsigned)cpy_limit)?count : (cpy_limit);
            fst_count = usb_mem_char_cpy(acm_ctx->write_buf+acm_ctx->send_write_point,(const unsigned char *)buf,cpy_count,cpy_limit,&dst_end);
            if(count != fst_count)
            {
                sec_count = (count - fst_count);
                sec_count = (sec_count <= acm_ctx->send_write_point)? sec_count : acm_ctx->send_write_point;
                usb_mem_char_cpy(acm_ctx->write_buf,(const unsigned char *)(buf + fst_count) , sec_count,acm_ctx->send_write_point,&dst_end);
            }
            acm_ctx->send_write_point = dst_end-(int)acm_ctx->write_buf;
            //binary_up(&(acm_ctx->write_sem));
            acm_ctx->is_ashell_write = TRUE;
            wake_up(&(acm_ctx->ashell_write_wait));
    	}
			
        if (got_lock) { 
            spin_unlock_irqrestore(&acm_ctx->lock, flags); 
            acm_ctx->stole_lock = 0; 
        } else { 
            /* fell thru */ 
            acm_ctx->stole_lock = 1; 
        } 
        
        return; 
    } 
    
    acm_ctx->stole_lock = 0; 
    spin_lock_irqsave(&acm_ctx->lock, flags); 

    /* Flush out any waiting output so we have as much space as possible */ 
    if (acm_ctx->enable)
    {
        cpy_limit = ACM_TTY_SHELL_BUFFER_SIZE-acm_ctx->send_write_point;
        cpy_count = (count <= (unsigned)cpy_limit)?count : (cpy_limit);
        fst_count = usb_mem_char_cpy(acm_ctx->write_buf+acm_ctx->send_write_point,(const unsigned char *)buf,cpy_count,cpy_limit,&dst_end);
        if(count != fst_count)
        {
            sec_count = (count - fst_count);
            sec_count = (sec_count <= acm_ctx->send_write_point)? sec_count : acm_ctx->send_write_point;
            (void)usb_mem_char_cpy(acm_ctx->write_buf,(const unsigned char *)(buf + fst_count) , sec_count,acm_ctx->send_write_point,&dst_end);
        }
        acm_ctx->send_write_point = dst_end-(int)acm_ctx->write_buf;
        //binary_up(&(acm_ctx->write_sem));
        acm_ctx->is_ashell_write = TRUE;
        wake_up(&(acm_ctx->ashell_write_wait));
    }
    spin_unlock_irqrestore(&acm_ctx->lock, flags); 
}

struct tty_driver *gs_console_device(struct console *cons, int *gidx) 
{
    if (gidx)
    {
        *gidx = 0; 
    }
    return tty_driver; 
} 
 
static int gs_console_setup (struct console *cons, char *options) 
{ 
    ashell_ctx_s* acm_ctx;
    /* 
     * If this is called we are about to become the active console. (?) 
     * 
     * That is not what this hook is for, but it serves our purposes. 
     * 
     * Since there is no actual tty yet, we have to do some of what is 
     * normally done in gs_open. 
     */ 
    acm_ctx = container_of(cons, ashell_ctx_s, port_console);
    acm_ctx = acm_ctx;
    return OK;
} 
#endif 
 


