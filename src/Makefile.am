@CODE_COVERAGE_RULES@
includedir=$(prefix)/include/
lib_LTLIBRARIES=libnaze32.la
include_HEADERS=naze32.h
libnaze32_la_SOURCES=naze32.c uart.c leds.c usb_vcp.c
libnaze32_la_CFLAGS=$(CODE_COVERAGE_CFLAGS) -I../staging_dir/include/stm32/f10x/ -DFREERTOS_PORT_ARM_CM3 -DSTM32F10X -DSTM32F10X_MD -std=gnu99 -Wall -Werror
