SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))
y-deps+=$(OBJS)
EX-OBJS+=libontom.a

libontom.a:$(OBJS)
	$(Q)echo "  **AR**        `pwd`/$<"
	$(Q)$(AR) -cqs $@ $^

include $(WORKDIR)/Rule.make
