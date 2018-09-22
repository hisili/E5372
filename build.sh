#!/bin/bash
#source /etc/profile
export INCLUDE_FLAG="-I${pwd}/include/linux"
export DRV_INCLUDE_FLAG=-I${pwd}/arch/arm/mach-balong/include 
export ASM_INCLUDE_FLAG="-I${pwd}/arch/arm/include/asm"
export PATH=/opt/4.5.1/bin/:$PATH

export ARCH=arm
export SUBARCH=arm
export CROSS_COMPILE=/opt/4.5.1/bin/arm-linux-

FEATURE_WIFI=FEATURE_ON
export FEATURE_WIFI

make hisi_balong_hi6920cs_e5776s_defconfig
make  -C ./ -j 64 BOARD_TYPE=BOARD_ASIC VERSION_TYPE=CHIP_BB_6920CS PRODUCT_CFG_BUILD_TYPE= 2>log.txt
if [ 0 -ne $? ]
then
	echo "############################# "
	echo "Make KERNEL Failed!"
	echo "#############################"
	exit 1
else
	echo "############################# "
	echo "Make KERNEL Success!"
	echo "#############################"
fi 



