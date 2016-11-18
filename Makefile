CFLAGS = "-g"
xxs: xxs0.c crc32.c
	$(CC) $(CFLAGS) -o $@ $^

all: xxs

