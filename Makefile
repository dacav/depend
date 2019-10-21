.PHONY: all clean

BINS = depend

CFLAGS += -D_POSIX_C_SOURCE=2
CFLAGS += -Wall
CFLAGS += -Werror
CFLAGS += -std=c11

all: $(BINS)

clean:
	rm -rvf *o $(BINS)
