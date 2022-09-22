# set compiler defaults for OSX versus *nix
CFLAGS = -Wall -g3 -pedantic -fPIC
DFLAGS = $(LN_DEBUG) -I$(DEBUGDIR)
LDLIBS = -lm

OBJS := $(patsubst %.c, %.o, $(wildcard *.c))

$(info $$OBJS is [${OBJS}])

libtpd.a: $(OBJS)
	ar -r libtpd.a $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(DFLAGS)

clean:
	rm -f $(CURDIR)/*.o $(CURDIR)/*.a