//
// Created by hyj on 2019-10-24.
//
#include <iostream>
#include <cassert>
#include <leveldb/db.h>
#include <sstream>
#include <unistd.h>
#include <thread>

using namespace std;
#define WRITE_THREAD_N 256

void writeThread(leveldb::DB *db, int *count, int index) {
//    cout << "writeThread " << this_thread::get_id() << endl;
    char buff[30];
    memset(buff, 'a', sizeof(buff));
    buff[30-1] = 0;
    int p = 10000 * 10000/WRITE_THREAD_N;

    for (int i = p*index; i < p*index + p; ++i) {
        stringstream ss;
        ss << i;
        const leveldb::Slice value = buff;
        leveldb::Slice key2 = ss.str();
        leveldb::WriteOptions writeOpt;
        writeOpt.sync = true;
        const leveldb::Status &s = db->Put(writeOpt, key2, value);
        if (!s.ok()) {
            cout << "i=" <<i << " " << s.ToString() << endl;
        }
        if (i != 0 && i % (10*1000) == 0) {
//            time(&tEnd);
//            cout << "speek=" <<((float)i /(tEnd-tStart)) << endl;
        }
        *count += 1;
    }
}

void calcThread(int *countArr, int arrLen)
{
    int sec = 0;
    long sum = 0;
    while (1) {
        sleep(1);
        sum = 0;
        sec += 1;
        for (int i = 0; i < arrLen; ++i) {
            sum += countArr[i];
        }
        cout << "speek=" <<((float)sum /(sec)) << " filesmystem=" << sum*(36)*1.0/1024/(sec) << " k/s "  << sum*(36)*1.0/1024/1024/(sec) << " m/s" << endl;
        if (sum == arrLen*(10000 * 10000)) {
            break;
        }
    }
}

int main() {
    leveldb::DB* db;

    leveldb::Options options;
    options.create_if_missing = true;
    options.write_buffer_size = 1 << 30;
    options.max_file_size = 1 << 30;
    cout << "options.write_buffer_size="<< options.write_buffer_size << endl;
    cout << "options.max_file_size="<< options.max_file_size << endl;

    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
    assert(status.ok());

    std::string value;
    leveldb::Slice key1("key1");
    leveldb::Status s = db->Get(leveldb::ReadOptions(), key1, &value);

    leveldb::WriteOptions writeOpt;
    writeOpt.sync = true;
    leveldb::Slice &key2 = key1;
    if (!s.ok()) {
        s = db->Put(writeOpt, key2, value);
    }
    value = "vl2";
    key2 = "key2";
    s = db->Put(writeOpt, key2, value);
    cout << s.ToString() << endl;

    value = "vl3";
    key2 = "key3";
    s = db->Put(writeOpt, key2, value);
    cout << s.ToString() << endl;

    char buff[30];
    memset(buff, 'a', sizeof(buff));
    buff[30-1] = 0;
    time_t tStart;
    time_t tEnd;
    time(&tStart);
//    for (int i = 0; i < 10000 * 10000; ++i) {
//        stringstream ss;
//        ss << i;
//        value = ss.str();
//        key2 = buff;
//        s = db->Put(writeOpt, key2, value);
//        if (!s.ok()) {
//            cout << "i=" <<i << " " << s.ToString() << endl;
//        }
//        if (i != 0 && i % (10*1000) == 0) {
//            time(&tEnd);
////            cout << "speek=" <<((float)i /(tEnd-tStart)) << endl;
//            cout << "speek=" <<((float)i /(tEnd-tStart)) << " filesmystem=" << i*(key2.size()+value.size())*1.0/(tEnd-tStart) << " k/s "  << i*(key2.size()+value.size())*1.0/1024/(tEnd-tStart) << " m/s" << endl;
//        }
//    }

//    time(&tEnd);
//    cout << "speek=" << 10000*10000/(tEnd-tStart) << endl;
//    sleep(1000);

    thread *threadArr[WRITE_THREAD_N+1];
    int countArr[WRITE_THREAD_N];
    for (int j = 0; j < WRITE_THREAD_N; ++j) {
        countArr[j] = 0;
        threadArr[j] = new thread(writeThread, db, &countArr[j], j);
    }
    threadArr[WRITE_THREAD_N] = new thread(calcThread, countArr, WRITE_THREAD_N);

    for (int k = 0; k < WRITE_THREAD_N+1; ++k) {
        threadArr[k]->join();
    }

//    assert(0);
    if (s.ok()) {
//        s = db->Delete(leveldb::WriteOptions(), key1);
    }

//    delete db;
}

