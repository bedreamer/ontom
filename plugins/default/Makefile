SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))
m-deps+=$(OBJS)
EX-OBJS+=build-in.a exso_default.so

build-in.a:$(OBJS)
	$(Q)echo "  **AR**      `pwd`/$@"
	$(Q)$(AR) -cqs $@ $^

exso_default.so:$(OBJS)
	$(Q)echo "  **LD**      $@"
	$(Q)$(LD) -shared $(LDFLAGS) -o $@ $^

include $(WORKDIR)/Rule.make
