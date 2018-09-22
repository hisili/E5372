#ifndef __ASM_ARCH_MEMORY_H
#define __ASM_ARCH_MEMORY_H

#include <generated/FeatureConfig.h>  /*syb*/

/* physical offset of RAM */
#if defined (BOARD_FPGA) && defined (VERSION_V3R2)
    #define PHYS_OFFSET        UL(0xc4400000)
#elif (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
	#define PHYS_OFFSET        UL(0xc3900000)
#elif defined (BOARD_SFT) && defined (VERSION_V3R2)
    #define PHYS_OFFSET        UL(0x34400000)
#elif defined (BOARD_SFT) && defined (VERSION_V7R1)
    #define PHYS_OFFSET        UL(0x34400000)
#elif defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2)
    /*V3R2 CS f00164371*/
    #if defined(CHIP_BB_6756CS)
	    #if (FEATURE_64M == FEATURE_ON )
            #define PHYS_OFFSET        UL(0x31C00000)  /*syb*/
        #else
            #define PHYS_OFFSET        UL(0x34400000)
        #endif
	#else	
        #if (FEATURE_64M == FEATURE_ON )
            #define PHYS_OFFSET        UL(0x31A00000)  /*syb*/
        #else
            #define PHYS_OFFSET        UL(0x34400000)
        #endif
    #endif
#elif (defined(BOARD_ASIC) || defined (BOARD_ASIC_BIGPACK) || defined (BOARD_SFT))
    #if defined(CHIP_BB_6920ES) 
        #define PHYS_OFFSET        UL(0x33900000)
    #elif defined (CHIP_BB_6920CS) 
	    #if (FEATURE_256M == FEATURE_ON )
	    #define PHYS_OFFSET        UL(0x35E00000)
	    #else
          #if(FEATURE_GUTL == FEATURE_ON ) /* 不影响现有产品内存分配，后续可以合并，by wuzechun */
			#if (FEATURE_HUTAF_HLT_COV == FEATURE_ON )
				#define PHYS_OFFSET        UL(0x34A00000)
			#else
				#define PHYS_OFFSET        UL(0x33F00000)
			#endif
          #else
          #define PHYS_OFFSET        UL(0x33900000)
          #endif
	    #endif
	#endif
#else
/*#error "Not implement the feature board, HAHA!!!"*/
#endif



#endif


