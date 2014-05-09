UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
SHAREDFLAGS = -dynamiclib
SHAREDEXT = dylib
else
SHAREDFLAGS = -shared
SHAREDEXT = so
endif

LIB_SRC=cm.c bf.c hash.c
LIB_HDR=cm.h bh.h hash.h
LIB_OBJ=$(LIB_SRC:.c=.o)
SO_OBJS=cm.o bf.o hash.o
SO_NAME=libcmsketch.$(SHAREDEXT)
ifneq ($(UNAME), Darwin)
    SHAREDFLAGS += -Wl,-soname,$(SO_NAME)
endif

A_NAME=libcmsketch.a

INCLUDES=-I.
SRC=cmtest.c
OBJ=cmtest.o
OUT=cmtest

CFLAGS += -Werror -Wall -Wextra -pedantic -std=c99
CC=gcc

ifeq ("$(LIBDIR)", "")
LIBDIR=/usr/local/lib
endif

ifeq ("$(INCDIR)", "")
INCDIR=/usr/local/include
endif

default: $(OUT)

.c.o:
	$(CC) -c -fPIC $(CFLAGS) $< -o $@

$(SO_NAME): $(LIB_OBJ)
	$(CC) $(SHAREDFLAGS) -o $(SO_NAME).1.0 $(SO_OBJS)
	ln -sf ./$(SO_NAME).1.0 ./$(SO_NAME).1
	ln -sf ./$(SO_NAME).1.0 ./$(SO_NAME)

$(A_NAME): $(LIB_OBJ)
	ar -r $(A_NAME) $(SO_OBJS)

$(OUT): $(SO_NAME) $(A_NAME) $(SRC)
	$(CC) -c $(INCLUDES) $(CFLAGS) $(SRC) -o $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(A_NAME) -o $(OUT)

check: $(OUT)
	LD_LIBRARY_PATH=. ./$(OUT)

clean:
	rm -f *.o *.a *.$(SHAREDEXT)  $(SO_NAME).* $(OUT)

install:
	 @echo "Installing libraries in $(LIBDIR)"; \
	 cp -pv $(A_NAME) $(LIBDIR)/;\
	 cp -Rv $(SO_NAME)* $(LIBDIR)/;\
	 echo "Installing headers in $(INCDIR)"; \
	 cp -pv $(LIB_HDR) $(INCDIR)/;
