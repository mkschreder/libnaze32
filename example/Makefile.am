@CODE_COVERAGE_RULES@
bin_PROGRAMS=peripherals
peripherals_SOURCES=peripherals.c
peripherals_CFLAGS=$(CODE_COVERAGE_CFLAGS) -I../src/ -DFREERTOS_PORT_ARM_CM3 -DSTM32F10X_MD -std=gnu99 -Wall -Werror
peripherals_LDFLAGS=$(CODE_COVERAGE_LDFLAGS) -L../src/.libs -L../staging_dir/lib/ -lnaze32 -lfreertos_arm_cm3 -lutype -lstm32f10x_md -Tstm32f10x_md.ld
