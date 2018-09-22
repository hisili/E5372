/*
 *  This file is used to debug balong android driver memory management and
 *  It is free. Balong android usesslub mechanism to alloc memory
 */
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <asm/uaccess.h>

#define BALONG_DEBUG_BUF_SIZE CONFIG_BALONG_MEMORY_DEBUG

#if BALONG_DEBUG_BUF_SIZE

//static DECLARE_MUTEX(mr_sem); // by xiechanglong
static DEFINE_SPINLOCK(mr_lock);
static char huge[BALONG_DEBUG_BUF_SIZE];
static int offset = 0;
static int enable = 1;


void balong_switch(unsigned long receive)
{			
	int i;	
	unsigned char j;
	
	for(i = 0; i < 8; i++)
	{
		j = ((receive >> (28 - 4*i))&0xf);		
			
		if((unsigned long)j >= 0x0 && (unsigned long)j <= 0x9)
			huge[offset++] = j + 0x30;
		else if((unsigned long)j >= 0xa && (unsigned long)j <= 0xf)
			huge[offset++] = j + 0x57;

	}
	return;
}

void balong_memory_debug_reset()
{
	unsigned long flags;

	printk("offset: %u\n", offset);

	spin_lock_irqsave(&mr_lock, flags);
	offset = 0;
	memset(huge, 0, BALONG_DEBUG_BUF_SIZE);
	enable = 1;
	spin_unlock_irqrestore(&mr_lock, flags);

}
EXPORT_SYMBOL(balong_memory_debug_reset);

void balong_memory_debug_malloc_trace(unsigned long this_ip, void * ret, size_t alloc_size)
{
	unsigned long flags;
	if(enable){
		if (offset < BALONG_DEBUG_BUF_SIZE - 28){
			
			spin_lock_irqsave(&mr_lock, flags);
			
			huge[offset++] = 'M';
			huge[offset++] = ',';
			
			balong_switch(this_ip);		
			huge[offset++] = ',';

			balong_switch((unsigned long)ret);		
			huge[offset++] = ',';
			
			balong_switch(alloc_size);
			huge[offset++] = '\n';		

			spin_unlock_irqrestore(&mr_lock, flags);
		}
		else
        {      
			printk("============Huge buffer is full============\n");
            enable = 0;
        }
	}
	return;
}

EXPORT_SYMBOL_GPL(balong_memory_debug_malloc_trace);

void balong_memory_debug_free_trace(const void * x)
{
	unsigned long flags;
	if(enable){
		if (offset < BALONG_DEBUG_BUF_SIZE - 11){			

			spin_lock_irqsave(&mr_lock, flags);

			huge[offset++] = 'F';
			huge[offset++] = ',';

			balong_switch((unsigned long)x);
			huge[offset++] = '\n';

			spin_unlock_irqrestore(&mr_lock, flags);
		}
		else
        {      
			printk("============Huge buffer is full============\n");
            enable = 0;
        }
	}
	return;	
}

EXPORT_SYMBOL_GPL(balong_memory_debug_free_trace);

static int balong_memory_debug_open(struct inode *inode, 
		struct file *filp)
{
	return 0;
}

static ssize_t balong_memory_debug_write(struct file *file,
		const char __user *buf,
		size_t length, loff_t *ppos)
{
	if (buf[0] == '1'){
		enable = 1;
		printk ("Debug ==> enable:%d\n", enable);
	}
	else if (buf[0] == '0'){
		enable = 0;
		//memset(huge, 0, BALONG_DEBUG_BUF_SIZE);
		printk ("Debug ==> disable:%d\n", enable);
	}
	return length;
}

static ssize_t balong_memory_debug_read(struct file *file,
		char __user *buf,
		size_t length, loff_t *ppos)
{
	unsigned long len;

	if (*ppos >= offset)
		return 0;

	if (offset - *ppos < length)
		len = offset - *ppos;
	else 
		len = length;

	copy_to_user(buf, huge + *ppos, len);

	*ppos += len;

	return len;
}

static const struct file_operations proc_fops = {
	.owner		= THIS_MODULE,
	.open            = balong_memory_debug_open,
	.read		= balong_memory_debug_read,
	.write		= balong_memory_debug_write,	
};


static struct proc_dir_entry *p;

int __init balong_memory_debug_init(void){
		
	printk ("*************Make balong_memory_debug proc successfully.*************\n");
	p = proc_create("balong_memory_debug", 0660, NULL, &proc_fops);
	
	if (!p) {
		printk ("Proc file fail.\n");
		return -1;
	}
	return 0;
}

void __exit balong_memory_debug_exit(void){
	remove_proc_entry("balong_memory_debug", NULL);
	return ;
}

module_init(balong_memory_debug_init);
module_exit(balong_memory_debug_exit);
MODULE_LICENSE("GPL");	

#endif


