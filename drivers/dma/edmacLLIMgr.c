/*
#ifndef WIN32   
#include "osal.h"
#else
#include "edmacWin32.h"
#endif
*/
#include <linux/dma-mapping.h>


#include "mach/edmacWin32.h"
#include "mach/edmacPri.h"
#include "mach/edmacDrv.h"
#include "mach/edmaLLIMgr.h"
#include "mach/edmacIP.h"
#include "edmacConfig.h"



LOCAL EDMAC_FREE_NODE_HEADER_STRU  s_pstFreeNodeHeader      = {0, 0}; 

/*
LOCAL UINT32                       s_ulMaxUsedNodeCount     = 0;
*/

void edma_lli_node_list_init( void )
{
    EDMAC_TRANSFER_CONFIG_STRU * pstNode = NULL;
    UINT32                       iTemp   = 0UL;
	UINT32                       edma_addr = 0;

    if( EDMAC_INIT_FLAG == s_pstFreeNodeHeader.ulInitFlag ) /*why EDMAC_INIT_FLAG  0x414D4445*/
    {
        /* EDMAC_ASSERT( 0 ); Not support re-entry, because can't free the node that has been allocated */
		printk("LII list has been initialized!"); 
		return ;
    }

    /* 
        + 32 : just for 32-byte alignment adjust.
        It is no useful now, because the DmaMalloc is 4K- alignmeng now.
    */
    pstNode = (EDMAC_TRANSFER_CONFIG_STRU *)dma_alloc_coherent(NULL,(EDMAC_LLI_NODE_NUM * sizeof(EDMAC_TRANSFER_CONFIG_STRU)),
                                                               &edma_addr, GFP_KERNEL);
                                            /*+ EDMAC_LLI_ALIGNMENT), &edma_addr, GFP_KERNEL);*/
    if( NULL != pstNode )
    {
        /* Check the header is the 32-byte align ? */
        iTemp = (UINT32)pstNode;
        if( iTemp & EDMAC_LLI_ALIGNMENT )/*z00178766 ,2011715,This should be a error*/
        {
            /* Now the DmaAlloc is 4k align, if not, delete the assert. */
            EDMAC_DEBUG( 0 ); 

            iTemp   = (iTemp & ~(EDMAC_LLI_ALIGNMENT)) + EDMAC_LLI_ALIGNMENT;  /* 32-byte alignment */
            pstNode = (EDMAC_TRANSFER_CONFIG_STRU *)iTemp;
        }
        
        s_pstFreeNodeHeader.ulFirstFreeNode    = 0UL;
        for( iTemp = 0; iTemp < EDMAC_LLI_NODE_NUM; iTemp++ )
        {
            /* Insert to the free list header always.   */
            pstNode->ulLLI                         = s_pstFreeNodeHeader.ulFirstFreeNode;
            s_pstFreeNodeHeader.ulFirstFreeNode    = EDMAC_MAKE_LLI_ADDR( pstNode ); 

            pstNode++;
        }
    }
    else
    {
        EDMAC_ASSERT( 0 );
    }

    s_pstFreeNodeHeader.ulInitFlag = EDMAC_INIT_FLAG;
}

void edma_lli_node_free( EDMAC_TRANSFER_CONFIG_STRU * pstNode )
{
    /* 
        Add the node to the free list.
        The fields of the structure will be reset after alloc, NEEDN'T clear it now
    */
    pstNode->ulLLI                         = s_pstFreeNodeHeader.ulFirstFreeNode;
    s_pstFreeNodeHeader.ulFirstFreeNode    = EDMAC_MAKE_LLI_ADDR( pstNode );
}

EDMAC_TRANSFER_CONFIG_STRU * edma_lli_node_malloc( void )
{
    EDMAC_TRANSFER_CONFIG_STRU  * pstNode;

    pstNode = (EDMAC_TRANSFER_CONFIG_STRU *)s_pstFreeNodeHeader.ulFirstFreeNode;
    if( NULL != pstNode )
    {    
        s_pstFreeNodeHeader.ulFirstFreeNode = pstNode->ulLLI;  /* Remove from the free list */
        return pstNode;
    }
    else
    {
        /* 
            Why? someone not free the resource ? or Need increase the LLI count? 
            If the count is not enough, maybe we can malloc a second buffer and added to the free list
        */

        EDMAC_ASSERT( 0 );
        return NULL; /* Wouldn't reach here */
    }
}


void edma_simple_lli_node_fill( EDMAC_TRANSFER_CONFIG_STRU * pstNode, const DMA_SIMPLE_LLI_STRU * pstContent )
{
    if( (NULL != pstNode) && (NULL != pstContent) )
    {
        pstNode->ulSrcAddr  = pstContent->ulSourAddr;
        pstNode->ulDesAddr  = pstContent->ulDestAddr;
        pstNode->ulCNT0     = pstContent->ulLength;
        pstNode->ulConfig   = EDMAC_MAKE_CONFIG( pstContent->ulConfig, EDMAC_TRANSFER_CONFIG_EXT_ADD_MODE_A_SYNC ); /* Ext Config is 0 and A-sync mode */

        pstNode->ulBINDX    = 0;
        pstNode->ulCINDX    = 0;
        pstNode->ulCNT1     = 0;
    }
    else
    {
        EDMAC_ASSERT( 0 );
    }
}


UINT32 edma_free_count_get( void )
{
    EDMAC_TRANSFER_CONFIG_STRU  * pstNode     = NULL;
    UINT32                        ulFreeCount = 0;

    pstNode = (EDMAC_TRANSFER_CONFIG_STRU *)s_pstFreeNodeHeader.ulFirstFreeNode;
    while( NULL != pstNode )
    {    
        ulFreeCount++;
        pstNode = (EDMAC_TRANSFER_CONFIG_STRU *)(pstNode->ulLLI & 0xFFFFFFE0);
    }

    return ulFreeCount;
}




