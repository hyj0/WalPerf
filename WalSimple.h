//
// Created by hyj on 2019-11-01.
//

#ifndef LEVELDB_WALSIMPLE_H
#define LEVELDB_WALSIMPLE_H

#include <WalPerf.h>

class WalSimple :public WalPerf{
private:
    pthread_mutex_t mutex_;
public:
    WalSimple(const string &walfile_);

    ~WalSimple() override;

    int addWalRecord(string &log) override;
};


#endif //LEVELDB_WALSIMPLE_H
