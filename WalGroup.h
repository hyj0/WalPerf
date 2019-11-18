//
// Created by hyj on 2019-11-01.
//

#ifndef LEVELDB_WALGROUP_H
#define LEVELDB_WALGROUP_H

#include <queue>
#include <WalPerf.h>
#include <string>
#include <thread>
using namespace std;

class LogNode {
public:
    string logvalue;
    int ret; //–¥»Î∑µªÿ
    pthread_mutex_t mutex_;
    pthread_cond_t cond_;
public:
    LogNode(const string &logvalue) : logvalue(logvalue) {
        ret = -99;
        mutex_ = PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_init(&cond_, NULL);
    }
    virtual ~LogNode() {}

};

class WalGroup : public WalPerf {
public:
    queue<LogNode*> writeQue;
    int writeQueLen;
    pthread_mutex_t writeQue_mutex_;
    pthread_cond_t writeCond_;
    thread *writeThread;
    int walFd;

public:
    WalGroup(string walfile_);

    ~WalGroup() override;

    int addWalRecord(string &log) override;
};


#endif //LEVELDB_WALGROUP_H
