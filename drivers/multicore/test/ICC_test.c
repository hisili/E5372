
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/slab.h>

#include <mach/spinLockAmp.h>
#include "BSP.h"
#include <mach/common/bsp_memory.h>
#include <mach/hardware.h>
#include "bsp_icc_drv.h"

#define ICC_TEST_SEND_LEN 10240
#define ICC_TEST_SEND_LEN2 40840000
static BSP_BOOL g_bInit = BSP_FALSE;

static BSP_U8 g_senddata[4096];
BSP_U32 g_totallen = 0;
BSP_U32 g_sendlen = 0;
static BSP_U8 g_recvdata[4096];
BSP_U32 g_asendlen[32] = {0};
BSP_U32 g_asendtime[32] = {0};
BSP_U8 *g_asendp[32] = {0};
BSP_U8 *g_asenddata[32] = {0};
BSP_U8 *g_arecvdata[32] = {0};


static struct task_struct *g_u32IccSendTaskId[32];
struct semaphore g_semIccSendTask[32];
struct semaphore g_semInitTask;

static volatile BSP_U32 g_arecvtotal[32] = {0};
static volatile BSP_U32 g_arecvtotalb[32] = {0};
static volatile BSP_U32 g_asendtotal[32] = {0};
static volatile BSP_U32 g_asendsec[32] = {0};

extern BSP_VOID ICC_SetTestMode(BSP_BOOL bEnable);

#define DEV_INIT()\
do{\
	if(!g_bInit)\
	{\
		if(OK != BSP_ICC_Init())\
		{\
			printk("init fail...\n");\
			return ERROR;\
		}\
		ICC_SetTestMode(BSP_TRUE);\
		g_bInit = BSP_TRUE;\
	}\
}while(0)

#define CHECK_RET(a, b)\
do{\
	BSP_S32 x;\
	x = a;\
	if(x != b)\
	{\
		printk("%s(%d): ret(0x%x) error...\n", __FUNCTION__, __LINE__, x);\
		return ERROR;\
	}\
}while(0)


#define CHECK_VAL(a, b)\
do{\
	if(a != b)\
	{\
		printk("%s(%d): value error... 0x%x 0x%x\n", __FUNCTION__, __LINE__, a, b);\
		return ERROR;\
	}\
}while(0)


static struct task_struct *g_s32IccTasktestId;


BSP_U32 *pLock1 = (BSP_U32 *)(MEMORY_AXI_MEMMGR_FLAG_ADDR);
BSP_U32 *pLock2 = (BSP_U32 *)(SM_BASE_ADDR+12);
BSP_U32 *psharedata = (BSP_U32 *)(SM_BASE_ADDR+16);
BSP_U32 *psharedata2 = (BSP_U32 *)(SM_BASE_ADDR+20);
BSP_U32 *psharedata3 = (BSP_U32 *)(SM_BASE_ADDR+24);
BSP_U32 *psharedata4 = (BSP_U32 *)(SM_BASE_ADDR+28);


BSP_S32 spinlocktest_take(BSP_U32 *pLock)
{
	spinLockAmpTake(pLock);
	printk("lock=%d\n", *pLock);
	return 0;
}

static int spin_task(void *__unused)
{
	int i;

	for (i=0; i<100; i++)
	{
		printk("try lock1=%d\n", *pLock1);
		spinLockAmpTake(pLock1);
		printk("get lock1=%u\n", *pLock1);

		msleep(10);

		printk("try lock2=%d\n", *pLock2);
		spinLockAmpTake(pLock2);
		printk("get lock2=%u\n", *pLock2);

		msleep(10);

		printk("give lock2=%d\n", *pLock2);
		spinLockAmpGive(pLock2);
		printk("give lock2=%u\n", *pLock2);
	}
	return 0;
}

BSP_S32 spinlocktest_trytake(void)
{
	g_s32IccTasktestId = kthread_run(spin_task, NULL, "tspinTask");
	if (IS_ERR(g_s32IccTasktestId))
	{
		printk("create kthread spin_task failed!\n");
		return -1;
	}
	
	return 0;
}

BSP_S32 spinlocktest_give(BSP_U32 *pLock)
{
	spinLockAmpGive(pLock);
	return 0;
}


BSP_S32 spinlocktest_add(void)
{
	int i = 0;
	volatile BSP_U32 *pdata = psharedata;

#ifdef BSP_CORE_MODEM
	pLock2[0] = 0;
	psharedata[0] = 0;
	psharedata2[0] = 1;
	psharedata3[0] = 1;
	psharedata4[0] = 1;
	while(psharedata2[0])
	{

	}
#else
	psharedata2[0] = 0;
#endif

	printk("add...\n");

	for (i=0; i<1000000; i++)
	{
		spinLockAmpTake(pLock2);
		pdata[0]++;
		spinLockAmpGive(pLock2);
	}
	printk("done...\n");

#ifdef BSP_CORE_MODEM
	psharedata3[0] = 0;
#else
	psharedata4[0] = 0;
#endif
	while(psharedata3[0] || psharedata4[0])
	{

	}

	printk("psharedata=%d\n", pdata[0]);
	return 0;
}


BSP_S32 spinlocktest_add2(void)
{
	int i = 0;
	volatile BSP_U32 *pdata = psharedata;

#ifdef BSP_CORE_MODEM
	psharedata[0] = 0;
	psharedata2[0] = 1;
	psharedata3[0] = 1;
	while(psharedata2[0])
	{

	}
#else
	psharedata2[0] = 0;
#endif
	printk("add...\n");

	for (i=0; i<1000000; i++)
	{
		pdata[0]++;
	}
	printk("done...\n");

#ifdef BSP_CORE_MODEM
	psharedata3[0] = 0;
#else
	psharedata4[0] = 0;
#endif
	while(psharedata3[0] || psharedata4[0])
	{

	}
	printk("psharedata=%d\n", pdata[0]);
	return 0;
}


BSP_S32 show_recv_total(void)
{
	int i = 0;

	for (i=0; i<32; i++)
	{
		printk("chan[%d], Rt=%d rest=%d Tt=%d\n", i, g_arecvtotal[i], g_asendlen[i], g_asendtotal[i]);
	}
	return 0;
}

BSP_U32 event_cb(BSP_U32 id, BSP_U32 event, BSP_VOID* Param)
{
	printk("chan[%d], event:%d\n", id,event);
	return 0;
}

BSP_U32 write_cb(BSP_U32 id)
{
	BSP_S32 ret = 0;

	if (!g_sendlen)
	{
		return 0;
	}

	ret = BSP_ICC_Write(id, g_senddata, g_sendlen);
	if (ret < 0 || ret > g_sendlen)
	{
		printk("[%d],write cb fail!, 0x%x\n", __LINE__, ret);
		return 0;
	}
	else if (ret < g_sendlen)
	{
		g_sendlen = g_sendlen - ret;
	}
	else
	{
		printk("[%d],write cb OK!, 0x%x\n", __LINE__, ret);
		g_sendlen = 0;
		up(&g_semIccSendTask[id]);
	}

	return 0;
}



BSP_U32 write_cb2(BSP_U32 id)
{
	up(&g_semIccSendTask[id]);
	return 0;
}


BSP_U32 write_cb3(BSP_U32 id)
{
	BSP_S32 ret;

	if (!g_sendlen)
	{
		return 0;
	}

	ret = BSP_ICC_Write(id, g_senddata, g_sendlen);
	if (ret < 0 || ret > g_sendlen)
	{
		printk("[%d],write cb fail!, 0x%x\n", __LINE__, ret);
		return 0;
	}
	else if (ret <= g_sendlen)
	{
		g_sendlen = g_sendlen - ret;
	}
	else
	{
		//        printk("[%d],write cb OK!, 0x%x", __LINE__, ret);
	}

	if (g_sendlen ==0)
	{
		up(&g_semIccSendTask[id]);
	}

	return 0;
}


BSP_U32 write_cb_p(BSP_U32 id)
{
	BSP_S32 ret = 0;

	if (!g_sendlen)
	{
		return 0;
	}

	ret = BSP_ICC_Write(id, g_senddata, 4084);
	if (BSP_ERR_ICC_BUFFER_FULL == ret)
	{
		printk("buffer full!\n");
	}
	else if (ret == g_sendlen)
	{
		g_sendlen = 0;
		printk("write size %d\n", ret);
	}
	else
	{
		printk("%s(%d): ret error...\n", __FUNCTION__, __LINE__);
	}

	return 0;
}



BSP_U32 write_cb_p1(BSP_U32 id)
{
	if (!g_sendlen)
	{
		return 0;
	}

	while (g_sendlen>0)
	{
		BSP_S32 ret;
		ret = BSP_ICC_Write(id, g_senddata, 4084);
		if (BSP_ERR_ICC_BUFFER_FULL == ret)
		{
			break;
		}
		else if (ret == 4084)
		{
			g_sendlen -= 4084;
		}
		else
		{
			printk("%s(%d): ret error...\n", __FUNCTION__, __LINE__);
			return (BSP_U32)ERROR;
		}
	}

	if (g_sendlen == 0)
	{
		up(&g_semIccSendTask[id]);
	}

	return 0;
}

BSP_U32 write_cb_p2(BSP_U32 id)
{
	up(&g_semIccSendTask[id]);
	return 0;
}


BSP_U32 read_cb(BSP_U32 id, BSP_S32 size)
{
	int i = 0;

	CHECK_RET(BSP_ICC_Read(id, g_recvdata, size), size);
	g_arecvtotal[id] +=size;
	//    printk("chan[%d] recv data, size=%d T=%d %d!\n", id, size, g_arecvtotal[id], g_arecvtotalb[id]);
	if (g_arecvtotal[id] == g_arecvtotalb[id])
	{
		printk("R: chan[%d] s=%d t=%d\n", id, size, g_arecvtotal[id]);
		up(&g_semIccSendTask[id]);
	}
#if 1   /* BEGIN: Modified by z67193, 2011-3-21 */
	for (i=0; (i<size); i++)
	{
		printk(" [0x%x]", g_recvdata[i]);
	}
	printk("\n");
#endif  /* END:   Modified by z67193, 2011-3-21 */

	return 0;
}

#if 0   /* BEGIN: Modified by z67193, 2011-3-26 */

BSP_U32 read_cb1(BSP_U32 id, BSP_S32 size)
{
	//    int i;

	CHECK_RET(BSP_ICC_Read(id, g_recvdata, size), size);
	printk("chan[%d] recv data, size=%d!", id, size);

	return 0;
}

#endif  /* END:   Modified by z67193, 2011-3-26 */

BSP_U32 read_cb2(BSP_U32 id, BSP_S32 size)
{
	CHECK_RET(BSP_ICC_Read(id, g_recvdata, size), size);
	g_arecvtotal[id] += size;
	if (g_arecvtotalb[id] <= g_arecvtotal[id])
	{
		printk("R: c[%d] s=%d t=%d\n", id, size, g_arecvtotal[id]);
		up(&g_semIccSendTask[id]);
	}

	return 0;
}



/* 打开非对称通道成功 */
BSP_S32 open_1_channel(BSP_U32 id)
{
	ICC_CHAN_ATTR_S attr;

	DEV_INIT();

	attr.enChanMode = ICC_CHAN_MODE_STREAM;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.event_cb = BSP_NULL;
	attr.read_cb = BSP_NULL;
	attr.write_cb = BSP_NULL;
#ifdef BSP_CORE_MODEM
	attr.u32FIFOInSize = 128;
	attr.u32FIFOOutSize = 128;
#else
	attr.u32FIFOInSize = 128;
	attr.u32FIFOOutSize = 128;
#endif

	CHECK_RET(BSP_ICC_Open(id, &attr), BSP_OK);

	return OK;
}


BSP_S32 open_1_channel2(BSP_U32 id)
{
	ICC_CHAN_ATTR_S attr;

	//DEV_INIT();

	attr.enChanMode = ICC_CHAN_MODE_STREAM;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.u32FIFOInSize = 128;
	attr.u32FIFOOutSize = 128;
	attr.event_cb = event_cb;
	attr.read_cb = read_cb;
	attr.write_cb = write_cb;

	CHECK_RET(BSP_ICC_Open(id, &attr), BSP_OK);

	return OK;
}


BSP_S32 open_1_channel2P(BSP_U32 id)
{
	ICC_CHAN_ATTR_S attr;

	//DEV_INIT();

	attr.enChanMode = ICC_CHAN_MODE_PACKET;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.u32FIFOInSize = 256;
	attr.u32FIFOOutSize = 256;
	attr.event_cb = event_cb;
	attr.read_cb = read_cb;
	attr.write_cb = write_cb_p;

	CHECK_RET(BSP_ICC_Open(id, &attr), BSP_OK);

	return OK;
}


BSP_S32 open_1_channel_large(BSP_U32 id)
{
	ICC_CHAN_ATTR_S attr;

	//DEV_INIT();

	attr.enChanMode = ICC_CHAN_MODE_STREAM;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.u32FIFOInSize = 1024;
	attr.u32FIFOOutSize = 1024;
	attr.event_cb = event_cb;
	attr.read_cb = read_cb2;
	attr.write_cb = write_cb3;

	CHECK_RET(BSP_ICC_Open(id, &attr), BSP_OK);

	return OK;
}


BSP_S32 open_1_channel_large2(BSP_U32 id)
{
	ICC_CHAN_ATTR_S attr;

	//DEV_INIT();

	attr.enChanMode = ICC_CHAN_MODE_STREAM;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.u32FIFOInSize = 1024;
	attr.u32FIFOOutSize = 1024;
	attr.event_cb = event_cb;
	attr.read_cb = read_cb2;
	attr.write_cb = write_cb2;

	CHECK_RET(BSP_ICC_Open(id, &attr), BSP_OK);

	return OK;
}

BSP_S32 open_1_channel_largeP(BSP_U32 id)
{
	ICC_CHAN_ATTR_S attr;
    BSP_S32 ret;
	//DEV_INIT();

	attr.enChanMode = ICC_CHAN_MODE_PACKET;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.u32FIFOInSize = 4096;
	attr.u32FIFOOutSize = 4096;
	attr.event_cb = event_cb;
	attr.read_cb = read_cb2;
	attr.write_cb =BSP_NULL;// write_cb_p1;

	//CHECK_RET(BSP_ICC_Open(id, &attr), BSP_OK);
      ret= BSP_ICC_Open(id, &attr);
	if (ret == BSP_OK )
		{
	     return OK;
		}
	else 
		{
	     return ERROR;
	}
}

BSP_S32 open_1_channel_largeP2(BSP_U32 id)
{
	ICC_CHAN_ATTR_S attr;

	//DEV_INIT();

	attr.enChanMode = ICC_CHAN_MODE_PACKET;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.u32FIFOInSize = 4096;
	attr.u32FIFOOutSize = 4096;
	attr.event_cb = event_cb;
	attr.read_cb = read_cb2;
	attr.write_cb = write_cb_p2;

	CHECK_RET(BSP_ICC_Open(id, &attr), BSP_OK);

	return OK;
}


BSP_S32 icc_rw_test_id(BSP_U32 chan, BSP_S32 size)
{
	int i = 0;

	while (i++ <100)
	{
		g_senddata[i] = i;
	}

	CHECK_RET(BSP_ICC_Write(chan, g_senddata, size), size);

	printk("TEST RW CASE PASSED\n");

	return OK;
}

BSP_S32 icc_rw_test(BSP_S32 size)
{
	int i = 0;
	int ret;

	while (i++ <100)
	{
		g_senddata[i] = i;
	}

	ret = BSP_ICC_Write(0, g_senddata, size);
	if (ret < 0 || ret > size)
	{
		printk("[%d],write fail!, 0x%x", __LINE__, ret);
		return 0;
	}
	else if (ret < size)
	{
		g_sendlen = size - ret;
	}
	else
	{
		printk("[%d],write OK!, 0x%x", __LINE__, ret);
	}

	printk("TEST RW CASE PASSED\n");

	return OK;
}

BSP_S32 icc_openp(BSP_U32 id)
{
	ICC_CHAN_ATTR_S attr;

	DEV_INIT();

	attr.enChanMode = ICC_CHAN_MODE_PACKET;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.event_cb = event_cb;
	attr.read_cb = read_cb;
	attr.write_cb = write_cb;
#ifdef BSP_CORE_MODEM
	attr.u32FIFOInSize = 1024;
	attr.u32FIFOOutSize = 1024;
#else
	attr.u32FIFOInSize = 1024;
	attr.u32FIFOOutSize = 1024;
#endif

	CHECK_RET(BSP_ICC_Open(id, &attr), BSP_OK);

	printk("TEST OPEN CASE 4 PASSED\n");

	return OK;
}

BSP_S32 icc_rw_testp(BSP_S32 size)
{
	int i = 0;
	int ret;

	while (i++ <1024)
	{
		g_senddata[i] = i;
	}

	ret = BSP_ICC_Write(0, g_senddata, size);
	if (ret < 0 || ret > size)
	{
		printk("[%d],write fail!, 0x%x", __LINE__, ret);
		return 0;
	}
	else if (ret < size)
	{
		g_sendlen = size - ret;
	}
	else
	{
		printk("[%d],write OK!, 0x%x", __LINE__, ret);
	}

	printk("TEST RW CASE PASSED\n");

	return OK;
}

BSP_S32 icc_open_task(void *data)
{
	int i = 0;
	int id = (int)data;
	ICC_CHAN_ATTR_S attr;
	printk("enter icc_open_task %d\n", id);

	attr.enChanMode = ICC_CHAN_MODE_STREAM;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.event_cb = BSP_NULL;
	attr.read_cb = BSP_NULL;
	attr.write_cb = BSP_NULL;
	attr.u32FIFOInSize = 128;
	attr.u32FIFOOutSize = 128;

	while (i++<100)
	{
		CHECK_RET(BSP_ICC_Open(id, &attr), BSP_OK);
		msleep(1000);

		CHECK_RET(BSP_ICC_Close(id), BSP_OK);
		msleep(10);
	}

	printk("TEST OPEN CLOSE CASE 13 chan[%d] PASSED\n", id);
	return 0;
}

BSP_S32 icc_write_task(void *data)
{
	int id = (int)data;
	BSP_U8 *p = g_asenddata[id];
	BSP_S32 ret = 0;

	printk("write task chan %d\n", id);

	sema_init(&g_semIccSendTask[id], SEM_EMPTY);

	if (BSP_OK != open_1_channel_large2(id))
	{
		printk("[%d]TEST OPEN CASE FAILED\n", __LINE__);
		return -1;
	}

	p = kmalloc(ICC_TEST_SEND_LEN, GFP_KERNEL);
	if (!p)
	{
		printk("[%d]TEST OPEN CASE FAILED\n", __LINE__);
		return -1;
	}
	g_asenddata[id] = p;

	g_asendlen[id] = g_arecvtotalb[id];

	while (g_asendlen[id])
	{
		ret = BSP_ICC_Write(id, g_asenddata[id], g_asendlen[id]);
		if (ret < 0 || ret > g_asendlen[id])
		{
			printk("chan[%d] %s(%d): ret(0x%x) error... 0x%x\n", id, __FUNCTION__, __LINE__, ret, g_asendlen[id]);
			return ERROR;
		}
		else if (ret < g_asendlen[id])
		{
			g_asendtotal[id] += ret;
			g_asendlen[id] -= ret;
			down(&g_semIccSendTask[id]);
		}
		else
		{
			g_asendtotal[id] += ret;
			g_asendlen[id] = 0;
		}
	}

	printk("========== chan[%d] write done! T = %d\n", id, g_asendtotal[id]);


	if (g_arecvtotal[id] != g_arecvtotalb[id])
	{
		down(&g_semIccSendTask[id]);
	}
	CHECK_RET(BSP_ICC_Close(id), BSP_OK);
	kfree(p);

	return 0;
}

BSP_S32 icc_write_taskb(void *data)
{
	int id = (int)data;
	BSP_U8 *p = g_asenddata[id];
	BSP_S32 ret;

	printk("write task chan %d\n", id);
	//sema_init(&g_semIccSendTask[id], SEM_EMPTY);

	p = kmalloc(ICC_TEST_SEND_LEN, GFP_KERNEL);
	if (!p)
	{
		printk("[%d]TEST OPEN CASE FAILED\n", __LINE__);
		return -1;
	}
	g_asenddata[id] = p;

	g_asendlen[id] = g_arecvtotalb[id];
    

	while (g_asendlen[id])
	{
		ret = BSP_ICC_Write(id, g_asenddata[id], g_asendlen[id]);
		if (ret < 0 || ret > g_asendlen[id])
		{
			printk("chan[%d] %s(%d): ret(0x%x) error... 0x%x\n", id, __FUNCTION__, __LINE__, ret, g_asendlen[id]);
			return ERROR;
		}
		else if (ret < g_asendlen[id])
		{
			g_asendtotal[id] += ret;
			g_asendlen[id] -= ret;
			down(&g_semIccSendTask[id]);
		}
		else
		{
			g_asendtotal[id] += ret;
			g_asendlen[id] = 0;
		}
	}

	printk("========== chan[%d] write done! T = %d\n", id, g_asendtotal[id]);

	if (g_arecvtotal[id] != g_arecvtotalb[id])
	{
		down(&g_semIccSendTask[id]);
	}

	kfree(p);

	return 0;
}


BSP_S32 icc_write_task2(void *data)
{
	int len = 0;
	int id = (int)data;
	//BSP_U8 *p = g_asenddata[id];
	BSP_S32 ret = 0;

	printk("write task chan %d\n", id);

	//sema_init(&g_semIccSendTask[id], SEM_EMPTY);

	g_asendlen[id] = g_arecvtotalb[id];
	while (g_asendlen[id])
	{
		if (g_asendsec[id] < g_asendlen[id])
		{
			len = g_asendsec[id];
		}
		else
		{
			len = g_asendlen[id];
		}
		ret = BSP_ICC_Write(id, g_senddata, len);
		if (BSP_ERR_ICC_BUFFER_FULL == ret)
		{
			down(&g_semIccSendTask[id]);
			continue;
		}
		else if (ret == len)
		{
			g_asendlen[id] -= len;
			g_asendtotal[id] += len;
		}
		else
		{
			printk("[%d]: ret(0x%x) error...\n", __LINE__, ret);
			return ERROR;
		}
	}

	//printk("========== chan[%d] write done! T = %d\n", id, g_asendtotal[id]);

	if (g_arecvtotal[id] != g_arecvtotalb[id])
	{
		down(&g_semIccSendTask[id]);
	}

	return 0;
}


BSP_S32 icc_write_task2p(void *data)
{
	int len = 0;
	int id = (int)data;
	//BSP_U8 *p = g_asenddata[id];
	BSP_S32 ret;

	sema_init(&g_semIccSendTask[id], SEM_EMPTY);

	if (BSP_OK != open_1_channel_largeP2(id))
	{
		printk("TEST OPEN CASE 10 FAILED\n");
		return -1;
	}

	g_asendlen[id] = ICC_TEST_SEND_LEN2;
	while (g_asendlen[id])
	{
		if (g_asendsec[id] < g_asendlen[id])
		{
			len = g_asendsec[id];
		}
		else
		{
			len = g_asendlen[id];
		}
		ret = BSP_ICC_Write(id, g_senddata, len);
		if (BSP_ERR_ICC_BUFFER_FULL == ret)
		{
			down(&g_semIccSendTask[id]);
			continue;
		}
		else if (ret == len)
		{
			g_asendlen[id] -= len;
			g_asendtotal[id] += len;
		}
		else
		{
			printk("[%d]: ret(0x%x) error...\n", __LINE__, ret);
			return ERROR;
		}
	}

	if (0 == g_asendlen[id])
		printk("========== chan[%d] write done! T = %d\n", id, g_asendtotal[id]);
	//    taskDelay(500);
	//    CHECK_RET(BSP_ICC_Close(id), BSP_OK);

	return 0;
}


/* 初始化测试 */
BSP_S32 BSP_ICC_ST_INIT_001(void)
{
	CHECK_RET(BSP_ICC_Init(), BSP_OK);

	g_bInit = BSP_TRUE;

	printk("TEST INIT CASE 1 PASSED\n");

	return OK;
}


BSP_S32 BSP_ICC_ST_INIT_002(void)
{
	ICC_CHAN_ATTR_S attr;
	BSP_U8 data[20];

	CHECK_RET(BSP_ICC_Open(0, &attr), BSP_ERR_ICC_NOT_INIT);
	CHECK_RET(BSP_ICC_Close(0), BSP_ERR_ICC_NOT_INIT);
	CHECK_RET(BSP_ICC_Write(0, data, 10), BSP_ERR_ICC_NOT_INIT);
	CHECK_RET(BSP_ICC_Read(0, data, 10), BSP_ERR_ICC_NOT_INIT);
	CHECK_RET(BSP_ICC_Ioctl(0, 1, 0), BSP_ERR_ICC_NOT_INIT);

	printk("TEST INIT CASE 2 PASSED\n");

	return OK;
}


/* 异常参数测试 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_001(void)
{
	ICC_CHAN_ATTR_S attr;

	DEV_INIT();

	CHECK_RET(BSP_ICC_Open(31, BSP_NULL), BSP_ERR_ICC_NULL);

	attr.enChanMode = 2;
	attr.u32FIFOInSize = 32;
	attr.u32FIFOOutSize = 4096;
	attr.u32Priority = 255;
	attr.u32TimeOut = 20;
	attr.event_cb = BSP_NULL;
	attr.read_cb = BSP_NULL;
	attr.write_cb = BSP_NULL;
	CHECK_RET(BSP_ICC_Open(31, &attr), BSP_ERR_ICC_INVALID_PARAM);
	attr.enChanMode = ICC_CHAN_MODE_STREAM;

	attr.u32FIFOInSize = 28;
	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_INVALID_PARAM);
	attr.u32FIFOInSize = 32;

	attr.u32FIFOInSize = 33;
	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_INVALID_PARAM);
	attr.u32FIFOInSize = 32;

	attr.u32FIFOInSize = 65540;
	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_INVALID_PARAM);
	attr.u32FIFOInSize = 32;

	attr.u32FIFOOutSize = 28;
	CHECK_RET(BSP_ICC_Open(2, &attr), BSP_ERR_ICC_INVALID_PARAM);
	attr.u32FIFOOutSize = 32;

	attr.u32FIFOOutSize = 65540;
	CHECK_RET(BSP_ICC_Open(2, &attr), BSP_ERR_ICC_INVALID_PARAM);
	attr.u32FIFOOutSize = 4096;

	attr.u32FIFOOutSize = 4095;
	CHECK_RET(BSP_ICC_Open(2, &attr), BSP_ERR_ICC_INVALID_PARAM);
	attr.u32FIFOOutSize = 4096;

	attr.u32Priority = 256;
	CHECK_RET(BSP_ICC_Open(2, &attr), BSP_ERR_ICC_INVALID_PARAM);
	attr.u32Priority = 255;

	printk("TEST OPEN CLOSE CASE 1 PASSED\n");

	return OK;
}


/* 无效测试 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_002(void)
{
	ICC_CHAN_ATTR_S attr;

	DEV_INIT();

	CHECK_RET(BSP_ICC_Open(32, &attr), BSP_ERR_ICC_INVALID_CHAN);

	printk("TEST OPEN CLOSE CASE 2 PASSED\n");

	return OK;
}


/* 单向打开通道 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_003(void)
{
	ICC_CHAN_ATTR_S attr;

	DEV_INIT();

	attr.enChanMode = ICC_CHAN_MODE_STREAM;
	attr.u32FIFOInSize = 32;
	attr.u32FIFOOutSize = 4096;
	attr.u32Priority = 255;
	attr.u32TimeOut = 20;
	attr.event_cb = BSP_NULL;
	attr.read_cb = BSP_NULL;
	attr.write_cb = BSP_NULL;

	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_NEGOTIATE_FAIL);

	printk("TEST OPEN CLOSE CASE 3 PASSED\n");

	return OK;
}


/* 打开对称通道成功 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_004(void)
{
	ICC_CHAN_ATTR_S attr;

	DEV_INIT();

	attr.enChanMode = ICC_CHAN_MODE_STREAM;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.event_cb = BSP_NULL;
	attr.read_cb = BSP_NULL;
	attr.write_cb = BSP_NULL;
#ifdef BSP_CORE_MODEM
	attr.u32FIFOInSize = 1024;
	attr.u32FIFOOutSize = 1024;
#else
	attr.u32FIFOInSize = 1024;
	attr.u32FIFOOutSize = 1024;
#endif

	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);

	printk("TEST OPEN CLOSE CASE 4 PASSED\n");

	msleep(1000);
	CHECK_RET(BSP_ICC_Close(1), BSP_OK);

	return OK;
}


/* 打开非对称通道成功 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_005(void)
{
	ICC_CHAN_ATTR_S attr;

	DEV_INIT();

	attr.enChanMode = ICC_CHAN_MODE_STREAM;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.event_cb = BSP_NULL;
	attr.read_cb = BSP_NULL;
	attr.write_cb = BSP_NULL;
#ifdef BSP_CORE_MODEM
	attr.u32FIFOInSize = 32;
	attr.u32FIFOOutSize = 4096;
#else
	attr.u32FIFOInSize = 4096;
	attr.u32FIFOOutSize = 32;
#endif

	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);

	printk("TEST OPEN CLOSE CASE 5 PASSED\n");

	msleep(1000);
	CHECK_RET(BSP_ICC_Close(1), BSP_OK);

	return OK;
}


/* 双方通道属性不同，打开失败 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_006(void)
{
	ICC_CHAN_ATTR_S attr;

	DEV_INIT();

#ifdef BSP_CORE_MODEM

	attr.enChanMode = ICC_CHAN_MODE_STREAM;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.event_cb = BSP_NULL;
	attr.read_cb = BSP_NULL;
	attr.write_cb = BSP_NULL;
	attr.u32FIFOInSize = 32;
	attr.u32FIFOOutSize = 4096;
	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_NEGOTIATE_FAIL);
#else
	attr.enChanMode = ICC_CHAN_MODE_STREAM;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.event_cb = BSP_NULL;
	attr.read_cb = BSP_NULL;
	attr.write_cb = BSP_NULL;
	attr.u32FIFOInSize = 4096;
	attr.u32FIFOOutSize = 32;

	attr.u32FIFOOutSize = 64;
	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_INVALID_PARAM);
	attr.u32FIFOOutSize = 32;

	attr.u32FIFOInSize = 2048;
	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_INVALID_PARAM);
	attr.u32FIFOInSize = 4096;

	attr.enChanMode = ICC_CHAN_MODE_PACKET;
	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_INVALID_PARAM);
	attr.enChanMode = ICC_CHAN_MODE_STREAM;

	attr.u32Priority = 254;
	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_INVALID_PARAM);
	attr.u32Priority = 255;
#endif

	printk("TEST OPEN CLOSE CASE 6 PASSED\n");

	return OK;
}


/* 重复打开通道 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_007(void)
{
	ICC_CHAN_ATTR_S attr;

	DEV_INIT();

	attr.enChanMode = ICC_CHAN_MODE_STREAM;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.event_cb = BSP_NULL;
	attr.read_cb = BSP_NULL;
	attr.write_cb = BSP_NULL;
#ifdef BSP_CORE_MODEM
	attr.u32FIFOInSize = 32;
	attr.u32FIFOOutSize = 4096;
#else
	attr.u32FIFOInSize = 4096;
	attr.u32FIFOOutSize = 32;
#endif

	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);

#ifdef BSP_CORE_MODEM
	msleep(1000);
	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_OPENED);
#else
	msleep(3000);
	CHECK_RET(BSP_ICC_Close(1), BSP_OK);
	msleep(3000);
#endif

#ifdef BSP_CORE_MODEM
	msleep(3000);
	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_OPENED);
#endif

	printk("TEST OPEN CLOSE CASE 7 PASSED\n");

#ifdef BSP_CORE_MODEM
	msleep(1000);
	CHECK_RET(BSP_ICC_Close(1), BSP_OK);
#endif

	return OK;
}

/* 打开多个通道 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_008(void)
{
	int a = 0;

	DEV_INIT();

	for(a=1; a<32; a++)
	{
		if(BSP_OK != open_1_channel(a))
		{
			printk("TEST OPEN CASE 8 FAILED\n");
			return -1;
		}
	}

	msleep(1000);
	for (a=1; a<32; a++)
	{
		CHECK_RET(BSP_ICC_Close(a), BSP_OK);
	}

	printk("TEST OPEN CLOSE CASE 8 PASSED\n");

	return OK;
}

BSP_S32 BSP_ICC_ST_OPEN_CLOSE_009(void)
{
	DEV_INIT();

	CHECK_RET(BSP_ICC_Close(32), BSP_ERR_ICC_INVALID_CHAN);

	printk("TEST OPEN CLOSE CASE 9 PASSED\n");

	return OK;
}


BSP_S32 BSP_ICC_ST_OPEN_CLOSE_010(void)
{
	DEV_INIT();

	open_1_channel(1);

	msleep(1000);

	CHECK_RET(BSP_ICC_Close(1), BSP_OK);

	printk("TEST OPEN CLOSE CASE 10 PASSED\n");

	return OK;
}


BSP_S32 BSP_ICC_ST_OPEN_CLOSE_011(void)
{
	DEV_INIT();

	CHECK_RET(BSP_ICC_Close(1), BSP_ERR_ICC_NOT_OPEN);

	printk("TEST OPEN CLOSE CASE 11 PASSED\n");

	return OK;
}

BSP_S32 BSP_ICC_ST_OPEN_CLOSE_012(void)
{
	int a, b;

	DEV_INIT();

	for(b=0; b<10; b++)
	{
		for(a=1; a<32; a++)
		{
			if(BSP_OK != open_1_channel(a))
			{
				printk("TEST OPEN CLOSE CASE 1 FAILED\n");
				return -1;
			}
		}

		msleep(1000);
		for(a=1; a<32; a++)
		{
			CHECK_RET(BSP_ICC_Close(a), BSP_OK);
		}

		msleep(2000);
	}

	printk("TEST OPEN CLOSE CASE 12 PASSED\n");

	return OK;
}


/* 多任务打开/关闭通道 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_013(BSP_U32 cnt)
{
	int i, j;
	char taskName[30] = {0};

	DEV_INIT();

	for(i = 1; i < cnt+1; i++)
	{
		sprintf(taskName,"iccTest%02d",i);
		g_u32IccSendTaskId[i] = kthread_run(icc_open_task, (void *)i, taskName);
		if (IS_ERR(g_u32IccSendTaskId[i]))
		{
			printk("create kthread icc_open_task failed!\n");
			for (j = 1; j < i; j++)
			{
				kthread_stop(g_u32IccSendTaskId[j]);
			}
			return ERROR;
		}
		
	}    

	return OK;
}


/* 读写测试，单向发送数据 */
BSP_S32 BSP_ICC_ST_RW_STREAM_001(void)
{
	int i;
	ICC_CHAN_ATTR_S attr;

	DEV_INIT();

    	g_arecvtotal[1] = 0;
	g_arecvtotalb[1] = 55+128;

	attr.enChanMode = ICC_CHAN_MODE_STREAM;
	attr.u32FIFOInSize = 128;
	attr.u32FIFOOutSize = 128;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.event_cb = event_cb;
	attr.read_cb = read_cb;
	attr.write_cb = write_cb;
	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);

	sema_init(&g_semIccSendTask[1], SEM_EMPTY);

	i=0;
	while(i++ <100)
	{
		g_senddata[i] = i;
	}

#ifdef BSP_CORE_MODEM
	msleep(100);
	i=0;
	while(i++ <10)
	{
		CHECK_RET(BSP_ICC_Write(1, g_senddata, i), i);
	}

	msleep(100);
	CHECK_RET(BSP_ICC_Write(1, g_senddata, 128), 127);
	g_sendlen = 1;
	down(&g_semIccSendTask[1]);

#else

	down(&g_semIccSendTask[1]);
#endif

	msleep(3000);
	CHECK_RET(BSP_ICC_Close(1), BSP_OK);

	printk("TEST RW STREAM CASE 1 PASSED\n");

	return OK;
}


/* 读写测试，双向发送数据 */
BSP_S32 BSP_ICC_ST_RW_STREAM_002(void)
{
	int i;
	ICC_CHAN_ATTR_S attr;
	int bDone = 0;

	DEV_INIT();

	sema_init(&g_semIccSendTask[1], SEM_EMPTY);

	g_arecvtotal[1] = 0;
	g_arecvtotalb[1] = 55;

	attr.enChanMode = ICC_CHAN_MODE_STREAM;
	attr.u32FIFOInSize = 128;
	attr.u32FIFOOutSize = 128;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.event_cb = event_cb;
	attr.read_cb = read_cb;
	attr.write_cb = write_cb;
	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);

        mdelay(100);
        
	i=0;
	while(i++ <100)
	{
		g_senddata[i] = i;
	}

	i=0;
	while(i++ <10)
	{
		CHECK_RET(BSP_ICC_Write(1, g_senddata, i), i);
	}
        mdelay(1000);

        do
        {
            msleep(1000);
            if (g_arecvtotal[1] != g_arecvtotalb[1])
	{
                continue;
	}

            bDone = 1;
        }while(!bDone);
        
	msleep(3000);
	CHECK_RET(BSP_ICC_Close(1), BSP_OK);

        up(&g_semIccSendTask[1]);

	printk("TEST RW STREAM CASE 2 PASSED\n");

	return OK;
}


/* 读写测试，多通道双向发送数据 */
BSP_S32 BSP_ICC_ST_RW_STREAM_003(void)
{
	int a, b, i;
	int bDone = 0;

	DEV_INIT();

	i=0;
	while(i++ <100)
	{
		g_senddata[i] = i;
	}

	for(a=1; a<32; a++)
	{
		g_arecvtotal[a] = 0;
		g_arecvtotalb[a] = 105;

		sema_init(&g_semIccSendTask[a], SEM_EMPTY);

		if(BSP_OK != open_1_channel2(a))
		{
			printk("TEST RW CASE 3 FAILED\n");
			return -1;
		}
	}

	for(b=1; b<15; b++)
	{
		for(a=1; a<32; a++)
		{
			CHECK_RET(BSP_ICC_Write(a, g_senddata, b), b);
		}
	}


	do
	{
		msleep(1000);
		for(a = 1; a < 32; a++)
		{
			if(g_arecvtotal[a] != g_arecvtotalb[a])
			{
				break;
			}
		}
		if(a == 32)
		{
			bDone = 1;
		}
	}while(!bDone);
        msleep(10000);

	for(a=1; a<32; a++)
	{
		CHECK_RET(BSP_ICC_Close(a), BSP_OK);
	}

	printk("TEST RW STREAM CASE 3 PASSED\n");

	return OK;
}


/* 读写测试，单通道大数据量 */
BSP_S32 BSP_ICC_ST_RW_STREAM_004(void)
{
	int i;

	DEV_INIT();

	sema_init(&g_semIccSendTask[1], SEM_EMPTY);

	i=0;
	while(i++ <100)
	{
		g_senddata[i] = i;
	}
	g_sendlen = 40960;
	g_arecvtotal[1] = 0;
	g_arecvtotalb[1] = 40960;

    	open_1_channel_large(1);

	while(g_sendlen > 0)
	{
	        BSP_S32 ret;
	        ret = BSP_ICC_Write(1, g_senddata, 4096);
		if(ret < 0|| ret>4096)
		{
			printk("%s(%d): ret error...\n", __FUNCTION__, __LINE__);
			return ERROR;
		}
		else if(0 == ret)
		{
			printk("buffer full!\n");
			break;
		}
		else if(ret != 4096)
		{
			g_sendlen -= ret;
			break;
		}
		else
		{
		//            printk("write ok!\n");
		}
	}

	if(g_sendlen > 0 || (g_arecvtotal[1] < g_arecvtotalb[1]))
	{
		down(&g_semIccSendTask[1]);
	}
	if(g_sendlen > 0 || (g_arecvtotal[1] < g_arecvtotalb[1]))
	{
		down(&g_semIccSendTask[1]);
	}
	msleep(1000);
	CHECK_RET(BSP_ICC_Close(1), BSP_OK);

	printk("TEST RW STREAM CASE 4 PASSED\n");

	return OK;
}

/* 读写测试，多通道大数据量 */
BSP_S32 BSP_ICC_ST_RW_STREAM_005(BSP_U32 cnt, BSP_U32 times)
{
	volatile int i, j;
	char taskName[30] = {0};
	int bDone = 0;
        //unsigned long flags = 0;

	DEV_INIT();

	for(i = 1; i < cnt+1; i++)
	{
		g_arecvtotal[i] = 0;
		g_asendtotal[i] = 0;
		g_arecvtotalb[i] = ICC_TEST_SEND_LEN*times;
		sema_init(&g_semIccSendTask[i], SEM_EMPTY);
		if(BSP_OK != open_1_channel_large2(i))
		{
			printk("[%d]TEST OPEN CASE FAILED\n", __LINE__);
			return -1;
		}
	}

	msleep(1000);
        //sema_init(&g_semInitTask, SEM_EMPTY);
        //msleep(1000);
        
	for(i = 1; i < cnt+1; i++)
	{
	        sprintf(taskName,"iccTest%02d",i);
                //down(&g_semInitTask);
		g_u32IccSendTaskId[i] = kthread_run(icc_write_taskb, (void *)i, taskName);
		if (IS_ERR(g_u32IccSendTaskId[i]))
		{
			printk("create kthread icc_write_taskb failed!\n");
			for (j = 1; j < i; j++)
			{
				kthread_stop(g_u32IccSendTaskId[j]);
			}
			return ERROR;
		}
	}
        

	do
	{
		msleep(2000);
		for(i = 1; i < cnt+1; i++)
		{
			if((g_asendlen[i] != 0) || (g_arecvtotal[i] != (g_arecvtotalb[i])))
			{
				break;
			}
		}
		
		if(i == cnt+1)
		{
			bDone = 1;
		}
	}while(!bDone);

	msleep(15000);
	for(i=1; i<cnt+1; i++)
	{
		CHECK_RET(BSP_ICC_Close(i), BSP_OK);
	}

	printk("TEST RW STREAM CASE 5 PASSED\n");

	return OK;
}

/* 读写测试，多通道大数据量 */
BSP_S32 BSP_ICC_ST_RW_STREAM_006(BSP_U32 cnt)
{
	int i, j;
	char taskName[30] = {0};

	DEV_INIT();

	for(i = 1; i < cnt+1; i++)
	{
		g_arecvtotal[i] = 0;
		g_asendtotal[i] = 0;
		g_arecvtotalb[i] = ICC_TEST_SEND_LEN*1000;
	}

	msleep(1000);
	for(i = 1; i < cnt; i++)
	{
		sprintf(taskName,"iccTest%02d",i);
		g_u32IccSendTaskId[i] = kthread_run(icc_write_task, (void *)i, taskName);
		if (IS_ERR(g_u32IccSendTaskId[i]))
		{
			printk("create kthread icc_write_task failed!\n");
			for (j = 1; j < i; j++)
			{
				kthread_stop(g_u32IccSendTaskId[j]);
			}
			return ERROR;
		}
	}

	printk("TEST RW STREAM CASE 6 PASSED\n");

	return OK;
}


/* packet 读写测试，单向发送数据 */
BSP_S32 BSP_ICC_ST_RW_PACKET_001(void)
{
	int i;
	ICC_CHAN_ATTR_S attr;

	DEV_INIT();

    	i=0;
	while(i++ <10)
	{
		g_senddata[i] = i;
	}
	g_sendlen = 244;
	g_arecvtotal[1] = 0;
	g_arecvtotalb[1] = 488+55;

	attr.enChanMode = ICC_CHAN_MODE_PACKET;
	attr.u32FIFOInSize = 256;
	attr.u32FIFOOutSize = 256;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.event_cb = event_cb;
	attr.read_cb = read_cb;
	attr.write_cb = write_cb;
	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);

	sema_init(&g_semIccSendTask[1], SEM_EMPTY);

#ifdef BSP_CORE_MODEM

	i=0;
	while(i++ <10)
	{
	CHECK_RET(BSP_ICC_Write(1, g_senddata, i), i);
	}

	msleep(100);
	CHECK_RET(BSP_ICC_Write(1, g_senddata, 252), BSP_ERR_ICC_PACKET_SIZE);

	msleep(100);
	CHECK_RET(BSP_ICC_Write(1, g_senddata, 248), BSP_ERR_ICC_PACKET_SIZE);

	msleep(1000);
	CHECK_RET(BSP_ICC_Write(1, g_senddata, 244), 244);
	CHECK_RET(BSP_ICC_Write(1, g_senddata, 244), BSP_ERR_ICC_BUFFER_FULL);
	down(&g_semIccSendTask[1]);

#else

	down(&g_semIccSendTask[1]);
#endif

	msleep(3000);
	CHECK_RET(BSP_ICC_Close(1), BSP_OK);
	printk("TEST RW PACKET CASE 1 PASSED\n");

	return OK;
}


/* packet读写测试，双向发送数据 */
BSP_S32 BSP_ICC_ST_RW_PACKET_002(void)
{
	int i;
	ICC_CHAN_ATTR_S attr;
	int bDone = 0;

	DEV_INIT();

	sema_init(&g_semIccSendTask[1], SEM_EMPTY);

    	g_arecvtotal[1] = 0;
	g_arecvtotalb[1] = 55;

	attr.enChanMode = ICC_CHAN_MODE_PACKET;
	attr.u32FIFOInSize = 256;
	attr.u32FIFOOutSize = 256;
	attr.u32Priority = 255;
	attr.u32TimeOut = 1000;
	attr.event_cb = event_cb;
	attr.read_cb = read_cb;
	attr.write_cb = write_cb;
	CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);

        mdelay(100);

	i=0;
	while(i++ <10)
	{
		g_senddata[i] = i;
	}

	i=0;
	while(i++ <10)
	{
		CHECK_RET(BSP_ICC_Write(1, g_senddata, i), i);
	}
        mdelay(100);

        do
        {
            msleep(1000);
            if (g_arecvtotal[1] != g_arecvtotalb[1])
	{
                continue;
	}

            bDone = 1;
        }while(!bDone);
    
	msleep(3000);
	CHECK_RET(BSP_ICC_Close(1), BSP_OK);

	printk("TEST RW PACKET CASE 2 PASSED\n");

	return OK;
}


/* packet读写测试，多通道双向发送数据 */
BSP_S32 BSP_ICC_ST_RW_PACKET_003(void)
{
	int a, b, i;
	int bDone = 0;

	DEV_INIT();

	i=0;
	while(i++ <100)
	{
		g_senddata[i] = i;
	}

	for(a=1; a<32; a++)
	{
		g_arecvtotal[a] = 0;
		g_arecvtotalb[a] = 45;
		
		sema_init(&g_semIccSendTask[a], SEM_EMPTY);
		
		if(BSP_OK != open_1_channel2P(a))
		{
			printk("TEST RW CASE 3 FAILED\n");
			return -1;
		}
	}

	for(b=1; b<10; b++)
	{
		for(a=1; a<32; a++)
		{
			CHECK_RET(BSP_ICC_Write(a, g_senddata, b), b);
		}
	}


	do
	{
		msleep(1000);
		for(a = 1; a < 32; a++)
		{
			if(g_arecvtotal[a] != g_arecvtotalb[a])
			{
				break;
			}
		}
		if(a == 32)
		{
			bDone = 1;
		}
	}while(!bDone);

	msleep(3000);
	for(a=1; a<32; a++)
	{
		CHECK_RET(BSP_ICC_Close(a), BSP_OK);
	}
	printk("TEST RW PACKET CASE 3 PASSED\n");

	return OK;
}


/* packet读写测试，单通道大数据量 */
BSP_S32 BSP_ICC_ST_RW_PACKET_004(void)
{
    BSP_S32 ret;
	DEV_INIT();

	sema_init(&g_semIccSendTask[1], SEM_EMPTY);
	msleep(100);

	ret=open_1_channel_largeP(1);
	if(ret!=OK)
		{
          printk("open_1_channel_largeP ret =%d \n",ret);
          return;
	}

	g_sendlen = 4084000;
	g_arecvtotal[1] = 0;
	g_arecvtotalb[1] = 4084000;

	while(g_sendlen>0)
	{
		BSP_S32 ret;
		ret = BSP_ICC_Write(1, g_senddata, 4084);
		
		if(BSP_ERR_ICC_BUFFER_FULL == ret)
		{
		 continue;//break;
		}
		else if(ret == 4084)
		{
			g_sendlen -= 4084;
		}
		else
		{
		 continue ;
			//printk("%s(%d): ret error...\n", __FUNCTION__, __LINE__);
			//return ERROR;
		}
	}

	if((g_arecvtotal[1] < g_arecvtotalb[1]))
	{
		down(&g_semIccSendTask[1]);
	}

	msleep(3000);
	CHECK_RET(BSP_ICC_Close(1), BSP_OK);

	printk("TEST RW PACKET CASE 4 PASSED\n");

	return OK;
}


/* packet读写测试，多通道大数据量 */
BSP_S32 BSP_ICC_ST_RW_PACKET_005(BSP_U32 cnt, BSP_U32 sec, BSP_U32 pkts)
{
	int i, j;
	char taskName[30] = {0};
	int bDone = 0;

	DEV_INIT();

	for(i = 1; i < cnt+1; i++)
	{
		g_arecvtotal[i] = 0;
		g_asendtotal[i] = 0;
		g_asendsec[i] = sec;
		g_arecvtotalb[i] = sec*pkts;
		
		sema_init(&g_semIccSendTask[i], SEM_EMPTY);
		
		if(BSP_OK != open_1_channel_largeP2(i))
		{
			printk("TEST RW PACKET CASE 5 FAILED\n");
			return -1;
		}
	}

	msleep(100);
	for(i = 1; i < cnt+1; i++)
	{
		sprintf(taskName,"iccTest%02d",i);
		g_u32IccSendTaskId[i] = kthread_run(icc_write_task2, (void *)i, taskName);
		if (IS_ERR(g_u32IccSendTaskId[i]))
		{
			printk("create kthread icc_write_task2 failed!\n");
			for (j = 1; j < i; j++)
			{
				kthread_stop(g_u32IccSendTaskId[j]);
			}
			return ERROR;
		}
	}    

	do
	{
		msleep(2000);
		for(i = 1; i < cnt+1; i++)
		{
			if((g_asendlen[i] != 0) || (g_arecvtotal[i] != (g_arecvtotalb[i])))
			{
				break;
			}
		}
		if(i == cnt+1)
		{
			bDone = 1;
		}
	}while(!bDone);

	msleep(15000);
	for(i=1; i<cnt+1; i++)
	{
		CHECK_RET(BSP_ICC_Close(i), BSP_OK);
	}
	printk("TEST RW PACKET CASE 5 PASSED\n");

	return OK;
}


/* packet读写测试，多通道大数据量2 */
BSP_S32 BSP_ICC_ST_RW_PACKET_006(BSP_U32 cnt, BSP_U32 sec)
{
	int i, j;
	char taskName[30] = {0};

	DEV_INIT();

	for(i = 1; i < cnt+1; i++)
	{
		g_arecvtotal[i] = 0;
		g_asendtotal[i] = 0;
		g_asendsec[i] = sec;
	}

	msleep(100);
	for(i = 1; i < cnt; i++)
	{
		sprintf(taskName,"iccTest%02d",i);
		g_u32IccSendTaskId[i] = kthread_run(icc_write_task2p, (void *)i, taskName);
		if (IS_ERR(g_u32IccSendTaskId[i]))
		{
			printk("create kthread icc_write_task2p failed!\n");
			for (j = 1; j < i; j++)
			{
				kthread_stop(g_u32IccSendTaskId[j]);
			}
			return ERROR;
		}
	}    

	printk("TEST RW PACKET CASE 6 PASSED\n");

	return OK;
}


