export ARCH=arm
export CROSS_COMPILE=arm-linux-
export PATH=$PATH:/opt/4.5.1/bin

export DHDARCH=arm
export VERBOSE=1
export BOARD_TYPE=BOARD_ASIC
export VERSION_TYPE=CHIP_BB_6920ES
export PRODUCT_CFG_BUILD_TYPE=ALLY 

#改成实际的kernel路径
export LINUXDIR=$1

make dhd-cdc-usb-hsic-gpl