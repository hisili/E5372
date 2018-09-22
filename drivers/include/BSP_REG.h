/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_REG.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_REG_H__
#define __BSP_REG_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* 寄存器操作宏定义 */
#define BSP_REG8(base, reg) (*(volatile BSP_U8 *)((BSP_U32)base + (reg)))
#define BSP_REG16(base, reg) (*(volatile BSP_U16 *)((BSP_U32)base + (reg)))

/* 将var中[pos, pos + bits-1]比特清零,  pos从0开始编号
   e.g BDOM_CLR_BITS(var, 4, 2) 表示将Bit5~4清零 */
#define BSP_REG_CLRBITS(base, reg, pos, bits) (BSP_REG(base, reg) &= ~((((BSP_U32)1 << (bits)) - 1) << (pos)))

/* 将var中[pos, pos + bits-1]比特设置为val,  pos从0开始编号
   e.g BDOM_SET_BITS(var, 4, 2, 2) 表示将Bit5~4设置为b'10 */
#define BSP_REG_SETBITS(base, reg, pos, bits, val) (BSP_REG(base, reg) = (BSP_REG(base, reg) & (~((((BSP_U32)1 << (bits)) - 1) << (pos)))) \
                                                                         | ((BSP_U32)((val) & (((BSP_U32)1 << (bits)) - 1)) << (pos)))

/* 获取var中[pos, pos + bits-1]比特值,  pos从0开始编号 */
#define BSP_REG_GETBITS(base, reg, pos, bits) ((BSP_REG(base, reg) >> (pos)) & (((BSP_U32)1 << (bits)) - 1))

#define BSP_REG_WRITEBITS(base, reg, data) \
            (BSP_REG(base, reg) |= (data))
#define BSP_REG8_READ(base, reg, result) \
    ((result) = BSP_REG8(base, reg))

#define BSP_REG16_READ(base, reg, result) \
    ((result) = BSP_REG16(base, reg))

#define BSP_REG_MODIFY(base, reg, clrmsk, setmsk) \
    (BSP_REG(base, reg) = ((BSP_REG(base, reg) & ~clrmsk) | setmsk))


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_REG_H__ */





