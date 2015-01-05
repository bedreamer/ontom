SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))
y-deps+=$(OBJS)
EX-OBJS+=libontom.a

libontom.a:$(OBJS)
	$(Q)$(AR) cqs -o $@ $^

include $(WORKDIR)/Rule.make
