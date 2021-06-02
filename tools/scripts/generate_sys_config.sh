#!/bin/bash
# generate_sys_config alias_project_name targe_sys_config
# example: 
# generate_sys_config bk7231u config/sys_config.h

ARM_GCC_TOOLCHAIN=${RTT_EXEC_PATH}

if [ "$2" == "" ]; then
	old_sys_config=config/sys_config.h
	old_rtconfig=rtconfig.h
fi

if [ ! -d "config" ]; then
	mkdir config
fi

case $1 in
	bk7231u)
		new_sys_config=beken378/app/config/sys_config_bk7231u.h
		new_linkscript=config/bk72xx.lds
		new_rt_config=config/rtconfig/rtconfig_bk7231u.h
		;;
	bk7231n)
		new_sys_config=beken378/app/config/sys_config_bk7231n.h
		new_linkscript=config/bk7231n_boot.lds
		new_rt_config=config/rtconfig/rtconfig_bk7231n.h
		;;
	bk7251)
		new_sys_config=beken378/app/config/sys_config_bk7251.h
		new_linkscript=config/bk72xx.lds
		new_rt_config=config/rtconfig/rtconfig_bk7251.h
		;;
	bk7231)
		new_sys_config=beken378/app/config/sys_config_bk7231.h
		new_linkscript=config/bk72xx.lds
		new_rt_config=config/rtconfig/rtconfig_bk7231.h
		;;
	bk7271)
		new_sys_config=beken378/app/config/sys_config_bk7271.h
		new_linkscript=config/bk72xx.lds
		new_rt_config=config/rtconfig/rtconfig_bk7271.h
		;;
	*)
		new_sys_config=beken378/app/config/$1.h
		new_linkscript=config/bk72xx.lds
		new_rt_config=config/rtconfig/rtconfig_bk7231u.h
esac

if [ -f $new_sys_config ]; then
	new_hash=`md5sum $new_sys_config | cut -d' ' -f 1`
else
	echo "$new_sys_config not exist!"
	exit 1
fi

if [ -f $old_sys_config ]; then
	old_hash=`md5sum $old_sys_config | cut -d' ' -f 1`
else
	old_hash=""
fi

#echo "hash($new_sys_config)=$new_hash"
#echo "hash($old_sys_config)=$old_hash"

if [ "$new_hash" != "$old_hash" ]; then
	cp -f $new_sys_config $old_sys_config
	cp -f $new_rt_config $old_rtconfig
#	echo "update $old_sys_config with $new_sys_config"
fi

cp -f $new_linkscript link.lds

rm -f .platform
echo $1 > .platform

# scons is used for rtt building, so skip .config generation
exit 0

echo "  ${GREEN}GEN  .config${NC}"
rm -f .config
echo '#include "config/sys_config.h"' > config.c
sed -n '/^#define/p' config/sys_config.h | awk '{print $2}' | sort -d | uniq | awk '{print "valueOf_"$1"="$1}' >> config.c
echo "# Autogenerated by Makefile, DON'T EDIT" > .config
${ARM_GCC_TOOLCHAIN}/arm-none-eabi-gcc -E config.c | grep '^valueOf_' | sed 's/valueOf_//' >> .config
sed -i '/_SYS_CONFIG_H_/d' .config
rm -f config.c
