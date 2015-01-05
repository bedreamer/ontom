# 直流充电桩监控系统生成规则文件
# if you need a sub-dir Makefile, you can define
# `sub-didrs:=$(somedirs)` in your Makefile before include this rule file.
# define `m-objs:=$(module)` to build a kernel module, depends on m-deps
# so you need define the other value named`m-deps:=$(some-objects` also.
include $(WORKDIR)/.config

# this is for sub-dirs Make rule.
ifdef sub-dirs
suball:=sub_all
submodules:=sub_modules
subclean:=sub_clean
submodules-clean:=sub-modules-clean
sub_all:
	$(Q)$(OMIT)for d in $(sub-dirs); do $(MAKE) $(MAKEPARAM) -C $$d all;done;
PHONY+=sub_all
sub_modules:
	$(Q)$(OMIT)for d in $(sub-dirs); do $(MAKE) $(MAKEPARAM) -C $$d modules;done;
PHONY+=sub_modules
sub_clean:
	$(Q)$(OMIT)for d in $(sub-dirs); do $(MAKE) $(MAKEPARAM) -C $$d clean;done;
PHONY+=sub_clean
sub-modules-clean:
	$(Q)$(OMIT)for d in $(sub-dirs); do $(MAKE) $(MAKEPARAM) -C $$d modules-clean;done;
PHONY+=sub-modules-clean
endif

ifdef m-objs
CPFLAGS+=-D_MODULE_
endif

# for module complire.
$(m-objs) : $(m-deps)

all : $(b-objs) $(s-deps) $(y-deps) $(m-objs) $(suball) $(nothing)
	$(Q)$(OMIT)for o in $(y-deps); do PWD=`pwd`; echo $$PWD/$$o >> $(yobjs-list); done;
	$(Q)$(OMIT)for o in $(s-deps); do PWD=`pwd`; echo $$PWD/$$o >> $(sobjs-list); done
PHONY+=all
modules: $(m-objs) $(submodules)
	$(Q)true;
PHONY+=modules
clean: $(subclean)
	$(Q)$(OMIT)echo "    RM        `pwd`/*.(o|ko)"
	$(Q)$(OMIT)$(RM) build-in.a $(b-objs) $(m-objs) $(m-deps) $(y-deps) $(s-deps) 2>/dev/null;
PHONY+=clean
ifdef m-objs
modules-clean:$(submodules-clean)
	$(Q)$(OMIT)$(RM) "    RM        `pwd`/*.ko"
	$(Q)$(OMIT)$(RM) build-in.a $(m-objs) $(m-deps) 2>/dev/null;
PHONY+=modules-clean
else
modules-clean:$(submodules-clean)
	$(Q)true;
PHONY+=modules-clean
endif
%.o : %.c Makefile
	$(Q)echo "    CC        `pwd`/$<"
	$(Q)$(CC) $(CFLAGS) $(CCFLAGS) $(CPFLAGS) -o $@ $<

%.o : %.s Makefile
	$(Q)echo "    AS        `pwd`/$<"
	$(Q)$(AS) $(ASFLAGS) -o $@ $<

%.bin : %.s Makefile
	$(Q)echo "    AS [BIN]  `pwd`/$<"
	$(Q)$(AS) $(BINASFLAGS) -o $@ $<

# kernel module file is a share libary file.
%.ko : $(m-deps) Makefile
	$(Q)echo "    LD [M]    `pwd`/$@"
	$(Q)$(LD) -r $(m-deps) -o $@
	$(Q)echo "`pwd`/$@" >> $(mobjs-list); done

# used for multi level Makefile.
.PHONY:$(PHONY)

# you can export some private value in sub-dir also.
# usage: EXPORT+=SOME-VALUE
export $(EXPORT)

# for sub-dir enviroment.
export $(DOEXPORTS)
