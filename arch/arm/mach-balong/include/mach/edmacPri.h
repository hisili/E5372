#ifndef EDMAC_PRI_H
#define EDMAC_PRI_H

#ifndef LOCAL
/* #define LOCAL static */
#define LOCAL 
#endif


#define EDMAC_INIT_FLAG    0x414D4445  /* EDMA */

#define EDMAC_ASSERT( _exp )    while( !(_exp) ){ ; }

#define EDMAC_DEBUG( _exp )    while( !(_exp) ){ ; }


void edmac_priority_set( UINT32 ulPriority );
void edmac_product_config( void );
UINT32 edma_convert_logic_channel( UINT32 ulLogicChannel );

#define EDMAC_LOCK()            OSAL_IntLock()
#define EDMAC_UNLOCK( _p )      OSAL_IntUnlock( _p )

#ifndef OSAL_INT_EDMAC
#define OSAL_INT_EDMAC   0 /* Just for test, delete after merge with chenyingguo */
#endif

#endif /* EDMAC_PRI_H */

