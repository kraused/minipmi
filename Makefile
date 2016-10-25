
VERSION    = 1
PATCHLEVEL = 0

ifeq ("$(origin V)", "command line")
	Q = 
else
	Q = @
endif

CC       = gcc
CFLAGS   = -O1 -ggdb
CPPFLAGS =
AR       = ar
ARFLAGS  = rcu

minipmi-obj = minipmi.o common.o version1.o version2.o
tests       = tests/test1.x tests/test1-wo-1.x tests/test1-wo-2.x \
              tests/test2.x tests/test2-wo-1.x tests/test2-wo-2.x \
              tests/test3.x tests/test3-wo-1.x tests/test3-wo-2.x

default: all
all    : libminipmi.a $(tests)

%.o: %.c %.h
	@echo " "CC $@
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

%.x: %.c libminipmi.a
	@echo " "CC $@
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -I$(PWD) $< -o $@ -L$(PWD) -lminipmi

%-wo-1.x: %.c libminipmi.a
	@echo " "CC $@
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -DDISABLE_VERSION=1 -I$(PWD) $< -o $@ -L$(PWD) -lminipmi

%-wo-2.x: %.c libminipmi.a
	@echo " "CC $@
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -DDISABLE_VERSION=2 -I$(PWD) $< -o $@ -L$(PWD) -lminipmi

libminipmi.a: $(minipmi-obj)
	@echo " "AR $@
	$(Q)$(AR) $(ARFLAGS) $@ $^

tar:
	python2 tar.py minipmi $(VERSION).$(PATCHLEVEL)

install:
	install -m0755 -d $(PREFIX)/usr/lib64/
	install -m0644 libminipmi.a $(PREFIX)/usr/lib64/libminipmi.a
	install -m0755 -d $(PREFIX)/usr/include/
	install -m0644 minipmi.h $(PREFIX)/usr/include/minipmi.h

clean:
	rm -f $(minipmi-obj) libminipmi.a $(tests)

