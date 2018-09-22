#ifndef _EDMAC_WIN32_H
#define _EDMAC_WIN32_H

/*#define SC_DMACSEL                  0x20000000

#define SUPPROT_LOGICCHANNEL_CN     EDMAC_MAX_CHANNEL
#define SUPPROT_CPUINT_IN           0x00000001*/
#define EDMAC_REG_OFFSET            0x0ba0

#define UINT32  unsigned int
#define UINT8   unsigned char

#ifndef NULL
#define NULL    (void *)(0)
#endif

/*y00186965*/
/*void *OSAL_CacheDmaMalloc( UINT32 ulSize );*/

#define OSAL_DATA_CACHE  0
#define OSAL_CacheFlush( _p1, _p2, _p3 );


#define DMA_SUCCESS 0
#define DMA_ERROR   1
#define DMA_ERROR_BASE     100
#define DMA_CHANNEL_INVALID (DMA_ERROR_BASE+1)
#define DMA_TRXFERSIZE_INVALID (DMA_ERROR_BASE+2)
#define DMA_SOURCE_ADDRESS_INVALID (DMA_ERROR_BASE+3)
#define DMA_DESTINATION_ADDRESS_INVALID (DMA_ERROR_BASE+4)
#define DMA_MEMORY_ADDRESS_INVALID (DMA_ERROR_BASE+5)
#define DMA_PERIPHERAL_ID_INVALID (DMA_ERROR_BASE+6)
#define DMA_DIRECTION_ERROR (DMA_ERROR_BASE+7)
#define DMA_TRXFER_ERROR (DMA_ERROR_BASE+8)
#define DMA_LLIHEAD_ERROR (DMA_ERROR_BASE+9)
#define DMA_SWIDTH_ERROR  (DMA_ERROR_BASE+0xa)
#define DMA_LLI_ADDRESS_INVALID  (DMA_ERROR_BASE+0xb)
#define DMA_TRANS_CONTROL_INVALID  (DMA_ERROR_BASE+0xc)
#define DMA_MEMORY_ALLOCATE_ERROR  (DMA_ERROR_BASE+0xd)
#define DMA_NOT_FINISHED   (DMA_ERROR_BASE+0xe)


#define DRV_LOG1( _p1, _p2, _p3, _p4, _p5 )
#define DRV_LOG(_p1, _p2, _p3, _p4 )

/* 
#define CLOCK_GATING_EDMAC  0

#define clock_gating_enable( _v ) 
#define clock_gating_disable( _v );
*/

#define OSAL_INT_EDMAC 0
void OSAL_IntConnect( UINT32, void *, int p );
void OSAL_IntEnable( UINT32 );

#define OSAL_IntLock()          0
#define OSAL_IntUnlock( _p )


#endif /* _EDMAC_WIN32_H */