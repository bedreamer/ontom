SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))
m-deps+=$(OBJS)
EX-OBJS+=build-in.a $(WORKDIR)/bmsdrv_0_1.0.so
EXLDFLAGS+=-L$(WORKDIR) -lbms -llj

build-in.a:$(OBJS)
	$(Q)echo "  **AR**      `pwd`/$@"
	$(Q)$(AR) -cqs $@ $^

$(WORKDIR)/bmsdrv_0_1.0.so:$(OBJS)
	$(Q)echo "  **LD**      $@"
	$(Q)$(LD) -shared $(LDFLAGS) $(EXLDFLAGS) -o $@ $^

include $(WORKDIR)/Rule.make
