#if defined(CONFIG_SERIAL_BALONG_V7R1_CONSOLE) && defined(CONFIG_MAGIC_SYSRQ)
#define SUPPORT_SYSRQ
#endif

#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/amba/bus.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>

#include <asm/io.h>
#include <asm/sizes.h>

#include "balong-v7r1uart.h"
/*lint -e40 */
#define UART_NR			4

#define SERIAL_BALONG_MAJOR	204
#define SERIAL_BALONG_MINOR	64
#define SERIAL_BALONG_NR    UART_NR

#define AMBA_ISR_PASS_LIMIT	256

#define UART_DR_ERROR		(UARTV7R1_DR_OE|UARTV7R1_DR_BE|UARTV7R1_DR_PE|UARTV7R1_DR_FE)
#define UART_DUMMY_DR_RX	(1 << 16)

/* There is by now at least one vendor with differing details, so handle it */
struct vendor_data {
	unsigned int		ifls;
	unsigned int		fifosize;
	unsigned int		lcrh_tx;
	unsigned int		lcrh_rx;
	bool			oversampling;
	bool			dma_threshold;
};/*lint !e959 !e958*/

#define UARTV7R1_LCRH		0x2c	/* Line control register. */
#define UARTV7R1_IFLS_RX1_8	(0 << 3)
#define UARTV7R1_IFLS_RX2_8	(1 << 3)
#define UARTV7R1_IFLS_RX4_8	(2 << 3)
#define UARTV7R1_IFLS_RX6_8	(3 << 3)
#define UARTV7R1_IFLS_RX7_8	(4 << 3)
#define UARTV7R1_IFLS_TX1_8	(0 << 0)
#define UARTV7R1_IFLS_TX2_8	(1 << 0)
#define UARTV7R1_IFLS_TX4_8	(2 << 0)
#define UARTV7R1_IFLS_TX6_8	(3 << 0)
#define UARTV7R1_IFLS_TX7_8	(4 << 0)

static struct vendor_data vendor_hisilicon = {
	.ifls			= UARTV7R1_IFLS_RX4_8|UARTV7R1_IFLS_TX4_8,
	.fifosize		= 16,
	.lcrh_tx		= UARTV7R1_LCRH,
	.lcrh_rx		= UARTV7R1_LCRH,
	.oversampling		= false,
	.dma_threshold		= false,
};

/*
 * We wrap our port structure around the generic uart_port.
 */
struct uart_balong_port {
	struct uart_port	port;
	struct clk		*clk;
	const struct vendor_data *vendor;
	unsigned int		dmacr;		/* dma control reg */
	unsigned int		im;		/* interrupt mask */
	unsigned int		old_status;
	unsigned int		fifosize;	/* vendor-specific */
	unsigned int		lcrh_tx;	/* vendor-specific */
	unsigned int		lcrh_rx;	/* vendor-specific */
	bool			autorts;
	char			type[12];
};/*lint !e959 !e958*/


static void uartv7r1_stop_tx(struct uart_port *port)
{
    struct uart_balong_port *uap = (struct uart_balong_port *)port;

    uap->im &= ~(PTIME|ETBEI);
    writel(uap->im, uap->port.membase + DW_UART_IER);
}

static void uartv7r1_start_tx(struct uart_port *port)
{
    struct uart_balong_port *uap = (struct uart_balong_port *)port;

    uap->im |= ETBEI;
    writel(uap->im, uap->port.membase + DW_UART_IER);
}

static void uartv7r1_stop_rx(struct uart_port *port)
{
    struct uart_balong_port *uap = (struct uart_balong_port *)port;

    uap->im &= ~(ELSI|ERBFI);         
    writel(uap->im, uap->port.membase + DW_UART_IER);
}

static void uartv7r1_enable_ms(struct uart_port *port)
{
}

static inline void dw_wait_idle(struct uart_port *port)
{
	while(readl(port->membase + DW_UART_USR) & DW_UART_BUSY)
	{
		if(readl(port->membase + DW_UART_LSR) & 0x01)
		{
			readl(port->membase + DW_UART_RBR);
		}
	}
}

static void uartv7r1_rx_chars(struct uart_balong_port *uap)
{
    struct tty_struct *tty = uap->port.state->port.tty;
    unsigned int status, ch, flag, rsr, max_count = 256;

    status = readl(uap->port.membase + DW_UART_LSR);

    while ((status & DW_DR) && max_count--)
    {
        ch = readl(uap->port.membase + DW_UART_RBR);
        flag = TTY_NORMAL;
        uap->port.icount.rx++;

        /*
         * Note that the error handling code is
         * out of the main execution path
         */

        rsr = status;

        if (unlikely(rsr & DW_RSR_ANY)) {    /*lint !e730*/
            if (rsr & DW_BI) {
                rsr &= ~(DW_FE|DW_PE);
                uap->port.icount.brk++;
                if (uart_handle_break(&uap->port))
                {
                	status = readl(uap->port.membase + DW_UART_LSR);
                	continue;
                }
            }
            else if (rsr & DW_PE)
            {
                uap->port.icount.parity++;
            }
            else if (rsr & DW_FE)
            {
                uap->port.icount.frame++;
            }
            else if (rsr & DW_OE)
            {
                uap->port.icount.overrun++;
            }

            rsr &= uap->port.read_status_mask;

            if (rsr & DW_BI)
                flag = TTY_BREAK;
            else if (rsr & DW_PE)
                flag = TTY_PARITY;
            else if (rsr & DW_FE)
                flag = TTY_FRAME;
        }

        if (uart_handle_sysrq_char(&uap->port, ch))
        {
            	status = readl(uap->port.membase + DW_UART_LSR);
            	continue;
        }

        uart_insert_char(&uap->port, rsr, DW_OE, ch, flag);
        
/*BEGIN:y00206456 2012-04-27 Deleted for pclint e563*/
/*BEGIN:y00206456 2012-04-27 Deleted for pclint e563*/
        status = readl(uap->port.membase + DW_UART_LSR);
    }
    tty_flip_buffer_push(tty);
    return;
}/*lint !e550*/

static void uartv7r1_tx_chars(struct uart_balong_port *uap)
{

	struct circ_buf *xmit = &uap->port.state->xmit;
    int count;

    if (uap->port.x_char) {	
	    /*lint -e527*/
        while ((readl(uap->port.membase + DW_UART_LSR) & 0x20)!=0x20)
        {
        }
		writel(uap->port.x_char, uap->port.membase + DW_UART_THR);
        uap->port.icount.tx++;
        uap->port.x_char = 0;
        return;
        /*lint +e527*/
    }

    if (uart_circ_empty(xmit) || uart_tx_stopped(&uap->port)) {
        uartv7r1_stop_tx(&uap->port);
        return;
    }

    count = uap->port.fifosize >> 1;

    do {
	   /*lint -e527*/
        while ((readl(uap->port.membase + DW_UART_LSR) & 0x20)!=0x20)
        {
        }
        writel(xmit->buf[xmit->tail], uap->port.membase + DW_UART_THR);
        xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);/*lint !e737 !e123*/
        uap->port.icount.tx++;
        if (uart_circ_empty(xmit))
            break;
        /*lint +e527*/
    } while (--count > 0);

    if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)/*lint !e737 !e123*/
        uart_write_wakeup(&uap->port);

    if (uart_circ_empty(xmit))
        uartv7r1_stop_tx(&uap->port);
}

static void uartv7r1_modem_status(struct uart_balong_port *uap)
{
 /*lint -e553*//*solve the pc lint 553:Undefined preprocessor variable*/
#if DW_UART_SUPPORT_MODEM
	unsigned int status, delta;

	status = readl(uap->port.membase + UARTV7R1_X_FR) & UARTV7R1_X_FR_MODEM_ANY;

	delta = status ^ uap->old_status;
	uap->old_status = status;

	if (!delta)
		return;

	if (delta & UARTV7R1_X_FR_DCD)
		uart_handle_dcd_change(&uap->port, status & UARTV7R1_X_FR_DCD);

	if (delta & UARTV7R1_X_FR_DSR)
		uap->port.icount.dsr++;

	if (delta & UARTV7R1_X_FR_CTS)
		uart_handle_cts_change(&uap->port, status & UARTV7R1_X_FR_CTS);

	wake_up_interruptible(&uap->port.state->port.delta_msr_wait);
#endif
/*lint +e553*/
}

static irqreturn_t uartv7r1_int(int irq, void *dev_id)
{
    #define DW_ISR_PASS_LIMIT    256

    struct uart_balong_port *uap = dev_id;
    unsigned int status, pass_counter = DW_ISR_PASS_LIMIT;
    unsigned int interruptID;
    int handled = 0;

    spin_lock(&uap->port.lock);

    status = readl(uap->port.membase + DW_UART_IIR);
    interruptID = 0x0F&status;                   

    while (interruptID != DW_NOINTERRUPT)
    {
        if ((DW_RECEIVEAVA == interruptID) || 
            (DW_RECTIMEOUT == interruptID) ||
            (DW_RECEIVERR == interruptID))
        {
            uartv7r1_rx_chars(uap);
        }
        else if (DW_MODEMSTA == interruptID)
        {
            uartv7r1_modem_status(uap);
        }
        else if (DW_TRANSEMP == interruptID)
        {
            uartv7r1_tx_chars(uap);
        }
		else /* transe busy intr*/  /* CR: AI7D01557 AI7D01593 AI7D01268 add 2007/10/09 */
		{
			status = readl(uap->port.membase + DW_UART_LSR);
			handled = 1;
			break;
		}

        if (pass_counter-- == 0)
            break;

        status = readl(uap->port.membase + DW_UART_IIR);
        interruptID = 0x0F&status;                   
        handled = 1;
    } 

    spin_unlock(&uap->port.lock);

    return IRQ_RETVAL(handled);/*lint !e64*/
}

static unsigned int uartv7r1_x_tx_empty(struct uart_port *port)
{
    struct uart_balong_port *uap = (struct uart_balong_port *)port;
    unsigned int status = readl(uap->port.membase + DW_UART_LSR);

    if(status & DW_TEMT)
    {
        return TIOCSER_TEMT;
    }

    return 0;            
}/*lint !e529*/

static unsigned int uartv7r1_x_get_mctrl(struct uart_port *port)
{
    unsigned int result = 0;
/*lint -e553*//*solve the pc lint 553:Undefined preprocessor variable*/
#if DW_UART_SUPPORT_MODEM
    struct uart_balong_port *uap = (struct uart_balong_port *)port;
    unsigned int result = 0;
    unsigned int status = readl(uap->port.membase + DW_UART_MSR);

#define BIT(uartbit, tiocmbit)        \
    if (status & uartbit)        \
        result |= tiocmbit

            BIT(DW_DCD, TIOCM_CAR);
    BIT(DW_DSR, TIOCM_DSR);
    BIT(DW_CTS, TIOCM_CTS);
    BIT(DW_RI, TIOCM_RNG);
#undef BIT

#endif
/*lint +e553*/

    return result;
}

static void uartv7r1_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
    return ;
/*lint -e553*//*solve the pc lint 553:Undefined preprocessor variable*/    
#if DW_UART_SUPPORT_MODEM
    struct uart_balong_port *uap = (struct uart_balong_port *)port;
    unsigned int cr;

    cr = readl(uap->port.membase + DW_UART_MCR);

#define    BIT(tiocmbit, uartbit)        \
    if (mctrl & tiocmbit)        \
        cr |= uartbit;        \
    else                \
        cr &= ~uartbit


			BIT(TIOCM_RTS, DW_MC_RTS);
    BIT(TIOCM_DTR, DW_MC_DTR);
    BIT(TIOCM_OUT1, DW_MC_OUT1);
    BIT(TIOCM_OUT2, DW_MC_OUT2);
    BIT(TIOCM_LOOP, DW_MC_LOOP);
#undef BIT

    writel(cr, uap->port.membase + DW_UART_MCR);
#endif
/*lint +e553*/
}

static void uartv7r1_break_ctl(struct uart_port *port, int break_state)
{
    struct uart_balong_port *uap = (struct uart_balong_port *)port;
    unsigned long flags = 0;
    unsigned int lcr_h;

    spin_lock_irqsave(&uap->port.lock, flags);/*lint !e26 !e515*/

	/* 
	 *	CR: AI7D01557 AI7D01593 AI7D01268 
	 *	insert dw_wait_idle() 2007/10/09
	 */
	dw_wait_idle(port);
    
    lcr_h = readl(uap->port.membase + DW_UART_LCR);
    if (break_state)
        lcr_h |= DW_UART_BREAK;
    else
        lcr_h &= ~DW_UART_BREAK;
    writel(lcr_h, uap->port.membase + DW_UART_LCR);

    spin_unlock_irqrestore(&uap->port.lock, flags);
}/*lint !e550*/

static int uartv7r1_startup(struct uart_port *port)
{

    struct uart_balong_port *uap = (struct uart_balong_port *)port;
    int retval;

    /*
     * Try to enable the clock producer.
     */
    retval = clk_enable(uap->clk);
    if (retval)
    {
        return retval;
    }
    uap->port.uartclk = clk_get_rate(uap->clk);

	/* disable interrupt */
    writel(0, uap->port.membase + DW_UART_IER);
	
	//writel(0x0, uap->port.membase + DW_UART_FCR);	
	/* enable FIFO */
	writel(0x1, uap->port.membase + DW_UART_FCR);
	/*set fifo trigger level*/
    writel(0x51,uap->port.membase + DW_UART_FCR);
	/*clear and reset fifo*/
    writel(0x57,uap->port.membase + DW_UART_FCR);
	
	/* clear line interrupt status */
    readl(uap->port.membase + DW_UART_LSR);
	
	/*clear rcv interrupt*/
    readl(uap->port.membase + DW_UART_RBR);
	
	/*clear iir reg*/
    readl(uap->port.membase + DW_UART_IIR);	
	
    /*clear line busy interrupt*/
    readl(uap->port.membase + DW_UART_USR);	
	/* 
	 *	CR: AI7D01557 AI7D01593 AI7D01268
	 *	del do{...}while; 2007/10/09
	 */
/*lint -e553*//*solve the pc lint 553:Undefined preprocessor variable*/
#if DW_UART_SUPPORT_MODEM
    /*
     * initialise the old status of the modem signals
     */
    uap->old_status = readl(uap->port.membase + DW_UART_MSR) & DW_MSR_ANY;

#endif    
/*lint +e553*/
	/*
	 * Allocate the IRQ
	 */
	retval = request_irq(uap->port.irq, uartv7r1_int, 0, "BalongV7R1 Uart", uap);
	if (retval)
	{
            clk_disable(uap->clk);
            return retval;   
    }
	
	#if 0
	/* enable FIFO */
	writel(0x1, uap->port.membase + DW_UART_FCR);
	/*set fifo trigger level*/
    writel(0x51,uap->port.membase + DW_UART_FCR);

	/* clear interrupt status */
    readl(uap->port.membase + DW_UART_LSR);
	#endif
    
	/*
     * Finally, enable interrupts
     */
    spin_lock_irq(&uap->port.lock);

    uap->im = (ERBFI|ELSI);
    writel(uap->im, uap->port.membase + DW_UART_IER);

    spin_unlock_irq(&uap->port.lock);

	return 0;

#if 0
clk_dis:
    clk_disable(uap->clk);
out:
    return retval;   
#endif 
}

static void uartv7r1_shutdown(struct uart_port *port)
{
    struct uart_balong_port *uap = (struct uart_balong_port *)port;
    unsigned long val;

    spin_lock_irq(&uap->port.lock);

    /*
     * disable/clear all interrupts
     */
    uap->im = 0;
    writel(uap->im, uap->port.membase + DW_UART_IER);
    readl( uap->port.membase + DW_UART_LSR);

    spin_unlock_irq(&uap->port.lock);

    /*
     * Free the interrupt
     */
    free_irq(uap->port.irq, uap);

    /*
     * disable break condition and fifos
     */

	/*
	 *	CR: AI7D01557 AI7D01593 AI7D01268
	 *	insert dw_wait_idle() 2007/10/09
	 */
	dw_wait_idle(port);
    val = readl(uap->port.membase + DW_UART_LCR);
    val &= ~(DW_UART_BREAK);
    writel(val, uap->port.membase + DW_UART_LCR);

    /* disable fifo*/
    writel(0,uap->port.membase + DW_UART_FCR);

    /*
     * Shut down the clock producer
     */
    clk_disable(uap->clk);

}/*lint !e550*/

static void
uartv7r1_set_termios(struct uart_port *port, struct ktermios *termios,
		     struct ktermios *old)
{
    unsigned int lcr_h, old_cr;
    unsigned long flags = 0;
    unsigned int baud, quot;

    /*
     * Ask the core to calculate the divisor for us.
     */
    baud = uart_get_baud_rate(port, termios, old, 0, port->uartclk/16);
    quot = port->uartclk / (16 * baud);

    switch (termios->c_cflag & CSIZE) {
        case CS5:
            lcr_h = DW_UART_5bit;
            break;
        case CS6:
            lcr_h = DW_UART_6bit;
            break;
        case CS7:
            lcr_h = DW_UART_7bit;
            break;
        case CS8:
            lcr_h = DW_UART_8bit;
            break;
        default: // CS8
            lcr_h = DW_UART_8bit;
            break;
    }

    if (termios->c_cflag & CSTOPB)
        lcr_h |= DW_UART_STOP;

    if (termios->c_cflag & PARENB) {
        lcr_h |= DW_UART_PEN;

        if (!(termios->c_cflag & PARODD))
            lcr_h |= DW_UART_EPS;
    }

    spin_lock_irqsave(&port->lock, flags);/*lint !e26 !e515*/

    /*
     * Update the per-port timeout.
     */
    uart_update_timeout(port, termios->c_cflag, baud);

    port->read_status_mask = DW_OE;

    if (termios->c_iflag & INPCK)
        port->read_status_mask |= DW_FE | DW_PE;
    if (termios->c_iflag & (BRKINT | PARMRK))
        port->read_status_mask |= DW_BI;

    /*
     * Characters to ignore
     */
    port->ignore_status_mask = 0;
    if (termios->c_iflag & IGNPAR)
        port->ignore_status_mask |= DW_FE | DW_PE;
    if (termios->c_iflag & IGNBRK) {
        port->ignore_status_mask |= DW_BI;
        /*
         * If we're ignoring parity and break indicators,
         * ignore overruns too (for real raw support).
         */
        if (termios->c_iflag & IGNPAR)
            port->ignore_status_mask |= DW_OE;
    }

    /*
     * Ignore all characters if CREAD is not set.
     */
    #define UART_DUMMY_RSR_RX	256
    if ((termios->c_cflag & CREAD) == 0)
        port->ignore_status_mask |= (UART_DUMMY_RSR_RX);
    //port->ignore_status_mask |= UART_DUMMY_RSR_RX; !!todo


    if (UART_ENABLE_MS(port, termios->c_cflag))
        uartv7r1_enable_ms(port);

	/*
	 *	CR: AI7D01557 AI7D01593 AI7D01268
	 *	insert dw_wait_idle() 2007/10/09
	 */

	dw_wait_idle(port);

	/* Enable DLL and DLH */
    old_cr = readl(port->membase + DW_UART_LCR);    
    old_cr |= DW_UART_DLAB;     
    writel(old_cr,port->membase + DW_UART_LCR);

    /* Set baud rate */
    writel(((quot&0xFF00)>>8), port->membase + DW_UART_DLH);
    writel((quot & 0xFF), port->membase + DW_UART_DLL);

    old_cr &= ~DW_UART_DLAB;
    writel(old_cr,port->membase + DW_UART_LCR);

    writel(lcr_h, port->membase + DW_UART_LCR);

    spin_unlock_irqrestore(&port->lock, flags);
}/*lint !e550*/

static const char *uartv7r1_type(struct uart_port *port)
{
	struct uart_balong_port *uap = (struct uart_balong_port *)port;
	return uap->port.type == PORT_AMBA ? uap->type : NULL;
}

/*
 * Release the memory region(s) being used by 'port'
 */
static void uartv7r1port_release_port(struct uart_port *port)
{
	release_mem_region(port->mapbase, SZ_4K);
}

/*
 * Request the memory region(s) being used by 'port'
 */
static int uartv7r1port_request_port(struct uart_port *port)
{
	return request_mem_region(port->mapbase, SZ_4K, "uart-balong")
			!= NULL ? 0 : -EBUSY;
}

/*
 * Configure/autoconfigure the port.
 */
static void uartv7r1port_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE) {
		port->type = PORT_AMBA;
		uartv7r1port_request_port(port);
	}
}

/*
 * verify the new serial_struct (for TIOCSSERIAL).
 */
static int uartv7r1port_verify_port(struct uart_port *port, struct serial_struct *ser)
{
    int ret = 0;
    if (ser->type != PORT_UNKNOWN && ser->type != PORT_AMBA)
        ret = -EINVAL;
    if (ser->irq < 0 || ser->irq >= NR_IRQS)
        ret = -EINVAL;
    if (ser->baud_base < 9600)
        ret = -EINVAL;
    return ret;
}

static struct uart_ops amba_uartv7r1_pops = {
	.tx_empty	= uartv7r1_x_tx_empty,
	.set_mctrl	= uartv7r1_set_mctrl,
	.get_mctrl	= uartv7r1_x_get_mctrl,
	.stop_tx	= uartv7r1_stop_tx,
	.start_tx	= uartv7r1_start_tx,
	.stop_rx	= uartv7r1_stop_rx,
	.enable_ms	= uartv7r1_enable_ms,
	.break_ctl	= uartv7r1_break_ctl,
	.startup	= uartv7r1_startup,
	.shutdown	= uartv7r1_shutdown,
	.set_termios	= uartv7r1_set_termios,
	.type		= uartv7r1_type,
	.release_port	= uartv7r1port_release_port,
	.request_port	= uartv7r1port_request_port,
	.config_port	= uartv7r1port_config_port,
	.verify_port	= uartv7r1port_verify_port,
};

static struct uart_balong_port *amba_ports[UART_NR];

struct uart_port* get_amba_ports(void)
{
        return (struct uart_port*)(&amba_ports[0]);
}


#ifdef CONFIG_SERIAL_BALONG_V7R1_CONSOLE

static void uartv7r1_console_putchar(struct uart_port *port, int ch)
{
    unsigned int status;

    do {
        status = readl(port->membase + DW_UART_LSR);
    } while ((status & 0x20) != 0x20);
    
    writel(ch, port->membase + DW_UART_THR);
}

static void uartv7r1_console_write(struct console *co, const char *s, unsigned int count)
{
	struct uart_balong_port *uap = amba_ports[co->index];
	

	clk_enable(uap->clk);

	uart_console_write(&uap->port, s, count, uartv7r1_console_putchar);

	clk_disable(uap->clk);
}

static void __init
uartv7r1_console_get_options(struct uart_balong_port *uap, int *baud,
			     int *parity, int *bits)
{
    unsigned int lcr_h, brd,usr_h;
    unsigned int flags = 0;
    unsigned char lbrd,hbrd;
	unsigned int usr_timeout = 1000;
	
    lcr_h = readl(uap->port.membase + DW_UART_LCR);

    *parity = 'n';
    if (lcr_h & DW_UART_PEN) {
        if (lcr_h & DW_UART_EPS)
            *parity = 'e';
        else
            *parity = 'o';
    }

    if ((lcr_h & DW_UART_DATALEN_MASK) == DW_UART_5bit)
        *bits = 5;
    else if((lcr_h & DW_UART_DATALEN_MASK) == DW_UART_6bit)
        *bits = 6;
    else if((lcr_h & DW_UART_DATALEN_MASK) == DW_UART_7bit)        
        *bits = 7;
    else
        *bits = 8;
	do{
		usr_timeout--;		
		usr_h = readl(uap->port.membase + DW_UART_USR);
	}while((usr_h & DW_UART_BUSY)&&(0 != usr_timeout));
	
	if(0 == usr_timeout)
	{
		return -EBUSY;
	}
	else
	{
		lcr_h |= DW_UART_DLAB;
		writel(lcr_h, uap->port.membase + DW_UART_LCR);
		
		spin_lock_irqsave(&uap->port.lock, flags);/*lint !e26 !e515*/
		lbrd = readl(uap->port.membase + DW_UART_DLL);
		hbrd = readl(uap->port.membase + DW_UART_DLH);		
		spin_unlock_irqrestore(&uap->port.lock, flags);
		
		lcr_h &= ~DW_UART_DLAB;
		writel(lcr_h, uap->port.membase + DW_UART_LCR);
	}
    brd = (hbrd<<8) + lbrd;

    *baud = uap->port.uartclk / (16*brd);   /*lint !e414 */
}

static int __init uartv7r1_console_setup(struct console *co, char *options)
{
	struct uart_balong_port *uap;
	int baud = 38400;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	/*
	 * Check whether an invalid uart number has been specified, and
	 * if so, search for the first available port that does have
	 * console support.
	 */
	if (co->index >= UART_NR)
		co->index = 0;
	uap = amba_ports[co->index];
	if (!uap)
		return -ENODEV;

	uap->port.uartclk = clk_get_rate(uap->clk);

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);
	else
		uartv7r1_console_get_options(uap, &baud, &parity, &bits);

	return uart_set_options(&uap->port, co, baud, parity, bits, flow);
}

static struct uart_driver amba_reg;
static struct console amba_console = {
	.name		= "ttyAMA",
	.write		= uartv7r1_console_write,
	.device		= uart_console_device,
	.setup		= uartv7r1_console_setup,
	.flags		= CON_PRINTBUFFER,
	.index		= -1,
	.data		= &amba_reg,
};

#define AMBA_CONSOLE	(&amba_console)
#else
#define AMBA_CONSOLE	NULL
#endif

static struct uart_driver amba_reg = {
	.owner			= THIS_MODULE,
	.driver_name	= "ttyAMA",
	.dev_name		= "ttyAMA",
	.major			= SERIAL_BALONG_MAJOR,
	.minor			= SERIAL_BALONG_MINOR,
	.nr			    = UART_NR,
	.cons			= AMBA_CONSOLE, /*lint !e34 */
};

struct console* get_uart_console(void)
{
	struct console* uart_console = amba_reg.cons;
	return uart_console;
}
static int uartv7r1_probe(struct amba_device *dev, struct amba_id *id)
{
	struct uart_balong_port *uap;
	struct vendor_data *vendor = id->data;
	void __iomem *base;
	int i, ret;

    /*解决参数类型不对的问题-- linux内核*/
    /*lint -e516*/
	/*lint -e30*/
	for (i = 0; (unsigned int)i < ARRAY_SIZE(amba_ports); i++)/*lint !e806 !e84*/
		if (amba_ports[i] == NULL)
			break;

	if ((unsigned int)i == ARRAY_SIZE(amba_ports)) {/*lint !e806 !e84*/
		ret = -EBUSY;
		goto out;
	}
	/*lint +e30*/
    /*lint +e516*/
	uap = kzalloc(sizeof(struct uart_balong_port), GFP_KERNEL);
	if (uap == NULL) {
		ret = -ENOMEM;
		goto out;
	}

	base = ioremap(dev->res.start, resource_size(&dev->res));
	if (!base) {
		ret = -ENOMEM;
		goto free;
	}

	uap->clk = clk_get(&dev->dev, NULL);
	if (IS_ERR(uap->clk)) {
		ret = PTR_ERR(uap->clk);
		goto unmap;
	}

	uap->vendor = vendor;
	uap->lcrh_rx = vendor->lcrh_rx;
	uap->lcrh_tx = vendor->lcrh_tx;
	uap->fifosize = vendor->fifosize;
	uap->port.dev = &dev->dev;
	uap->port.mapbase = dev->res.start;
	uap->port.membase = base;
	uap->port.iotype = UPIO_MEM;
	uap->port.irq = dev->irq[0];
	uap->port.fifosize = uap->fifosize;
	uap->port.ops = &amba_uartv7r1_pops;
	uap->port.flags = UPF_BOOT_AUTOCONF;
	/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
	uap->port.line = (unsigned int)i;
	/*end*/
	snprintf(uap->type, sizeof(uap->type), "Balong rev%u", amba_rev(dev));

	amba_ports[i] = uap;

	amba_set_drvdata(dev, uap);
	ret = uart_add_one_port(&amba_reg, &uap->port);
	if (ret) {
		amba_set_drvdata(dev, NULL);
		amba_ports[i] = NULL;
		clk_put(uap->clk);
 unmap:
		iounmap(base);
 free:
		kfree(uap);
	}
 out:
	return ret;
}

static int uartv7r1_remove(struct amba_device *dev)
{
	struct uart_balong_port *uap = amba_get_drvdata(dev);
	int i;

	amba_set_drvdata(dev, NULL);

	uart_remove_one_port(&amba_reg, &uap->port);
    /*解决参数类型不对的问题-- linux内核*/
    /*lint -e516 -e84 -e30 -e806*/
	for (i = 0; (unsigned int)i < ARRAY_SIZE(amba_ports); i++)
		if (amba_ports[i] == uap)
			amba_ports[i] = NULL;
    /*lint +e516 +e84 +e30 +e806*/
	iounmap(uap->port.membase);
	clk_put(uap->clk);
	kfree(uap);
	return 0;
}

#ifdef CONFIG_PM
static int uartv7r1_suspend(struct amba_device *dev, pm_message_t state)
{
	struct uart_balong_port *uap = amba_get_drvdata(dev);

	if (!uap)
		return -EINVAL;

	return uart_suspend_port(&amba_reg, &uap->port);
}

static int uartv7r1_resume(struct amba_device *dev)
{
	struct uart_balong_port *uap = amba_get_drvdata(dev);

	if (!uap)
		return -EINVAL;

	return uart_resume_port(&amba_reg, &uap->port);
}
#endif

static struct amba_id uartv7r1_ids[] = {
	{
		.id	= 0x000c21c0,
		.mask	= 0x000fffff,
		.data	= &vendor_hisilicon,
	},
	{ 0, 0 },
};

static struct amba_driver uartv7r1_driver = {
	.drv = {
		.name	= "uart-BlongV7R1",
	},
	.id_table	= uartv7r1_ids,
	.probe		= uartv7r1_probe,
	.remove		= uartv7r1_remove,
#ifdef CONFIG_PM
	.suspend	= uartv7r1_suspend,
	.resume		= uartv7r1_resume,
#endif
};

static int __init uartv7r1_init(void)
{
	int ret;
	printk(KERN_INFO "Serial: BalongV7R1 UART driver\n");

	/*V3R2 CS f00164371，保持回片走相同代码，不再添加6756宏隔离*/
	#if defined (BOARD_SFT) && defined (VERSION_V3R2)
	#elif defined (BOARD_SFT) && defined (VERSION_V7R1)
	#elif defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2)
	#elif defined (BOARD_FPGA) && defined (VERSION_V3R2)
		/*set uart0 for cpu1*/
		*(volatile int *)0xf1001838 = 0x01020101;
	#else
	//#error "there is no file included!"
	#endif

	ret = uart_register_driver(&amba_reg);
	if (ret == 0) {
		ret = amba_driver_register(&uartv7r1_driver);
		if (ret)
			uart_unregister_driver(&amba_reg);
	}
	return ret;
}

static void __exit uartv7r1_exit(void)
{
	amba_driver_unregister(&uartv7r1_driver);
	uart_unregister_driver(&amba_reg);
}

arch_initcall(uartv7r1_init);
module_exit(uartv7r1_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon Tech.Co.,Ltd.<czluo@huawei.com>");
MODULE_DESCRIPTION("BalongV7R1 serial port driver");
/*lint +e40 */

