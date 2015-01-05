SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))
y-deps+=$(OBJS)
EX-OBJS+=libontom.a

build-in.a:$(OBJS)
	$(Q)echo "  **AR**        `pwd`/$<"
	$(Q)$(AR) -cqs $@ $^

include $(WORKDIR)/Rule.make
