/*
 * Linux OS Independent Layer
 *
 * Copyright (C) 2012, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: linux_osl.h 347841 2012-07-30 02:12:54Z $
 */

#ifndef _linux_osl_h_
#define _linux_osl_h_

#include <typedefs.h>

#define USB_ADDR_ALIGN 4

extern void * osl_os_open_image(char * filename);
extern int osl_os_get_image_block(char * buf, int len, void * image);
extern void osl_os_close_image(void * image);
extern int osl_os_image_size(void *image);


#ifdef BCMDRIVER


extern osl_t *osl_attach(void *pdev, uint bustype, bool pkttag);
extern void osl_detach(osl_t *osh);


extern uint32 g_assert_type;


#if defined(BCMASSERT_LOG)
	#define ASSERT(exp) \
	  do { if (!(exp)) osl_assert(#exp, __FILE__, __LINE__); } while (0)
extern void osl_assert(const char *exp, const char *file, int line);
#else
	#ifdef __GNUC__
		#define GCC_VERSION \
			(__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
		#if GCC_VERSION > 30100
			#define ASSERT(exp)	do {} while (0)
		#else
			
			#define ASSERT(exp)
		#endif 
	#endif 
#endif 


#define	OSL_DELAY(usec)		osl_delay(usec)
extern void osl_delay(uint usec);

#define	OSL_PCMCIA_READ_ATTR(osh, offset, buf, size) \
	osl_pcmcia_read_attr((osh), (offset), (buf), (size))
#define	OSL_PCMCIA_WRITE_ATTR(osh, offset, buf, size) \
	osl_pcmcia_write_attr((osh), (offset), (buf), (size))
extern void osl_pcmcia_read_attr(osl_t *osh, uint offset, void *buf, int size);
extern void osl_pcmcia_write_attr(osl_t *osh, uint offset, void *buf, int size);


#define	OSL_PCI_READ_CONFIG(osh, offset, size) \
	osl_pci_read_config((osh), (offset), (size))
#define	OSL_PCI_WRITE_CONFIG(osh, offset, size, val) \
	osl_pci_write_config((osh), (offset), (size), (val))
extern uint32 osl_pci_read_config(osl_t *osh, uint offset, uint size);
extern void osl_pci_write_config(osl_t *osh, uint offset, uint size, uint val);


#define OSL_PCI_BUS(osh)	osl_pci_bus(osh)
#define OSL_PCI_SLOT(osh)	osl_pci_slot(osh)
extern uint osl_pci_bus(osl_t *osh);
extern uint osl_pci_slot(osl_t *osh);
extern struct pci_dev *osl_pci_device(osl_t *osh);


typedef struct {
	bool pkttag;
	bool mmbus;		
	pktfree_cb_fn_t tx_fn;  
	void *tx_ctx;		
	void	*unused[3];
} osl_pubinfo_t;

#define PKTFREESETCB(osh, _tx_fn, _tx_ctx)		\
	do {						\
	   ((osl_pubinfo_t*)osh)->tx_fn = _tx_fn;	\
	   ((osl_pubinfo_t*)osh)->tx_ctx = _tx_ctx;	\
	} while (0)



#define BUS_SWAP32(v)		(v)

	#define MALLOC(osh, size)	osl_malloc((osh), (size))
	#define MFREE(osh, addr, size)	osl_mfree((osh), (addr), (size))
	#define MALLOCED(osh)		osl_malloced((osh))
	extern void *osl_malloc(osl_t *osh, uint size);
	extern void osl_mfree(osl_t *osh, void *addr, uint size);
	extern uint osl_malloced(osl_t *osh);

#define NATIVE_MALLOC(osh, size)		kmalloc(size, GFP_ATOMIC)
#define NATIVE_MFREE(osh, addr, size)	kfree(addr)

#define	MALLOC_FAILED(osh)	osl_malloc_failed((osh))
extern uint osl_malloc_failed(osl_t *osh);


#define	DMA_CONSISTENT_ALIGN	osl_dma_consistent_align()
#define	DMA_ALLOC_CONSISTENT(osh, size, align, tot, pap, dmah) \
	osl_dma_alloc_consistent((osh), (size), (align), (tot), (pap))
#define	DMA_FREE_CONSISTENT(osh, va, size, pa, dmah) \
	osl_dma_free_consistent((osh), (void*)(va), (size), (pa))
extern uint osl_dma_consistent_align(void);
extern void *osl_dma_alloc_consistent(osl_t *osh, uint size, uint16 align, uint *tot, ulong *pap);
extern void osl_dma_free_consistent(osl_t *osh, void *va, uint size, ulong pa);


#define	DMA_TX	1	
#define	DMA_RX	2	


#define	DMA_UNMAP(osh, pa, size, direction, p, dmah) \
	osl_dma_unmap((osh), (pa), (size), (direction))
extern uint osl_dma_map(osl_t *osh, void *va, uint size, int direction);
extern void osl_dma_unmap(osl_t *osh, uint pa, uint size, int direction);


#define OSL_DMADDRWIDTH(osh, addrwidth) do {} while (0)



	#define SELECT_BUS_WRITE(osh, mmap_op, bus_op) mmap_op
	#define SELECT_BUS_READ(osh, mmap_op, bus_op) mmap_op

#define OSL_ERROR(bcmerror)	osl_error(bcmerror)
extern int osl_error(int bcmerror);


#define	PKTBUFSZ	2048   


#include <linuxver.h>           
#include <linux/kernel.h>       
#include <linux/string.h>       

#define OSL_SYSUPTIME()		((uint32)jiffies * (1000 / HZ))
#define	printf(fmt, args...)	printk(fmt , ## args)
#include <linux/kernel.h>	
#include <linux/string.h>	

#define	bcopy(src, dst, len)	memcpy((dst), (src), (len))
#define	bcmp(b1, b2, len)	memcmp((b1), (b2), (len))
#define	bzero(b, len)		memset((b), '\0', (len))



#ifndef __mips__
#define R_REG(osh, r) (\
	SELECT_BUS_READ(osh, \
		({ \
			__typeof(*(r)) __osl_v; \
			BCM_REFERENCE(osh);	\
			switch (sizeof(*(r))) { \
				case sizeof(uint8):	__osl_v = \
					readb((volatile uint8*)(r)); break; \
				case sizeof(uint16):	__osl_v = \
					readw((volatile uint16*)(r)); break; \
				case sizeof(uint32):	__osl_v = \
					readl((volatile uint32*)(r)); break; \
			} \
			__osl_v; \
		}), \
		OSL_READ_REG(osh, r)) \
)
#else 
#define R_REG(osh, r) (\
	SELECT_BUS_READ(osh, \
		({ \
			__typeof(*(r)) __osl_v; \
			BCM_REFERENCE(osh);	\
			__asm__ __volatile__("sync"); \
			switch (sizeof(*(r))) { \
				case sizeof(uint8):	__osl_v = \
					readb((volatile uint8*)(r)); break; \
				case sizeof(uint16):	__osl_v = \
					readw((volatile uint16*)(r)); break; \
				case sizeof(uint32):	__osl_v = \
					readl((volatile uint32*)(r)); break; \
			} \
			__asm__ __volatile__("sync"); \
			__osl_v; \
		}), \
		({ \
			__typeof(*(r)) __osl_v; \
			__asm__ __volatile__("sync"); \
			__osl_v = OSL_READ_REG(osh, r); \
			__asm__ __volatile__("sync"); \
			__osl_v; \
		})) \
)
#endif 

#define W_REG(osh, r, v) do { \
	BCM_REFERENCE(osh);   \
	SELECT_BUS_WRITE(osh, \
		switch (sizeof(*(r))) { \
			case sizeof(uint8):	writeb((uint8)(v), (volatile uint8*)(r)); break; \
			case sizeof(uint16):	writew((uint16)(v), (volatile uint16*)(r)); break; \
			case sizeof(uint32):	writel((uint32)(v), (volatile uint32*)(r)); break; \
		}, \
		(OSL_WRITE_REG(osh, r, v))); \
	} while (0)

#define	AND_REG(osh, r, v)		W_REG(osh, (r), R_REG(osh, r) & (v))
#define	OR_REG(osh, r, v)		W_REG(osh, (r), R_REG(osh, r) | (v))


#define	bcopy(src, dst, len)	memcpy((dst), (src), (len))
#define	bcmp(b1, b2, len)	memcmp((b1), (b2), (len))
#define	bzero(b, len)		memset((b), '\0', (len))


#ifdef __mips__
#include <asm/addrspace.h>
#define OSL_UNCACHED(va)	((void *)KSEG1ADDR((va)))
#define OSL_CACHED(va)		((void *)KSEG0ADDR((va)))
#else
#define OSL_UNCACHED(va)	((void *)va)
#define OSL_CACHED(va)		((void *)va)


#define OSL_CACHE_FLUSH(va, len)

#endif 

#ifdef __mips__
#define OSL_PREF_RANGE_LD(va, sz) prefetch_range_PREF_LOAD_RETAINED(va, sz)
#define OSL_PREF_RANGE_ST(va, sz) prefetch_range_PREF_STORE_RETAINED(va, sz)
#else 
#define OSL_PREF_RANGE_LD(va, sz)
#define OSL_PREF_RANGE_ST(va, sz)
#endif 


#if defined(__i386__)
#define	OSL_GETCYCLES(x)	rdtscl((x))
#else
#define OSL_GETCYCLES(x)	((x) = 0)
#endif 


#define	BUSPROBE(val, addr)	({ (val) = R_REG(NULL, (addr)); 0; })


#if !defined(CONFIG_MMC_MSM7X00A)
#define	REG_MAP(pa, size)	ioremap_nocache((unsigned long)(pa), (unsigned long)(size))
#else
#define REG_MAP(pa, size)       (void *)(0)
#endif 
#define	REG_UNMAP(va)		iounmap((va))


#define	R_SM(r)			*(r)
#define	W_SM(r, v)		(*(r) = (v))
#define	BZERO_SM(r, len)	memset((r), '\0', (len))


#include <linuxver.h>		


#define	PKTGET(osh, len, send)		osl_pktget((osh), (len))
#define	PKTDUP(osh, skb)		osl_pktdup((osh), (skb))
#define PKTLIST_DUMP(osh, buf)
#define PKTDBG_TRACE(osh, pkt, bit)
#define	PKTFREE(osh, skb, send)		osl_pktfree((osh), (skb), (send))
#ifdef CONFIG_DHD_USE_STATIC_BUF
#define	PKTGET_STATIC(osh, len, send)		osl_pktget_static((osh), (len))
#define	PKTFREE_STATIC(osh, skb, send)		osl_pktfree_static((osh), (skb), (send))
#endif 
#define	PKTDATA(osh, skb)		(((struct sk_buff*)(skb))->data)
#define	PKTLEN(osh, skb)		(((struct sk_buff*)(skb))->len)
#define PKTHEADROOM(osh, skb)		(PKTDATA(osh, skb)-(((struct sk_buff*)(skb))->head))
#define PKTTAILROOM(osh, skb) ((((struct sk_buff*)(skb))->end)-(((struct sk_buff*)(skb))->tail))
#define	PKTNEXT(osh, skb)		(((struct sk_buff*)(skb))->next)
#define	PKTSETNEXT(osh, skb, x)		(((struct sk_buff*)(skb))->next = (struct sk_buff*)(x))
#define	PKTSETLEN(osh, skb, len)	__skb_trim((struct sk_buff*)(skb), (len))
#define	PKTPUSH(osh, skb, bytes)	skb_push((struct sk_buff*)(skb), (bytes))
#define	PKTPULL(osh, skb, bytes)	skb_pull((struct sk_buff*)(skb), (bytes))
#define PKTRESERVE(osh, skb, bytes) skb_reserve((struct sk_buff*)(skb), (bytes))
#define	PKTTAG(skb)			((void*)(((struct sk_buff*)(skb))->cb))
#define PKTSETPOOL(osh, skb, x, y)	do {} while (0)
#define PKTPOOL(osh, skb)		FALSE
#define PKTSHRINK(osh, m)		(m)

#ifdef CTFPOOL
#define	CTFPOOL_REFILL_THRESH	3
typedef struct ctfpool {
	void		*head;
	spinlock_t	lock;
	uint		max_obj;
	uint		curr_obj;
	uint		obj_size;
	uint		refills;
	uint		fast_allocs;
	uint 		fast_frees;
	uint 		slow_allocs;
} ctfpool_t;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 22)

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
#define	FASTBUF	(1 << 4)
#define	CTFBUF	(1 << 5)
#else
#define	FASTBUF	(1 << 16)
#define	CTFBUF	(1 << 17)
#endif

#define	PKTSETFAST(osh, skb)	((((struct sk_buff*)(skb))->mac_len) |= FASTBUF)
#define	PKTCLRFAST(osh, skb)	((((struct sk_buff*)(skb))->mac_len) &= (~FASTBUF))
#define	PKTSETCTF(osh, skb)	((((struct sk_buff*)(skb))->mac_len) |= CTFBUF)
#define	PKTCLRCTF(osh, skb)	((((struct sk_buff*)(skb))->mac_len) &= (~CTFBUF))
#define	PKTISFAST(osh, skb)	((((struct sk_buff*)(skb))->mac_len) & FASTBUF)
#define	PKTISCTF(osh, skb)	((((struct sk_buff*)(skb))->mac_len) & CTFBUF)
#define	PKTFAST(osh, skb)	(((struct sk_buff*)(skb))->mac_len)
#else
#define	FASTBUF	(1 << 0)
#define	CTFBUF	(1 << 1)
#define	PKTSETFAST(osh, skb)	((((struct sk_buff*)(skb))->__unused) |= FASTBUF)
#define	PKTCLRFAST(osh, skb)	((((struct sk_buff*)(skb))->__unused) &= (~FASTBUF))
#define	PKTSETCTF(osh, skb)	((((struct sk_buff*)(skb))->__unused) |= CTFBUF)
#define	PKTCLRCTF(osh, skb)	((((struct sk_buff*)(skb))->__unused) &= (~CTFBUF))
#define	PKTISFAST(osh, skb)	((((struct sk_buff*)(skb))->__unused) & FASTBUF)
#define	PKTISCTF(osh, skb)	((((struct sk_buff*)(skb))->__unused) & CTFBUF)
#define	PKTFAST(osh, skb)	(((struct sk_buff*)(skb))->__unused)
#endif 

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
#define	CTFPOOLPTR(osh, skb)	(((struct sk_buff*)(skb))->ctfpool)
#define	CTFPOOLHEAD(osh, skb)	(((ctfpool_t *)((struct sk_buff*)(skb))->ctfpool)->head)
#else
#define	CTFPOOLPTR(osh, skb)	(((struct sk_buff*)(skb))->sk)
#define	CTFPOOLHEAD(osh, skb)	(((ctfpool_t *)((struct sk_buff*)(skb))->sk)->head)
#endif

extern void *osl_ctfpool_add(osl_t *osh);
extern void osl_ctfpool_replenish(osl_t *osh, uint thresh);
extern int32 osl_ctfpool_init(osl_t *osh, uint numobj, uint size);
extern void osl_ctfpool_cleanup(osl_t *osh);
extern void osl_ctfpool_stats(osl_t *osh, void *b);
#endif 

#ifdef CTFMAP
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 14)
#define CTFMAPPTR(osh, skb)	(((struct sk_buff*)(skb))->sp)
#else 
#define CTFMAPPTR(osh, skb)	(((struct sk_buff*)(skb))->list)
#endif 

#define PKTCTFMAP(osh, p) \
do { \
	if (PKTISCTF(osh, p)) { \
		int32 sz; \
		sz = (uint32)(((struct sk_buff *)p)->end) - \
		     (uint32)CTFMAPPTR(osh, p); \
		 \
		if (sz > 0) { \
			_DMA_MAP(osh, (void *)CTFMAPPTR(osh, p), \
			         sz, DMA_RX, p, NULL); \
		} \
		 \
		PKTCLRCTF(osh, p); \
		CTFMAPPTR(osh, p) = NULL; \
	} \
} while (0)
#endif 

#ifdef HNDCTF
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 22)

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
#define	SKIPCT	(1 << 6)
#define	CHAINED	(1 << 7)
#else
#define	SKIPCT	(1 << 18)
#define	CHAINED	(1 << 19)
#endif
#define	PKTSETSKIPCT(osh, skb)	(((struct sk_buff*)(skb))->mac_len |= SKIPCT)
#define	PKTCLRSKIPCT(osh, skb)	(((struct sk_buff*)(skb))->mac_len &= (~SKIPCT))
#define	PKTSKIPCT(osh, skb)	(((struct sk_buff*)(skb))->mac_len & SKIPCT)
#define	PKTSETCHAINED(osh, skb)	(((struct sk_buff*)(skb))->mac_len |= CHAINED)
#define	PKTCLRCHAINED(osh, skb)	(((struct sk_buff*)(skb))->mac_len &= (~CHAINED))
#define	PKTISCHAINED(skb)	(((struct sk_buff*)(skb))->mac_len & CHAINED)
#else 
#define	SKIPCT	(1 << 2)
#define	CHAINED	(1 << 3)
#define	PKTSETSKIPCT(osh, skb)	(((struct sk_buff*)(skb))->__unused |= SKIPCT)
#define	PKTCLRSKIPCT(osh, skb)	(((struct sk_buff*)(skb))->__unused &= (~SKIPCT))
#define	PKTSKIPCT(osh, skb)	(((struct sk_buff*)(skb))->__unused & SKIPCT)
#define	PKTSETCHAINED(osh, skb)	(((struct sk_buff*)(skb))->__unused |= CHAINED)
#define	PKTCLRCHAINED(osh, skb)	(((struct sk_buff*)(skb))->__unused &= (~CHAINED))
#define	PKTISCHAINED(skb)	(((struct sk_buff*)(skb))->__unused & CHAINED)
#endif 
#else 
#define	PKTSETSKIPCT(osh, skb)
#define	PKTCLRSKIPCT(osh, skb)
#define	PKTSKIPCT(osh, skb)
#endif 

extern void osl_pktfree(osl_t *osh, void *skb, bool send);
extern void *osl_pktget_static(osl_t *osh, uint len);
extern void osl_pktfree_static(osl_t *osh, void *skb, bool send);

extern void *osl_pkt_frmnative(osl_t *osh, void *skb);
extern void *osl_pktget(osl_t *osh, uint len);
extern void *osl_pktdup(osl_t *osh, void *skb);
extern struct sk_buff *osl_pkt_tonative(osl_t *osh, void *pkt);
#define PKTFRMNATIVE(osh, skb)	osl_pkt_frmnative(((osl_t *)osh), (struct sk_buff*)(skb))
#define PKTTONATIVE(osh, pkt)		osl_pkt_tonative((osl_t *)(osh), (pkt))

#define	PKTLINK(skb)			(((struct sk_buff*)(skb))->prev)
#define	PKTSETLINK(skb, x)		(((struct sk_buff*)(skb))->prev = (struct sk_buff*)(x))
#define	PKTPRIO(skb)			(((struct sk_buff*)(skb))->priority)
#define	PKTSETPRIO(skb, x)		(((struct sk_buff*)(skb))->priority = (x))
#define PKTSUMNEEDED(skb)		(((struct sk_buff*)(skb))->ip_summed == CHECKSUM_HW)
#define PKTSETSUMGOOD(skb, x)		(((struct sk_buff*)(skb))->ip_summed = \
						((x) ? CHECKSUM_UNNECESSARY : CHECKSUM_NONE))

#define PKTSHARED(skb)                  (((struct sk_buff*)(skb))->cloned)

#define PKTALLOCED(osh)		osl_pktalloced(osh)
extern uint osl_pktalloced(osl_t *osh);

#ifdef CTFMAP
#include <ctf/hndctf.h>
#define	CTFMAPSZ	320
#define	DMA_MAP(osh, va, size, direction, p, dmah) \
({ \
	typeof(size) sz = (size); \
	if (PKTISCTF((osh), (p))) { \
		sz = CTFMAPSZ; \
		CTFMAPPTR((osh), (p)) = (void *)(((uint8 *)(va)) + CTFMAPSZ); \
	} \
	osl_dma_map((osh), (va), sz, (direction)); \
})
#if defined(__mips__)
#define	_DMA_MAP(osh, va, size, direction, p, dmah) \
	dma_cache_inv((uint)(va), (size))
#elif defined(__ARM_ARCH_7A__)
#include <asm/cacheflush.h>
#define	_DMA_MAP(osh, va, size, direction, p, dmah) \
	dma_inv_range((va), (void *)(((uint8 *)(va)) + (size)))
#else
#define	_DMA_MAP(osh, va, size, direction, p, dmah)
#endif

#else 
#define	DMA_MAP(osh, va, size, direction, p, dmah) \
	osl_dma_map((osh), (va), (size), (direction))
#endif 

#ifdef PKTC

struct chain_node {
	struct sk_buff	*link;
	unsigned int	flags:3, pkts:9, bytes:20;
};

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 14)
#define CHAIN_NODE(skb)		((struct chain_node*)&(((struct sk_buff*)skb)->tstamp))
#else
#define CHAIN_NODE(skb)		((struct chain_node*)&(((struct sk_buff*)skb)->stamp))
#endif

#define	PKTCSETATTR(s, f, p, b)	({CHAIN_NODE(s)->flags = (f); CHAIN_NODE(s)->pkts = (p); \
	                         CHAIN_NODE(s)->bytes = (b);})
#define	PKTCCLRATTR(s)		({CHAIN_NODE(s)->flags = CHAIN_NODE(s)->pkts = \
	                         CHAIN_NODE(s)->bytes = 0;})
#define	PKTCGETATTR(s)		(CHAIN_NODE(s)->flags << 29 | CHAIN_NODE(s)->pkts << 20 | \
	                         CHAIN_NODE(s)->bytes)
#define	PKTCCNT(skb)		(CHAIN_NODE(skb)->pkts)
#define	PKTCLEN(skb)		(CHAIN_NODE(skb)->bytes)
#define	PKTCFLAGS(skb)		(CHAIN_NODE(skb)->flags)
#define	PKTCSETCNT(skb, c)	(CHAIN_NODE(skb)->pkts = (c))
#define	PKTCINCRCNT(skb)	(CHAIN_NODE(skb)->pkts++)
#define	PKTCADDCNT(skb, c)	(CHAIN_NODE(skb)->pkts += (c))
#define	PKTCSETLEN(skb, l)	(CHAIN_NODE(skb)->bytes = (l))
#define	PKTCADDLEN(skb, l)	(CHAIN_NODE(skb)->bytes += (l))
#define	PKTCSETFLAG(skb, fb)	(CHAIN_NODE(skb)->flags |= (fb))
#define	PKTCCLRFLAG(skb, fb)	(CHAIN_NODE(skb)->flags &= ~(fb))
#define	PKTCLINK(skb)		(CHAIN_NODE(skb)->link)
#define	PKTSETCLINK(skb, x)	(CHAIN_NODE(skb)->link = (struct sk_buff*)(x))
#define FOREACH_CHAINED_PKT(skb, nskb) \
	for (; (skb) != NULL; (skb) = (nskb)) \
		if ((nskb) = (PKTISCHAINED(skb) ? PKTCLINK(skb) : NULL), \
		    PKTSETCLINK((skb), NULL), 1)
#define	PKTCFREE(osh, skb, send) \
do { \
	void *nskb; \
	ASSERT((skb) != NULL); \
	FOREACH_CHAINED_PKT((skb), nskb) { \
		PKTCLRCHAINED((osh), (skb)); \
		PKTCCLRATTR((skb)); \
		PKTFREE((osh), (skb), (send)); \
	} \
} while (0)
#define PKTCENQTAIL(h, t, p) \
do { \
	if ((t) == NULL) { \
		(h) = (t) = (p); \
	} else { \
		PKTSETCLINK((t), (p)); \
		(t) = (p); \
	} \
} while (0)
#endif 

#else 



	#define ASSERT(exp)	do {} while (0)


#define MALLOC(o, l) malloc(l)
#define MFREE(o, p, l) free(p)
#include <stdlib.h>


#include <string.h>


#include <stdio.h>


extern void bcopy(const void *src, void *dst, size_t len);
extern int bcmp(const void *b1, const void *b2, size_t len);
extern void bzero(void *b, size_t len);
#endif 

#endif	
