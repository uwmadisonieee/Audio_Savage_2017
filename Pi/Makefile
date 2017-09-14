CC = gcc
CFLAGS = -Wall -Werror -c
OBJ = server/util.o server/server.o server/websocket.o server/http.o server/sha1.o server/base64.o

main: server
	$(CC) $(OBJ) main.c -lpthread -lwiringPi -o Audio_Savage

server: $(OBJ)

server/%.o: server/%.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm server/*.o
	rm Audio_Savage
