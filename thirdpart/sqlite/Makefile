m-deps+=sqlite3.o
EX-OBJS+=build-in.a $(WORKDIR)/libsqlite3.so
EXFLAGS=-DSQLITE_THREADSAFE

build-in.a:sqlite3.o
	$(Q)echo "  **AR**      `pwd`/$@"
	$(Q)$(AR) -cqs $@ $^

$(WORKDIR)/libsqlite3.so:sqlite3.o
	$(Q)echo "  **LD**      $@"
	$(Q)$(LD) -shared $(LDFLAGS) -o $@ $^

include $(WORKDIR)/Rule.make
