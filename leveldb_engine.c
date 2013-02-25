/*
 * leveldb engine for memcached 
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "memcached/visibility.h"
#include "memcached/engine.h"
#include "memcached/util.h"
#include "leveldb/c.h"
#include "ldb.h"

struct leveldb_engine {
    ENGINE_HANDLE_V1 engine;
};

struct _leveldb_stuff *ldbs;

struct leveldb_item {
    void *key;
    size_t nkey;
    void *data;
    size_t ndata;
    int flags;
    rel_time_t exptime;
};

static void ldbe_destroy(ENGINE_HANDLE *h) {
    free(h);
    ldb_destroy(ldbs);
}


static void ldbe_item_release(ENGINE_HANDLE* handle,
                            const void *cookie,
                            item* item) {
    struct leveldb_item *it = item;
    free(it->key);
    free(it->data);
    free(it);
}


static ENGINE_ERROR_CODE ldbe_initialize(ENGINE_HANDLE *h, const char* config_str) {
    ldbs = ldb_initialize("/tmp/wat");
    if (ldbs) return ENGINE_SUCCESS;
    else return ENGINE_FAILED;
}

static ENGINE_ERROR_CODE ldbe_allocate(ENGINE_HANDLE* handle,
                                     const void* cookie,
                                     item **item,
                                     const void* key,
                                     const size_t nkey,
                                     const size_t nbytes,
                                     const int flags,
                                     const rel_time_t exptime) {

    struct leveldb_item *it = malloc(sizeof(struct leveldb_item));
    if (it == NULL) return ENGINE_ENOMEM;
    it->flags = flags;
    it->exptime = exptime;
    it->nkey = nkey;
    it->ndata = nbytes;
    it->key = malloc(nkey);
    it->data = malloc(nbytes);

    if (it->key == NULL || it->data == NULL) {
        free(it->key);
        free(it->data);
        free(it);
        return ENGINE_ENOMEM;
    }

    memcpy(it->key, key, nkey);
    *item = it;
    return ENGINE_SUCCESS;
}

static bool ldbe_get_item_info(ENGINE_HANDLE *handle, const void *cookie,
                             const item* item, item_info *item_info) {

    struct leveldb_item *it = (struct leveldb_item*)item;

    if (item_info->nvalue < 1) return false;

    item_info->cas = 0; /* Not supported */
    item_info->clsid = 0; /* Not supported */
    item_info->exptime = it->exptime;
    item_info->flags = it->flags;
    item_info->key = it->key;
    item_info->nkey = it->nkey;
    item_info->nbytes = it->ndata; /* Total length of the items data */
    item_info->nvalue = 1; /* Number of fragments used */
    item_info->value[0].iov_base = it->data; /* pointer to fragment 1 */
    item_info->value[0].iov_len = it->ndata; /* Length of fragment 1 */

    return true;
}

static ENGINE_ERROR_CODE ldbe_store(ENGINE_HANDLE* handle,
                                  const void *cookie,
                                  item* item,
                                  uint64_t *cas,
                                  ENGINE_STORE_OPERATION operation,
                                  uint16_t vbucket) {

    struct leveldb_item *it = item;
    void *ret;
    // msgpack it
    ret = ldb_put(ldbs, it->key, it);
    if (ret == NULL) return ENGINE_NOT_STORED;
    *cas = 0;
    return ENGINE_SUCCESS;
}

static ENGINE_ERROR_CODE ldbe_get(ENGINE_HANDLE* handle,
                                const void* cookie,
                                item** item,
                                const void* key,
                                const int nkey,
                                uint16_t vbucket) {

    struct leveldb_item* it = NULL;
    int ndata;
    char * ret;

    it = ldb_get(ldbs, key);
    if (it == NULL) return ENGINE_KEY_ENOENT;
    

    ndata = it->ndata;

    //ENGINE_ERROR_CODE ret = leveldb_allocate(handle, cookie, (void**)&it, key, nkey,
    //                                   ndata, 0, 0);
    
    if (ret != ENGINE_SUCCESS) return ENGINE_ENOMEM;
    
    *item = it;
    return ENGINE_SUCCESS;
}

static const engine_info* ldbe_get_info(ENGINE_HANDLE* handle) {
   static engine_info info = {
      .description = "LevelDB engine v0.1",
      .num_features = 0
   };

   return &info;
}

static ENGINE_ERROR_CODE ldbe_item_delete(ENGINE_HANDLE* handle,
                                        const void* cookie,
                                        const void* key,
                                        const size_t nkey,
                                        uint64_t cas,
                                        uint16_t vbucket) {
   return ENGINE_KEY_ENOENT;
}

static ENGINE_ERROR_CODE ldbe_get_stats(ENGINE_HANDLE* handle,
                                      const void* cookie,
                                      const char* stat_key,
                                      int nkey,
                                      ADD_STAT add_stat) {
   return ENGINE_SUCCESS;
}

static ENGINE_ERROR_CODE ldbe_flush(ENGINE_HANDLE* handle,
                                  const void* cookie, time_t when) {

   return ENGINE_SUCCESS;
}

static void ldbe_reset_stats(ENGINE_HANDLE* handle, const void *cookie) {

}

static ENGINE_ERROR_CODE ldbe_unknown_command(ENGINE_HANDLE* handle,
                                            const void* cookie,
                                            protocol_binary_request_header *request,
                                            ADD_RESPONSE response) {
   return ENGINE_ENOTSUP;
}

static void ldbe_item_set_cas(ENGINE_HANDLE *handle, const void *cookie,
                            item* item, uint64_t val){

}


MEMCACHED_PUBLIC_API ENGINE_ERROR_CODE create_instance(uint64_t interface, GET_SERVER_API get_server_api, 
        ENGINE_HANDLE **handle) {

            if (interface == 0) return ENGINE_ENOTSUP;
            struct leveldb_engine *h = calloc(1, sizeof(*h));
            if (h == NULL) return ENGINE_ENOMEM;
            h->engine.interface.interface = 1;

            /* command handlers */
            h->engine.initialize = ldbe_initialize;
            h->engine.destroy = ldbe_destroy;
            h->engine.get_info = ldbe_get_info;
            h->engine.allocate = ldbe_allocate;
            h->engine.remove = ldbe_item_delete;
            h->engine.release = ldbe_item_release;
            h->engine.get = ldbe_get;
            h->engine.get_stats = ldbe_get_stats;
            h->engine.reset_stats = ldbe_reset_stats;
            h->engine.store = ldbe_store;
            h->engine.flush = ldbe_flush;
            h->engine.unknown_command = ldbe_unknown_command;
            h->engine.item_set_cas = ldbe_item_set_cas;
            h->engine.get_item_info = ldbe_get_item_info;

            *handle = (ENGINE_HANDLE *) h;
            return ENGINE_SUCCESS;
}

