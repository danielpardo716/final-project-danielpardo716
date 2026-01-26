# Shell script to start Raspberry Pi Zero W (64-bit) on QEMU
#!/bin/sh

KERNEL="buildroot/output/images/sdcard.img"
DTB="buildroot/output/images/bcm2708-rpi-zero-w.dtb" 
ROOTFS="buildroot/output/images/rootfs.ext2"

if [ ! -e $KERNEL ]
then
	echo "$KERNEL does not exist"
	exit 1
fi

if [ ! -e $DTB ]
then
	echo "$DTB does not exist"
	exit 1
fi

if [ ! -e $ROOTFS ]
then
	echo "$ROOTFS does not exist"
	exit 1
fi


qemu-system-aarch64 \
	-M raspi0 \
	-kernel $KERNEL \
	-dtb $DTB \
	-m 512 -nographic \
	-append "rw console=ttyAMA0,115200 root=/dev/mmcblk0 fsck.repair=yes rootwait" \
	-device sd-card,drive=mycard -drive if=none,file=$ROOTFS,format=raw,id=mycard

