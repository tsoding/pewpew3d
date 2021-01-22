PKGS=raylib
CFLAGS=-Wall -Wextra -ggdb -pedantic -std=c11 `pkg-config --cflags --static $(PKGS)`
LIBS=`pkg-config --libs --static $(PKGS)`

pewpew3d: main.c
	$(CC) $(CFLAGS) -o pewpew3d main.c $(LIBS)
