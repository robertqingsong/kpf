CC=gcc
AR=ar
STRIP=strip
RM=rm -rf
CFLAGS=
LDFLAGS=
INCLUDE_DIR=-I../../inc -I../../src
TARGET=libkpf.a

OBJECTS=../../src/block.o ../../src/bsearch.o ../../src/future.o ../../src/list.o \
	../../src/btree.o ../../src/mem_api.o ../../src/select.o ../../src/task.o \
	../../src/kqueue.o ../../src/lock.o ../../src/net_api.o ../../src/sem.o \
	../../src/bsearch.o ../../src/log.o ../../src/oal_api.o ../../src/smart.o

all:$(OBJECTS)
	ar rc  $(TARGET) $(OBJECTS)

$(OBJECTS):%.o:%.c
	$(CC) -c $(INCLUDE_DIR) $< -o $@

clean:
	$(RM) -rf $(OBJECTS)
	$(RM) -rf $(TARGET)

