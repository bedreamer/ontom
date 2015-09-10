SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))
m-deps+=$(OBJS)
EX-OBJS+=build-in.a $(WORKDIR)/libpandora.so

$(WORKDIR)/libpandora.so:$(OBJS)
	$(Q)echo "  **LD**      $@"
	$(Q)$(LD) -fPIC -shared $(LDFLAGS) -o $@ $^

build-in.a:$(OBJS)
	$(Q)echo "  **AR**      `pwd`/$@"
	$(Q)$(AR) -cqs $@ $^

include $(WORKDIR)/Rule.make
