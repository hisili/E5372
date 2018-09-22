#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/console.h>

#include <mach/balong_v100r001.h>
#include <mach/common/bsp_memory.h>
#include <DrvInterface.h>

#include <vxprintk.h>

#define __LOG_BUF_LEN	(MEMORY_AXI_EXC_ASHELL_SIZE >> 1)

/*
 * logbuf_lock protects log_buf, log_start, log_end, con_start and logged_chars
 * It is also used in interesting ways to provide interlocking in
 * release_console_sem().
 */
static DEFINE_SPINLOCK(logbuf_lock);

typedef struct {
	unsigned magic;
	unsigned log_start;	/* Index into log_buf: next char to be read by syslog() */
	unsigned log_end;	/* Index into log_buf: most-recently-written-char + 1 */
	unsigned logged_chars; /* Number of chars produced since last read+clear operation */
	int log_buf_len;
	char *log_buf;
} vxprintk_buff_t;

static vxprintk_buff_t*  g_vxprintk_buff = NULL ;
#define VXPRINTK_BUFF_MAGIC 0x32324554

#define LOG_BUF_MASK (__LOG_BUF_LEN -1)
#define LOG_BUF(idx) (g_vxprintk_buff->log_buf[(idx) & LOG_BUF_MASK])

static BSP_BOOL vxprintk_enable = BSP_TRUE;

/*
 * Return the number of unread characters in the log buffer.
 */
int vxprintk_get_len(void)
{
	return g_vxprintk_buff->logged_chars;
}
EXPORT_SYMBOL(vxprintk_get_len);
/*
 * Clears the ring-buffer
 */
void vxprintk_buf_clear(void)
{
	spin_lock_irq(&logbuf_lock);

	g_vxprintk_buff->logged_chars = 0;
	g_vxprintk_buff->log_start = 0;
	g_vxprintk_buff->log_end = 0;
	
	spin_unlock_irq(&logbuf_lock);
}
EXPORT_SYMBOL(vxprintk_buf_clear);


void emit_log_char(char c)
{
	LOG_BUF(g_vxprintk_buff->log_end) = c;
	g_vxprintk_buff->log_end++;
	if (g_vxprintk_buff->log_end - g_vxprintk_buff->log_start > g_vxprintk_buff->log_buf_len)
		g_vxprintk_buff->log_start = g_vxprintk_buff->log_end - g_vxprintk_buff->log_buf_len;
	if (g_vxprintk_buff->logged_chars < g_vxprintk_buff->log_buf_len)
		g_vxprintk_buff->logged_chars++;
}
EXPORT_SYMBOL(emit_log_char);

int vxprintk_read(char* buffer, const unsigned len)
{
	int i = 0;
	char c;
	
	spin_lock_irq(&logbuf_lock);

	while ((g_vxprintk_buff->log_start != g_vxprintk_buff->log_end) && i < len) 
	{
		c = LOG_BUF(g_vxprintk_buff->log_start);
		g_vxprintk_buff->log_start++;
		*buffer = c;
		buffer++;
		i++;
	}
	g_vxprintk_buff->logged_chars -= i;
	spin_unlock_irq(&logbuf_lock);
	
	return i;
}
EXPORT_SYMBOL(vxprintk_read);
	
int vxprintk_write(const char* buffer, const unsigned len)
{
	char* buff = (char*)buffer;

	spin_lock_irq(&logbuf_lock);

	if(NULL == g_vxprintk_buff)
	{
		vxprintk_init();
	}

	while(buff < buffer + len)
	{

		(void)emit_log_char(*buff);
		buff++;
	}
	spin_unlock_irq(&logbuf_lock);
	return buff - buffer;
}
EXPORT_SYMBOL(vxprintk_write);

void vxprintk_write_char(char c)
{
	if(NULL == g_vxprintk_buff || BSP_TRUE != vxprintk_enable)
	{
		return;
	}
	(void)emit_log_char(c);
}
EXPORT_SYMBOL(vxprintk_write_char);

void vxprintk_stop()
{
	vxprintk_enable = BSP_FALSE;
	memcpy(MEMORY_AXI_EXC_ASHELL_ADDR + __LOG_BUF_LEN, MEMORY_AXI_EXC_ASHELL_ADDR, __LOG_BUF_LEN);
}
EXPORT_SYMBOL(vxprintk_stop);

void vxprintk_init()
{
	g_vxprintk_buff = MEMORY_AXI_EXC_ASHELL_ADDR;
	if(VXPRINTK_BUFF_MAGIC != g_vxprintk_buff->magic
		|| g_vxprintk_buff->log_buf_len != __LOG_BUF_LEN - sizeof(vxprintk_buff_t)
		|| g_vxprintk_buff->log_buf != g_vxprintk_buff + sizeof(vxprintk_buff_t))
	{
		memset(MEMORY_AXI_EXC_ASHELL_ADDR, 0, MEMORY_AXI_EXC_ASHELL_SIZE);
		g_vxprintk_buff->magic = VXPRINTK_BUFF_MAGIC;
		g_vxprintk_buff->log_start = 0;
		g_vxprintk_buff->log_end = 0;
		g_vxprintk_buff->logged_chars = 0;
		g_vxprintk_buff->log_buf_len = __LOG_BUF_LEN;
		g_vxprintk_buff->log_buf = g_vxprintk_buff + sizeof(vxprintk_buff_t);
		
	}
	vxprintk_enable = BSP_TRUE;
}

//console_initcall(vxprintk_init);

