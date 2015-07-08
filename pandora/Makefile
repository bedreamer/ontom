SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))
m-deps+=$(OBJS)
EX-OBJS+=$(WORKDIR)/pandoras-box build-in.a

$(WORKDIR)/pandoras-box:$(OBJS)
	$(Q)echo "  **LD**      $@"
	$(Q)$(LD) $(LDFLAGS) $(LDEXFLAGS) -o $@ $^

build-in.a:$(OBJS)
	$(Q)echo "  **AR**      `pwd`/$@"
	$(Q)$(AR) -cqs $@ $^

include $(WORKDIR)/Rule.make
