#include "ldb.h"

struct _leveldb_stuff *ldb_initialize(char *path){
    
    struct _leveldb_stuff *ldbs = NULL;
    leveldb_cache_t *cache;
    leveldb_filterpolicy_t *policy;
    char* err = NULL;

    ldbs = malloc(sizeof(struct _leveldb_stuff));
    memset(ldbs, 0, sizeof(struct _leveldb_stuff));

    ldbs->options = leveldb_options_create();
    snprintf(ldbs->dbname, sizeof(ldbs->dbname), "%s-%d", path, ((int) geteuid()));
    
    cache = leveldb_cache_create_lru(LRU_SIZE);
    policy = leveldb_filterpolicy_create_bloom(BLOOM_KEY_SIZE);
    
    leveldb_options_set_filter_policy(ldbs->options, policy);
    leveldb_options_set_cache(ldbs->options, cache);

    ldbs->roptions = leveldb_readoptions_create();
    leveldb_readoptions_set_verify_checksums(ldbs->roptions, 1);
    leveldb_readoptions_set_fill_cache(ldbs->roptions, 0);

    ldbs->woptions = leveldb_writeoptions_create();
    leveldb_writeoptions_set_sync(ldbs->woptions, 1);
 
    leveldb_options_set_create_if_missing(ldbs->options, 1);
    ldbs->db = leveldb_open(ldbs->options, ldbs->dbname, &err);

    if (err) { 
        fprintf(stderr,"%s", err);
        leveldb_free(err);
        free(ldbs);
        return NULL;
    } else {
        return ldbs;
    }

}

void ldb_destroy(struct _leveldb_stuff *ldbs){
    char* err = NULL;
    leveldb_close(ldbs->db);
    leveldb_destroy_db(ldbs->options, ldbs->dbname, &err);
    if (err) { 
        fprintf(stderr,"%s", err);
    }
    leveldb_options_destroy(ldbs->options);
    leveldb_readoptions_destroy(ldbs->roptions);
    leveldb_writeoptions_destroy(ldbs->woptions);
    free(ldbs);
}

void *ldb_get(struct _leveldb_stuff *ldbs, char *key){
    char *err=NULL;
    char *retval = NULL;
    size_t temp_val_len;
    retval = leveldb_get(ldbs->db, ldbs->roptions, key, strlen(key), &temp_val_len, &err);    
    if (err) { 
        fprintf(stderr,"%s", err);
        return NULL;
    } else {
        return retval;
    }
}

void *ldb_put(struct _leveldb_stuff *ldbs, char *key, void *value){
    char *err=NULL;
    leveldb_put(ldbs->db, ldbs->woptions, key, strlen(key), value, strlen(value), &err);
    if (err) { 
        fprintf(stderr,"%s", err);
        return NULL;
    } else {
        return key;
    }
}

void *ldb_delete(struct _leveldb_stuff *ldbs, char *key){
    char* err = NULL;
    leveldb_delete(ldbs->db, ldbs->woptions, key, strlen(key), &err);

    if (err) { 
        fprintf(stderr,"%s", err);
        return NULL;
    } else {
        return key;
    }
}
