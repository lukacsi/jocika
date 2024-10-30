CC = cc
CFLAGS = -I/usr/include/concord
LDFLAGS = -L/usr/lib -ldiscord -lcurl

main: main.c
	$(CC) main.c -o ./build/main $(CFLAGS) $(LDFLAGS)

