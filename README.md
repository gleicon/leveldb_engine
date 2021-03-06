### LevelDB Engine for Memcached
    Delicious k/v db, WiP

### What's inside
- Memcached backend
- Writes on LevelDB
- Serialize with msgpack
- Replicates with ZMQ


### How to use and test
    - fetch memcached source code, engine-pu branch: https://github.com/memcached/memcached/tree/engine-pu
    - $ git clone -b engine-pu git://github.com/memcached/memcached.git memcached-engine-pu
    - $ cd memcached-engine-pu
    - $ ./config/autorun.sh
    - $ ./configure --prefix=/opt/memcached
    - $ make all install

    - If you don't have it, install memcapable (on MacOSX tou have to brew install libmemcached)

    - Test it: $ /opt/memcached/bin/memcached -E /opt//memcached/lib/memcached/default_engine.so

    - in a different term:   $ memcapable 
    (All tests should pass. your memcached is ok and can switch backends)

    - $ cd leveldb_engine
    - Edit Makefile if you change memcached's include location

    - $ make clean all
    - $ /opt/memcached/bin/memcached -E leveldb_engine

    - in a different term execute $ memcapable

