SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))
m-deps+=$(OBJS)
EX-OBJS+=build-in.a $(WORKDIR)/libjob.so

build-in.a:$(OBJS)
	$(Q)echo "  **AR**      `pwd`/$@"
	$(Q)$(AR) -cqs $@ $^

$(WORKDIR)/libjob.so:$(OBJS)
	$(Q)echo "  **LD**      $@"
	$(Q)$(LD) -fPIC -shared $(LDFLAGS) -o $@ $^

include $(WORKDIR)/Rule.make
