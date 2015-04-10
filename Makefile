# Makefile for fdos.
# < cuplision@163.com>

# config file, auto been generated
include .config
# define exported values.
include .exports

ZEUS=zeus
# RELEASE=0 release
# RELEASE=1 debug
# RELEASE=2 demon
VER= $(shell svn info | grep 'Revision'|grep -o '[0-9]\+')
PREFIX=arm-arago-linux-gnueabi-
RELEASE=2
CC=$(PREFIX)gcc
CXX=$(PREFIX)g++
AS=nasm
LD=$(PREFIX)gcc
AR=$(PREFIX)ar
RM=rm
MAKE=make
MAKEPARAM=--no-print-directory
#MAKEPARAM=
OMIT=-
PWD=pwd
WORKDIR=$(shell pwd)
#Q=@
Q=
CFLAGS=-c -DVERSION=$(VER) -pipe -O2 -Wall -Wno-unused-parameter -W -D_REENTRANT
CCFLAGS+=-I../qt-4.8.2-arm/mkspecs/qws/linux-arm-gcc -I.
# used for some directory.
CPFLAGS=-I./ontom -I./thirdpart/D8 -I./thirdpart/mongoose -I./thirdpart/sqlite -I./thirdpart/extso -Wl,-O1
EXFLAGS=
LDFLAGS=-L. -L/home/tom/workspace/qt-4.8.2-arm/lib \
	-L/home/tom/workspace/tslib/lib -L$(WORKDIR)/lib
LDEXFLAGS=-lrt -lpthread -ldl -llj
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
	BINASFLAGS CPFLAGS EXFLAGS yobjs-list mobjs-list sobjs-list
SEP-DIRS=thirdpart/mongoose thirdpart/sqlite thirdpart/extso
# must be the last one.
LAST-DIR=
SUB-DIRS=ontom lib/lj plugins/default plugins/demo
EX-OBJS=
PHONY+=EX-OBJS

all: _all $(ZEUS)
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
	$(Q)dd if=/dev/null of=$(yobjs-list) bs=1 count=0 2>/dev/null
	$(Q)dd if=/dev/null of=$(mobjs-list) bs=1 count=0 2>/dev/null
	$(Q)dd if=/dev/null of=$(sobjs-list) bs=1 count=0 2>/dev/null
	$(Q)for d in $(SEP-DIRS) $(SUB-DIRS) $(LAST-DIR); do\
	   $(MAKE) $(MAKEPARAM) -C $$d all;\
     done
PHONY+=all

modules:
	$(Q)echo ''> $(mobjs-list);
	$(Q)for d in $(SEP-DIRS) $(SUB-DIRS) $(LAST-DIR); do\
	   $(MAKE) $(MAKEPARAM) -C $$d modules;\
     done
PHONY+=modules

zeus:
	$(Q)echo "------------------------------------------"
	$(Q)echo "  **LD**      "`pwd`/$@_r$(VER)
	$(Q)$(LD) $(shell cat $(yobjs-list)) $(LDFLAGS) $(LDEXFLAGS) -o $@_r$(VER)
	$(Q)$(OMIT)$(RM) me 2>/dev/null
	$(Q)ln -s $@_r$(VER) me 2>/dev/null
	$(Q)echo "         "`date`
	$(Q)echo "------------------------------------------"

browser:
	$(Q)$(MAKE) $(MAKEPARAM) -C browser all;

watchdog:
	$(Q)$(MAKE) $(MAKEPARAM) -C watchdog all;
	
clean:list-clean
	$(Q)$(OMIT)for d in $(SEP-DIRS) $(SUB-DIRS) $(LAST-DIR);do $(MAKE) $(MAKEPARAM) -C $$d clean 2>/dev/null;done;
PHONY+=clean
list-clean:
	$(Q)for f in $(yobjs-list) $(mobjs-list) $(sobjs-list);do echo "    RM        $$f";done;
	$(Q)$(OMIT)$(RM) $(yobjs-list) $(mobjs-list) $(sobjs-list) 2>/dev/null;
	$(Q)true
PHONY+=list-clean

DOEXPORTS=$(EXPORTS)

export $(EXPORTS) DOEXPORTS
.PHONY: $(PHONY)
