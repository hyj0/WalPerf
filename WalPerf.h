//
// Created by hyj on 2019-11-01.
//

#ifndef LEVELDB_WALPERF_H
#define LEVELDB_WALPERF_H

#include <string>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

class WalPerf {
protected:
    string walfile_;
    int walFd;
public:
    WalPerf(string walfile_) : walfile_(walfile_) {
        walFd = open(walfile_.c_str(), O_WRONLY|O_CREAT|O_TRUNC|O_APPEND);
    }
    virtual ~WalPerf() {}
    virtual int addWalRecord(string &log) = 0;
};


#endif //LEVELDB_WALPERF_H
