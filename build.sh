#! /bin/sh

# 命令回显
#set -v on

if [ ! -f "./configure" ];then
    ./autogen.sh 

#   以下命令再命令行输入（放在脚本内执行无效？？why？？反引号在脚本内无法执行）
#    sed -i '/for ac_func in getpagesize/i\\ncat >>condefs.h << _ACEOF\n#define HAVE_GETPAGESIZE 1\n_ACEOF\n\n' configure 
#    sed -i '/for ac_func in getpagesize/i\\ncat >>condefs.h << _ACEOF\n#define HAVE_MMAP 1\n_ACEOF\n\n' configure
    
#    sed -i 's/sys\/shm.h/linux\/shm.h/g' `grep -rl "sys/shm.h" --include=\*.{c,cpp,h} ./`
#    sed -i 's/sys\/sem.h/linux\/sem.h/g' `grep -rl "sys/sem.h" --include=\*.{c,cpp,h} ./`
#    sed -i 's/sys\/fcntl.h/linux\/fcntl.h/g' `grep -rl "sys/fcntl.h" --include=\*.{c,cpp,h} ./`
#    sed -i 's/sys\/termios.h/termios.h/g' `grep -rl "sys/termios.h" --include=\*.{c,cpp,h} ./`
#    sed -i 's/_LINUX_TYPES_H/_LINUX_MINIGUI_TYPES_H/g' `grep -rl "_LINUX_TYPES_H" --include=\*.{c,cpp,h} ./`
#    sed -i 's/union semun {/union __deprecated__semun {/g' `grep -rl "union semun {" --include=\*.{c,cpp,h} ./`

    
fi

OUTPUT=/home/suyc/V3S/minigui-git/output

CC=$ANDROID_BUILD_TOP/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-gcc \
AR=$ANDROID_BUILD_TOP/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-ar \
RANLIB=$ANDROID_BUILD_TOP/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-ranlib \
CFLAGS="-O3 -I$ANDROID_BUILD_TOP/hardware/libhardware_legacy/include -I$ANDROID_BUILD_TOP/hardware/libhardware_legacy/include/hardware_legacy -I$ANDROID_BUILD_TOP/bionic/libc/kernel/common/linux/can -I$ANDROID_BUILD_TOP/bionic/libm/include -I$ANDROID_BUILD_TOP/bionic/libc/include -I$ANDROID_BUILD_TOP/bionic/libc/arch-arm/include -I$ANDROID_BUILD_TOP/bionic/libstdc++/include -I$ANDROID_BUILD_TOP/bionic/libc/kernel/common -I$ANDROID_BUILD_TOP/bionic/libc/kernel/arch-arm -L$ANDROID_PRODUCT_OUT/system/lib -nostdlib -lm -lstdc++ -lc -ldl -Bdynamic -Wl,-dynamic-linker,/system/bin/linker -I$ANDROID_BUILD_TOP/external/tslib/src -I$ANDROID_BUILD_TOP/external/jpeg -I$ANDROID_BUILD_TOP/external/libpng -I$ANDROID_BUILD_TOP/external/zlib -I$OUTPUT/include" \
PKG_CONFIG=/usr/bin/pkg-config \
PKG_CONFIG_PATH=$OUTPUT/lib/pkgconfig \
./configure --host=arm-none-linux --target=arm-eabi \
--enable-static --enable-shared \
--prefix=$OUTPUT

#sed -i 's/\/\* #undef HAVE_GETPAGESIZE \*\//#define HAVE_GETPAGESIZE   1/g' mgconfig.h
#sed -i 's/\/\* #undef HAVE_MMAP \*\//#define HAVE_MMAP   1/g' mgconfig.h




#cd ../libminigui-3.0.12-linux
#make uninstall
#make && make install
#arm-linux-androideabi-strip output/lib/libminigui_ths-3.0.so.12.0.0 
#cp output/lib/libminigui_ths-3.0.so.12.0.0 /leelen/project/V3S/camdroid/frameworks/prebuilts/libminigui_ths.so
#cd ../camdroid/
#make && pack
#mm clean-ccdr && mm clean-libminigui_ths-3.0.so.12 && make && pack
#cd -

