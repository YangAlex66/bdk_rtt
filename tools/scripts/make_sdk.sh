#! /bin/bash
# generate SDK after a new git clone and update the SDK/OSK revisions

# generate libs

source ./tools/scripts/build_include.sh

PLATFORMS="$SUPPORTED_BUILD_PLATFORMS"
CLEAN_GIT=0

if [ "$1" != "" ]; then
	if [ "$1" == "-c" ]; then
		CLEAN_GIT=1
	else
		PLATFORMS="$1"
	fi
	if [ "$2" != "" ]; then
		if [ "$2" == "-c" ]; then
			CLEAN_GIT=1
		else
			echo "The second param must be '-c'"
			exit 1
		fi
	fi
fi

for PLATFORM in ${PLATFORMS}
do
	validate_platform $PLATFORM
	if [ $? != 0 ]; then
		exit 1
	fi
done

for PLATFORM in ${PLATFORMS}
do
	./tools/scripts/generate_beken_libs.sh $PLATFORM
	if [ $? != 0 ]; then
		echo "make $PLATFORM libs fail"
		exit 1
	fi
done

# clean lib source files
./tools/scripts/clean_src_files.sh
if [ $? != 0 ]; then
	echo "clean src files fail"
	exit 1
fi

# remove output and unused files
echo "clean unused files ..."
rm -rf config/sys_config.h
rm -f rtconfig.h
rm -rf build
rm -rf bugzilla
rm -f README.md
rm -f .platform
rm -rf ./samples/story_xyos
rm -rf ./samples/wantong
rm -rf .sconsign.dblite
find ./ -name "*.pyc" | xargs rm -rf
rm -rf ./beken378/bugzilla
rm -f ./beken378/README.md

# remove git files
if [ $CLEAN_GIT -eq 1 ]; then
	find ./ -name *.git* | xargs rm -rf
fi

# add toolchain directory
if [ "${RTT_EXEC_PATH}" != "" ]; then
	if [ ! -d toolchain ]; then
		TOOLCHAIN_DIR=$(dirname "$RTT_EXEC_PATH")
		TOOLCHAIN_BASE=$(basename "$TOOLCHAIN_DIR")
		echo "TOOLCHAIN_DIR: ${TOOLCHAIN_DIR}"
		echo "packaging toolchain ..."
		mkdir toolchain
		cd toolchain
		cp -rf ${TOOLCHAIN_DIR} ${TOOLCHAIN_BASE}
		tar -cf ${TOOLCHAIN_BASE}.tar.bz2 ${TOOLCHAIN_BASE}
		rm -rf ${TOOLCHAIN_BASE}
		cd ..
	fi
fi

echo "make SDK done."
