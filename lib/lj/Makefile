SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))
m-deps+=$(OBJS)
EX-OBJS+=build-in.a $(WORKDIR)/liblj.so

build-in.a:$(OBJS)
	$(Q)echo "  **AR**      `pwd`/$@"
	$(Q)$(AR) -cqs $@ $^

$(WORKDIR)/liblj.so:$(OBJS)
	$(Q)echo "  **LD**      $@"
	$(Q)$(LD) -shared $(LDFLAGS) -o $@ $^

include $(WORKDIR)/Rule.make
