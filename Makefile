INCLUDE=-I. -I/usr/local/Cellar/leveldb/1.8.0/include/ -I/opt/memcached/include/
LIBS=-lleveldb
NAME=leveldb_engine

CC = gcc
CFLAGS=-std=gnu99 -g -DNDEBUG -Wall -fno-strict-aliasing -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations \
	    -Wredundant-decls ${INCLUDE} -DHAVE_CONFIG_H -lleveldb

LDFLAGS=-shared

SRC = leveldb_engine.c ldb.c
OBJS = leveldb_engine.o ldb.o

.SUFFIXES: .c
.c .o:                                                                        
		${CC} ${CFLAGS} -c -o $@ $<


leveldb_engine: ${OBJS}
		#$(CC) -c -o $(NAME) $(CFLAGS) $(OBJS) $(LIBS)
		${LINK.c} -o $@ ${OBJS}
clean:                                                                          
		$(RM) leveldb_engine $(OBJS)

