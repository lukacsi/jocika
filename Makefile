CC = gcc
CFLAGS = -I/usr/local/include -L/usr/local/lib/
LDFLAGS = -ldiscord -lcurl -pthread
ONAME = bot

main: main.c
	$(CC)  main.c -o ./build/$(ONAME) $(CFLAGS) $(LDFLAGS)

clean:
	rm ./build/$(ONAME)
