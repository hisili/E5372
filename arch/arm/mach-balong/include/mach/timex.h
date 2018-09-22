#if defined (BOARD_SFT) && defined (VERSION_V3R2)
#include <mach/balong_v3r2sft_v100r001.h>
#elif defined (BOARD_FPGA) && defined (VERSION_V3R2)
#include <mach/balong_p500bbit_v100r001.h>
#elif (defined (BOARD_FPGA_P500) && defined(CHIP_BB_6920ES))
#include <mach/balong_p500fpga.h>
#elif (defined (BOARD_FPGA_P500) && defined (CHIP_BB_6920CS))
#include <mach/balong_hi6920csp500.h>
#elif defined (BOARD_SFT) && defined (VERSION_V7R1)
#include <mach/balong_v7r1sft.h>
#elif defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2)
/*++pandong cs*/
    #if defined(CHIP_BB_6756CS)
        #include <mach/balong_v3r2cs.h>
    #else
        #include <mach/balong_v3r2asic.h>
    #endif
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
#include <mach/balong_v7r1asic.h>
#elif (defined (BOARD_SFT) && defined (CHIP_BB_6920CS))
#include <mach/balong_hi6920cs_sft.h>
#else

#endif
#define CLOCK_TICK_RATE        ((PBXA9_TIMERS_CLK)  / 16)

