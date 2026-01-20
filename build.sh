#!/bin/bash
# Script to build image for qemu.
# Author: Siddhant Jajoo.

git submodule init
git submodule sync
git submodule update

# local.conf won't exist until this step on first execution
source poky/oe-init-build-env

CONFLINE="MACHINE = \"qemuarm64\""

cat conf/local.conf | grep "${CONFLINE}" > /dev/null
local_conf_info=$?

if [ $local_conf_info -ne 0 ];then
	echo "Append ${CONFLINE} in the local.conf file"
	echo ${CONFLINE} >> conf/local.conf
	
else
	echo "${CONFLINE} already exists in the local.conf file"
fi


bitbake-layers show-layers | grep "meta-sensor-node" > /dev/null
layer_info=$?

if [ $layer_info -ne 0 ];then
	echo "Adding meta-sensor-node layer"
	bitbake-layers add-layer ../meta-sensor-node
else
	echo "meta-sensor-node layer already exists"
fi

set -e
bitbake core-image-aesd
