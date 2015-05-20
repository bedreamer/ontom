SRCS=$(wildcard *.c)
VERDEP=$(wildcard *.c) $(wildcard *.h) Makefile ../Makefile ../Rule.make ../lib/bms/* ../lib/lj/*
OBJS=$(patsubst %.c,%.o,$(SRCS))
y-deps+=$(OBJS)
EX-OBJS+=build-in.a

ajax.o: $(VERDEP)

tom.o : $(VERDEP)

build-in.a:$(OBJS)
	$(Q)echo "  **AR**      `pwd`/$@"
	$(Q)$(AR) -cqs $@ $^

include $(WORKDIR)/Rule.make
