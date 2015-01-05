SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$SRCS)
y-deps+=printk.o kmain.o exec.o kmodel.o schedu.o signal.o time.o
include $(WORKDIR)/Rule.make
