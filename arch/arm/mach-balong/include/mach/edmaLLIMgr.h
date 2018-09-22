#ifndef DMA_LLI_MGR_H
#define DMA_LLI_MGR_H

/*
#ifdef WIN32
#include "edmacWin32.h"
#else
#include "osal.h"
#endif
*/
#include "edmacWin32.h"


#include "edmacIP.h"
#include "edmacDrv.h"

typedef struct
{
    UINT32  ulInitFlag;
    UINT32  ulFirstFreeNode; 
}EDMAC_FREE_NODE_HEADER_STRU;


void edma_lli_node_list_init( void );

void edma_lli_node_free( EDMAC_TRANSFER_CONFIG_STRU * pstNode );
void edma_lli_free( void * pstFirstNode );

EDMAC_TRANSFER_CONFIG_STRU * edma_lli_node_malloc( void );

void edma_simple_lli_node_fill( EDMAC_TRANSFER_CONFIG_STRU * pstNode, const DMA_SIMPLE_LLI_STRU * pstContent );

UINT32 edma_free_count_get( void );


#endif /* DMA_LLI_MGR_H */

