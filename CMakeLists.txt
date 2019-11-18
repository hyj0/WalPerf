set(CMAKE_EXE_LINKER_FLAGS " -fexceptions -static -static-libgcc -static-libstdc++  -Wl,--whole-archive -lpthread -Wl,--no-whole-archive")
add_executable(testLevelDb testLevelDb.cpp)
target_link_libraries(testLevelDb leveldb snappy)

include_directories(.)
add_executable(WalMain WalMain.cpp WalPerf.cpp WalSimple.cpp WalGroup.cpp WalGroup.h )
target_link_libraries(WalMain pthread)


add_executable(testRocksDb testRocksDb.cpp)
target_link_libraries(testRocksDb rocksdb snappy bz2 z)