
ifeq ("$(origin V)", "command line")
	Q = 
else
	Q = @
endif

CC       = gcc
CFLAGS   = -O0 -ggdb -DDEBUG
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

clean:
	rm -f $(minipmi-obj) libminipmi.a $(tests)

