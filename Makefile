
CFLAGS=-Wall -O2 -std=c99

cmtest:	cmtest.c cm.c
	gcc $(CFLAGS) $^ -o $@
