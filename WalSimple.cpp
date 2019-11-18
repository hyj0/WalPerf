//
// Created by hyj on 2019-11-01.
//

#include "WalSimple.h"

int WalSimple::addWalRecord(string &log) {
//    pthread_mutex_lock(&mutex_);
    int ret = write(this->walFd, log.c_str(), log.length());
    if (ret != log.length()) {
        pthread_mutex_unlock(&mutex_);
        return ret;
    }
    ret = fdatasync(walFd);
//    pthread_mutex_unlock(&mutex_);
    return ret;
}

WalSimple::WalSimple(const string &walfile_) : WalPerf(walfile_) {
    mutex_ = PTHREAD_MUTEX_INITIALIZER;
}

WalSimple::~WalSimple() {

}
