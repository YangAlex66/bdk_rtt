#! /bin/bash
# Usage: clean_libs.sh [platform]

source ./tools/scripts/build_include.sh

echo "cleaning libs ..."

PLATFORMS=$SUPPORTED_BUILD_PLATFORMS

if [ "$1" != "" ]; then
	PLATFORMS="$1"
fi

for PLATFORM in ${PLATFORMS}
do
	validate_platform $PLATFORM
	if [ $? != 0 ]; then
		exit 1
	fi

	scons --beken=$PLATFORM --cleanlibs
	if [ $? != 0 ]; then
		echo "clean $PLATFORM libs error!"
		exit 1
	fi
done

rm -rf ./beken378/lib/*

echo "clean libs done."
