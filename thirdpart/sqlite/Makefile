m-deps+=sqlite3.o
EX-OBJS+=build-in.a libsqlite3.so
EXFLAGS=-DSQLITE_THREADSAFE

build-in.a:$(OBJS)
	$(Q)echo "  **AR**      `pwd`/$@"
	$(Q)$(AR) -cqs $@ $^

libsqlite3.so:$(OBJS)
	$(Q)echo "  **LD**      $@"
	$(Q)$(LD) -shared $(LDFLAGS) -o $@ $^

include $(WORKDIR)/Rule.make
