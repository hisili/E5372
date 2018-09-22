#include "BSP.h"

extern unsigned int spinLockAmpTake(unsigned int * pSpinLock);
extern VOID spinLockAmpGive(BSP_U32* handle);
extern unsigned int spinLockAmpTakeOneTime(unsigned int * pSpinLock);

