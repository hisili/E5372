/* (c) 2011 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _ACM_UDI_TTY_H_
#define _ACM_UDI_TTY_H_

#include <linux/wait.h>
#include <linux/tty.h>
#include <linux/console.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/version.h>

//#include "BSP_USB_ACM.h"
//#include "BSP_USB_ACM_Sio.h"


#define ACM_TTY_SHELL
#define CONFIG_USB_G_SERIAL_CONSOLE

#define ACM_BUF_SIZE        512
#define ACM_IO_DELAY        1

#define ACM_TTY_MAJOR       210
#define ACM_TTY_MINORS      1

#define ACM_DFLT_BITRATE    115200
#define ACM_BUF_SIZE        512
#define ACM_IO_DELAY        1

/* delay of 15 seconds */
#define CLOSE_TIMEOUT               (15 * 1000)
#define ACM_TTY_MAX_BUFFER_SIZE	    (64 * 1024)
#define ACM_TTY_LIST_MAX_COUNT	    16
//#define ACM_TTY_LIST_BUFFER_SIZE    0xfe00  //(64 * 1024-512)
#define ACM_TTY_LIST_BUFFER_SIZE    0x400

#define ACM_TTY_IOCTL_SET_RD_DONE_CB        0xAC000001
#define ACM_TTY_IOCTL_SET_RD_LIST_BUFFER    0xAC000002

#define ACM_TTY_SHELL_BUFFER_SIZE 	(32*1024)
#define ACM_MAX_PACKET_SIZE			512
#define ACM_CONTROL_SIG_DTR 		0x1
#define ACM_CONTROL_SIG_RTS 		0x2
#define ACM_SERIAL_DCD            0x0001
#define ACM_SERIAL_DSR            0x0002
#define ACM_SERIAL_BREAK          0x0004
#define ACM_SERIAL_RING_SIG       0x0008
#define ACM_SERIAL_FRAMING_ERROR  0x0010
#define ACM_SERIAL_PARITY_ERROR   0x0020
#define ACM_SERIAL_OVERRUN_ERROR  0x0040

#ifndef OK
#define OK 0
#endif

#ifndef ERROR
#define ERROR -1
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define ASSERT(X)						\
do {								\
	if (!(X)) {					\
		printk(KERN_ERR "\n");				\
		printk(KERN_ERR "AFS: Assertion failed\n");	\
		BUG();						\
	}							\
} while(0)

typedef struct
{
	struct tty_struct   *tty;
    uint32_t is_ashell_write;
    uint32_t is_acm_ready;
    uint32_t is_tty_wakeup;
    uint32_t send_write_point;
	uint32_t send_read_point;
    uint32_t enable;
    BSP_U8* write_buf;
	int             stole_lock;     /* for console oopses */
    struct console   port_console; 
	uint16_t  ctrlout;
	uint16_t  ctrlin;
	uint16_t  used;
	void      *dev;
	int    close_in_progress;
	uint16_t   read_pending;
	int     is_device;
	spinlock_t  lock;
	uint16_t   segment_size;
	int  throttle;
	int  throttled_in_read;
    wait_queue_head_t ashell_write_wait;
    
	struct task_struct *write_task;
    struct delayed_work tty_wake_work;
}ashell_ctx_s;
#endif

