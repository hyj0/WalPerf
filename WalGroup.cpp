//
// Created by hyj on 2019-11-01.
//
#include <thread>
#include <assert.h>
#include <iostream>
#include "WalGroup.h"
using namespace std;

void writeThreadFunc(WalGroup *this_) {
    pthread_mutex_lock(&this_->writeQue_mutex_);
    timespec stTime;

    int needLock = 0;
    while (1) {
        if (needLock == 1) {
            pthread_mutex_lock(&this_->writeQue_mutex_);
            needLock = 0;
        }
        stTime.tv_sec = 0;
        stTime.tv_nsec = 500*1000; //500 ms
        int ret = pthread_cond_timedwait(&this_->writeCond_, &this_->writeQue_mutex_, &stTime);
        if (ret != ETIMEDOUT && this_->writeQueLen < 100) {
//            cout << " not ETIMEDOUT  writeQueLen=" << this_->writeQueLen << endl;
            continue;
        }
//        if (this_->writeQueLen <= 0) {
//            continue;
//        }
        if (1 || ret == ETIMEDOUT) {
            vector<LogNode *> logNodeVect;
            string buff;
            if (this_->writeQueLen <= 0) {
                continue;
            }
            while (this_->writeQueLen > 0) {
                LogNode *log = this_->writeQue.front();
                logNodeVect.push_back(log);
                buff += log->logvalue;
                this_->writeQue.pop();
                this_->writeQueLen--;
            }
            //解锁 让客户端继续addWalRecord
            needLock = 1;
            pthread_mutex_unlock(&this_->writeQue_mutex_);
            //write
//            cout << "write buff " << buff.length() << " count="<< logNodeVect.size() << endl;
            int n = write(this_->walFd, buff.c_str(), buff.length());
            if (n != buff.length()) {
                assert(0);
            }
            n = fdatasync(this_->walFd);
            if (n != 0) {
                assert(0);
            }
            for (int i = 0; i < logNodeVect.size(); ++i) {
                pthread_mutex_lock(&logNodeVect[i]->mutex_);
                logNodeVect[i]->ret = 0;
                pthread_cond_signal(&logNodeVect[i]->cond_);
                pthread_mutex_unlock(&logNodeVect[i]->mutex_);
            }
        }
    }
    pthread_mutex_unlock(&this_->writeQue_mutex_);
}

WalGroup::WalGroup(string walfile_) : WalPerf(walfile_) {
    writeQueLen = 0;
    writeQue_mutex_ = PTHREAD_MUTEX_INITIALIZER;
    writeCond_ = PTHREAD_COND_INITIALIZER;
    writeThread = new thread(writeThreadFunc, (WalGroup*)this);
    walFd = WalPerf::walFd;
}

WalGroup::~WalGroup() {
//    writeThread->get_id();
}

int WalGroup::addWalRecord(string &log) {
    LogNode logNode(log);
    pthread_mutex_lock(&writeQue_mutex_);
    this->writeQue.push(&logNode);
    this->writeQueLen++;
    pthread_cond_signal(&writeCond_);

    pthread_mutex_unlock(&writeQue_mutex_);

    pthread_mutex_lock(&logNode.mutex_);
    while (1) {
        int ret = pthread_cond_wait(&logNode.cond_, &logNode.mutex_);
        if (logNode.ret == -99) {
            continue;
        }
        pthread_mutex_unlock(&logNode.mutex_);
        return logNode.ret;
    }
    return 0;
}

