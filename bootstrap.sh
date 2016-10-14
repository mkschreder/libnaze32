#!/bin/sh
#!/bin/sh

set -e 

DEPENDS="libstm32 libutype libfreertos" 
GIT_ROOT="https://github.com/mkschreder"
TOOLCHAIN_PREFIX=$(dirname $(dirname $(which arm-none-eabi-gcc))) 
STAGING_DIR=${PWD}/staging_dir/
CONFIGURE_FLAGS=

export CFLAGS="-mthumb -mcpu=cortex-m3 -mfloat-abi=soft -nostartfiles -DCPU_CLOCK_FREQ=72000000UL -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -ffunction-sections -fdata-sections" 
export LDFLAGS="" 

echo "Using toolchain prefix: ${TOOLCHAIN_PREFIX}"

for lib in $DEPENDS; do 
	[ -d $lib ] || git clone "$GIT_ROOT/$lib.git"
	cd $lib
	./configure --host=arm-none-eabi --prefix=${STAGING_DIR}
	make 
	make install
	cd ..
done

export CFLAGS="${CFLAGS} -DSTM32F10X_MD -I$PWD/staging_dir/include/" 
#export LDFLAGS="-Wl,--gc-sections" 
./configure --host=arm-none-eabi
