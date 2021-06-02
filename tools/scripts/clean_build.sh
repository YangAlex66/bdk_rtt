#! /bin/bash
# Usage: clean_build.sh [platform]

source ./tools/scripts/build_include.sh

echo "cleaning build ..."

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

	scons --beken=$PLATFORM -c
	if [ $? != 0 ]; then
		echo "clean $PLATFORM build error!"
		exit 1
	fi
done

rm -rf out

echo "clean build done."
