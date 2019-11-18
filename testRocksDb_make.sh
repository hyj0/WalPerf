g++ -g -o ../cmake-build-debug/hyjTest/testRocksDb testRocksDb.cpp -static-libgcc -static-libstdc++ -Wl,--whole-archive -Wl,--no-whole-archive -static -lrocksdb -lsnappy -lbz2 -lpthread -lz
