# Makefile for fdos.
# < cuplision@163.com>

# config file, auto been generated
include .config
# define exported values.
include .exports

KERNELFILE=KFOOL
# RELEASE=0 release
# RELEASE=1 debug
# RELEASE=2 demon
PREFIX=arm-arago-linux-gnueabi-
RELEASE=2
CC=$(PREFIX)gcc
CXX=$(PREFIX)g++
AS=nasm
LD=arm-arago-linux-gnueabi-gcc
AR=$(PREFIX)ar
RM=rm
MAKE=make
MAKEPARAM=--no-print-directory
#MAKEPARAM=
OMIT=-
PWD=pwd
WORKDIR=$(shell pwd)
IMGFILE=$(WORKDIR)/floppy.img
#Q=@
Q=
CFLAGS=-c -pipe -O2 -Wall -Wno-unused-parameter -W -D_REENTRANT
CCFLAGS+=-I../qt-4.8.2-arm/mkspecs/qws/linux-arm-gcc -I.
# used for some directory.
CPFLAGS=-I./thirdpart/D8 -Wl,-O1
LDFLAGS=-L. -L/home/tom/workspace/qt-4.8.2-arm/lib \
	-L/home/tom/workspace/tslib/lib -L$(WORKDIR)/thirdpart/D8
LDEXFLAGS=-lrt -lts -lD8U -lD8usb -lpthread
# build-in objects.
y-objs=
yobjs-list:=$(WORKDIR)/.yobjs
# module objects.
m-objs=
mobjs-list:=$(WORKDIR)/.mobjs
# separate objects.
s-objs=
sobjs-list:=$(WORKDIR)/.sobjs
# binary objects.
b-objs=
PHONY+=y-objs m-objs s-objs b-objs
EXPORTS+=KERNELFILE VERSION CC CXX AS LD AR RM MAKE MAKEPARAM OMIT \
	PWD ARCH WORKDIR IMGFILE Q CFLAGS CCFLAGS CMODULE \
	BINASFLAGS CPFLAGS yobjs-list mobjs-list sobjs-list
SEP-DIRS=ontom
# must be the last one.
LAST-DIR=
SUB-DIRS=
EX-OBJS=
PHONY+=EX-OBJS

all: _all

PHONY+=all _all

config:
	$(Q)$(OMIT) chmod +x config.sh
	$(Q)$(OMIT) ./config.sh 0
PHONY+=config
reconfig:
	$(Q)$(OMIT) chmod +x config.sh
	$(Q)$(OMIT) ./config.sh 1
PHONY+=reconfig

# FIXME: maybe there is a better way to refresh objects-list files.
_all:
	$(Q)echo ''> $(yobjs-list);echo ''> $(mobjs-list);echo ''> $(sobjs-list);
	$(Q)for d in $(SEP-DIRS) $(SUB-DIRS) $(LAST-DIR); do\
	   $(MAKE) $(MAKEPARAM) -C $$d all;\
     done
PHONY+=all
drivers:
	$(Q)$(MAKE) $(MAKEPARAM) -C drivers all 2>/dev/null
PHONY+=drivers
fs:
	$(Q)$(MAKE) $(MAKEPARAM) -C fs all 2>/dev/null
PHONY+=fs
kernel:
	$(Q)$(MAKE) $(MAKEPARAM) -C kernel all 2>/dev/null
PHONY+=kernel
modules:
	$(Q)echo ''> $(mobjs-list);
	$(Q)for d in $(SEP-DIRS) $(SUB-DIRS) $(LAST-DIR); do\
	   $(MAKE) $(MAKEPARAM) -C $$d modules;\
     done
PHONY+=modules

zeus:ontom/libontom.a
	$(Q)echo "  **LD**        "`pwd`/$@
	$(Q)$(LD) $(LDFLAGS) $(LDEXFLAGS) $^ -o $@

clean:list-clean
	$(Q)$(OMIT)for d in $(SEP-DIRS) $(SUB-DIRS) $(LAST-DIR);do $(MAKE) $(MAKEPARAM) -C $$d clean 2>/dev/null;done;
PHONY+=clean
list-clean:
	$(Q)for f in $(yobjs-list) $(mobjs-list) $(sobjs-list);do echo "    RM        $$f";done;
	$(Q)$(OMIT)$(RM) $(yobjs-list) $(mobjs-list) $(sobjs-list) 2>/dev/null;
	$(Q)true
PHONY+=list-clean
modules-clean:
	$(Q)echo ''> $(mobjs-list);
	$(Q)for d in $(SEP-DIRS) $(SUB-DIRS) $(LAST-DIR); do\
	   $(MAKE) $(MAKEPARAM) -C $$d modules-clean;\
     done
drivers-clean:
	$(Q)$(MAKE) $(MAKEPARAM) -C drivers clean 2>/dev/null
PHONY+=drivers-clean
kernel-clean:
	$(Q)$(MAKE) $(MAKEPARAM) -C kernel clean 2>/dev/null
PHONY+=kernel-clean
fs-clean:
	$(Q)$(MAKE) $(MAKEPARAM) -C fs clean 2>/dev/null
PHONY+=fs-clean

DOEXPORTS=$(EXPORTS)

export $(EXPORTS) DOEXPORTS
.PHONY: $(PHONY)
