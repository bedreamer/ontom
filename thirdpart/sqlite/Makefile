y-deps+=sqlite3.o
EX-OBJS+=build-in.a

build-in.a:$(OBJS)
	$(Q)echo "  **AR**      `pwd`/$@"
	$(Q)$(AR) -cqs $@ $^

include $(WORKDIR)/Rule.make
