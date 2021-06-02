#! /bin/bash

# Clean up the sourcce files after libs are generated
# example:
# generate_beken_lib.sh bk7231u [SDK_path]

source ./tools/scripts/build_include.sh

if [ "$2" == "" ]; then
	BEKEN_SDK_DIR=./beken378
else
	BEKEN_SDK_DIR=$2
fi

echo "making ${1} libs..."

SYS_CFG_FILE=${BEKEN_SDK_DIR}/app/config/sys_config_${1}.h
if [ ! -f ${SYS_CFG_FILE} ]; then
	echo "${SYS_CFG_FILE}: no such file!"
	exit 1
fi
echo "backup ${SYS_CFG_FILE}"
cp -f ${SYS_CFG_FILE} ${SYS_CFG_FILE}.bak

case $1 in
	bk7231n)
		modify_config ${SYS_CFG_FILE} CFG_SUPPORT_BLE 1
		modify_config ${SYS_CFG_FILE} CFG_WIFI_SENSOR 1
		make_target="beken_ip beken_ble beken_vad beken_sensor beken_cal beken_rf_use beken_rf_test beken_uart_debug beken_supplicant"
		;;
	bk7231u)
		modify_config ${SYS_CFG_FILE} CFG_SUPPORT_BLE 1
		modify_config ${SYS_CFG_FILE} CFG_WIFI_SENSOR 1
		modify_config ${SYS_CFG_FILE} CFG_USB 1
		make_target="beken_ip beken_ble beken_usb beken_vad beken_sensor beken_cal beken_rf_use beken_rf_test beken_uart_debug beken_supplicant"
		;;
	bk7251)
		modify_config ${SYS_CFG_FILE} CFG_SUPPORT_BLE 1
		modify_config ${SYS_CFG_FILE} CFG_WIFI_SENSOR 1
		modify_config ${SYS_CFG_FILE} CFG_USB 1
		make_target="beken_ip beken_ble beken_usb beken_vad beken_sensor beken_cal beken_rf_use beken_rf_test beken_uart_debug beken_supplicant"
		;;
	bk7271)
		modify_config ${SYS_CFG_FILE} CFG_SUPPORT_BLE 0
		modify_config ${SYS_CFG_FILE} CFG_WIFI_SENSOR 1
		modify_config ${SYS_CFG_FILE} CFG_USB 1
		make_target="beken_ip beken_usb beken_vad beken_sensor beken_cal beken_rf_use beken_rf_test beken_uart_debug beken_supplicant"
		;;
	*)
		echo "unsupported platform"
		exit 1
		;;
esac

for sub in ${make_target}
do
	echo "making lib ${sub}..."
	scons --beken=$1 --buildlib=$sub -j8
	if [ $? != 0 ]; then
		echo "make lib $sub fail"
		exit 1
	fi
	rm -f lib${sub}_gcc.a
done
rm -rf build
rm -f config/sys_config.h
rm -f link.lds

echo "restore ${SYS_CFG_FILE}"
rm -f ${SYS_CFG_FILE}
mv -f ${SYS_CFG_FILE}.bak ${SYS_CFG_FILE}
