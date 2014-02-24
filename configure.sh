#!/bin/sh

#to use Open Watcom instead of MinGW in the RosBE, simply define
#WATCOM to path of the root of your watcom install

if [ -d "$WATCOM" ]; then
	echo Watcom detected, setting NT / Windows cross compilation options
	PATH=$WATCOM/binnt:$WATCOM/binw:$PATH
	INCLUDE=$WATCOM/h:$WATCOM/h/nt:$WATCOM/h/nt/directx:$WATCOM/h/nt/ddk:$INCLUDE
	EDPATH=$WATCOM/eddat
	WIPFC=$WATCOM/wipfc
		
elif [ "x$ROS_ARCH" = "x" ]; then
	echo Could not detect RosBE.
	exit 1
fi

if [ -d "$WATCOM" ]; then
	BUILD_ENVIRONMENT=Watcom
	#for now we default to 32-bit
	ARCH=i386
else
	BUILD_ENVIRONMENT=MinGW
	ARCH=$ROS_ARCH
fi

REACTOS_SOURCE_DIR=$(cd `dirname $0` && pwd)
REACTOS_OUTPUT_PATH=output-$BUILD_ENVIRONMENT-$ARCH

usage() {
	echo Invalid parameter given.
	exit 1
}

if [ -d "$WATCOM" ]; then
# annoying: CMake on Linux can not make wmake files, falling back to gmake
#	CMAKE_GENERATOR="Watcom WMake"
	CMAKE_GENERATOR="Unix Makefiles"

else
	CMAKE_GENERATOR="Ninja"
	while [ $# -gt 0 ]; do
		case $1 in
			-D)
				shift
				if echo "x$1" | grep 'x?*=*' > /dev/null; then
					ROS_CMAKEOPTS=$ROS_CMAKEOPTS" -D $1"
				else
					usage
				fi
			;;

			-D?*=*|-D?*)
				ROS_CMAKEOPTS=$ROS_CMAKEOPTS" $1"
			;;
			makefiles|Makefiles)
				CMAKE_GENERATOR="Unix Makefiles"
			;;
			*)
				usage
		esac

		shift
	done
fi

if [ "$REACTOS_SOURCE_DIR" = "$PWD" ]; then
	echo Creating directories in $REACTOS_OUTPUT_PATH
	mkdir -p "$REACTOS_OUTPUT_PATH"
	cd "$REACTOS_OUTPUT_PATH"
fi

mkdir -p host-tools reactos

echo Preparing host tools...
cd host-tools
rm -f CMakeCache.txt

REACTOS_BUILD_TOOLS_DIR="$PWD"
cmake -G "$CMAKE_GENERATOR" -DARCH=$ARCH $ROS_CMAKEOPTS "$REACTOS_SOURCE_DIR"

echo Preparing reactos...
cd ../reactos
rm -f CMakeCache.txt

cmake -G "$CMAKE_GENERATOR" -DENABLE_CCACHE=0 -DCMAKE_TOOLCHAIN_FILE=toolchain-gcc.cmake -DARCH=$ARCH -DREACTOS_BUILD_TOOLS_DIR="$REACTOS_BUILD_TOOLS_DIR" $ROS_CMAKEOPTS "$REACTOS_SOURCE_DIR"

echo Configure script complete! Enter directories and execute appropriate build commands \(ex: ninja, make, makex, etc...\).
