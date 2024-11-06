CC = gcc
CFLAGS = -I/usr/local/include -L/usr/local/lib/
LDFLAGS = -ldiscord -lcurl -pthread
ONAME = bot

main: ./src/main.c
	$(CC)  ./src/main.c -o ./build/$(ONAME) $(CFLAGS) $(LDFLAGS)

clean:
	rm ./build/$(ONAME)
