#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#include "leveldb/c.h"

#define DEFAULT_DBNAME_SIZE 200
#define LRU_SIZE    1024 * 1048576
#define BLOOM_KEY_SIZE  10

struct _leveldb_stuff {
    leveldb_t* db;
    leveldb_options_t* options;
    leveldb_readoptions_t* roptions;
    leveldb_writeoptions_t* woptions;
    char dbname[DEFAULT_DBNAME_SIZE];
};

struct _leveldb_stuff *ldb_initialize(char *path);
void ldb_destroy(struct _leveldb_stuff *ldbs);
void *ldb_get(struct _leveldb_stuff *ldbs, char *key);
void *ldb_put(struct _leveldb_stuff *ldbs, char *key, void *value);
void *ldb_delete(struct _leveldb_stuff *ldbs, char *key);
