obj-m := maplesyrup_mod.o
maplesyrup_mod-objs +=  entrypoint.o \
                    cmd_handler.o \
                    kernel_return.o \
                    device_tree.o \
                    undef_hook.o \
                    gic.o \
                    ../common/ms_debug.o \
                    ../common/jumptable.o \
                    ../common/aarch64.o \
                    ../common/secext.o \
                    ../common/virtmem.o \
                    ../common/ident.o \
                    ../common/addrtrans.o \
                    ../common/fault.o \
                    ../common/timer.o \
                    ../common/misc.o \
                    ../common/perfmon.o \
                    ../common/other.o \
                    ../common/fp.o \
                    ../common/jazelle.o \
                    ../common/thumb.o \
                    ../common/debug.o \
                    ../common/impl_def.o \
                    ../common/psr.o

EXTRA_CFLAGS = -I$(src)/../lib/include/ -I$(src)/../common/ -fno-pic
#KERNEL_SRC = /path/to/kernel/source

all:
	make -C $(KERNEL_SRC) M=`pwd` modules

clean:
	make -C $(KERNEL_SRC) M=`pwd` clean
	make -C $(KERNEL_SRC) M=`pwd`/../common/ clean
