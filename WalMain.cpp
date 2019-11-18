//
// Created by hyj on 2019-11-01.
//
#include <iostream>
#include <string>
#include <WalPerf.h>
#include <WalSimple.h>
#include <WalGroup.h>
#include <cstring>

using namespace std;

#define WRITE_THREAD_N 256


void writeThread(WalPerf *wal, int *count, int index, int argc, char **argv) {
//    cout << "writeThread " << this_thread::get_id() << endl;
//    char buff[30];
//    memset(buff, 'a', sizeof(buff));
//    buff[30-1] = 0;
    int p = 10000 * 10000/WRITE_THREAD_N;

//    int count = 10000*100;
    char buff[1024*6];
    memset(buff, 'a', sizeof(buff));
    buff[sizeof(buff)-1] = '\0';
    if (argc == 2) {
        buff[atoi(argv[1])] = '\0';
    }
    string walLog = buff;

    for (int i = p*index; i < p*index + p; ++i) {
        time_t tStart = time(NULL);
        int ret = wal->addWalRecord(walLog);
        time_t tEnd = time(NULL);
        if (tEnd - tStart >= 1) {
//            cout << "delay sec=" << tEnd-tStart << endl;
        }
        if (ret != 0) {
            cout << "ret=" << ret << endl;
            return ;
        }

        *count += 1;
    }
}
class WriteThreadArg {
public:
    WalPerf *wal;
    int *count;
    int index;
    int argc;
    char **argv;
};
void *writeThreadEntry(void *arg) {
    WriteThreadArg *writeThreadArg = static_cast<WriteThreadArg *>(arg);
    writeThread(writeThreadArg->wal, writeThreadArg->count, writeThreadArg->index, writeThreadArg->argc, writeThreadArg->argv);
}

void calcThread(int *countArr, int arrLen, int argc, char **argv)
{
    int sec = 0;
    long sum = 0;
    while (1) {
        sum = 0;
        sleep(1);
        sec += 1;
        for (int i = 0; i < arrLen; ++i) {
            sum += countArr[i];
        }
//        cout << "speek=" <<((float)sum /sec) << endl;
        int size = 1024*6;
        if (argc > 1) {
            size = atoi(argv[1]);
        }
        cout << "speek=" <<((float)sum /sec) << " filesmystem=" << size*(sum)*1.0/1024/(sec) << " k/s "  << size*(sum)*1.0/1024/1024/(sec) << " m/s" << endl;
        if (sum == arrLen*(10000 * 10000)) {
            break;
        }
    }
}

int main(int argc, char **argv) {
    cout << "usage:" << argv[0] << " buffLen thread_count " << endl;
    string walfile = "/tmp/wal.dat";
    WalGroup wal(walfile);
    time_t tStart;
    time_t tEnd;

    int count = 10000*100;
    char buff[1024*6];
    memset(buff, 'a', sizeof(buff));
    buff[sizeof(buff)-1] = '\0';
    if (argc >= 2) {
        buff[atoi(argv[1])] = '\0';
    }
    int thread_count = WRITE_THREAD_N;
    if (argc >= 3) {
        thread_count = atoi(argv[2]);
        if (thread_count >WRITE_THREAD_N) {
            thread_count = WRITE_THREAD_N;
        }
    }

    string walLog = buff;
    cout << "buffLen=" << walLog.length() << endl;
    cout << "thread_count=" << thread_count << endl;
    time(&tStart);
//    for (int i = 0; i < count; ++i) {
//        int ret = wal.addWalRecord(walLog);
//        if (ret != 0) {
//            cout << "ret=" << ret << endl;
//            return ret;
//        }
//        if (i != 0 && i % (1*100) == 0) {
//            time(&tEnd);
//            cout << "speek=" <<((float)i /(tEnd-tStart)) << " filesystem=" << i*walLog.length()*1.0/(tEnd-tStart) << " k/s "  << i*walLog.length()*1.0/1024/(tEnd-tStart) << " m/s" << endl;
//        }
//    }

#if 1
    thread *threadArr[WRITE_THREAD_N+1];
    int countArr[WRITE_THREAD_N];
    for (int j = 0; j < thread_count; ++j) {
        countArr[j] = 0;
        threadArr[j] = new thread(writeThread, &wal, &countArr[j], j, argc, argv);
    }
    threadArr[thread_count] = new thread(calcThread, countArr, thread_count, argc, argv);

    for (int k = 0; k < thread_count+1; ++k) {
        threadArr[k]->join();
    }
#else

    pthread_t threadArr[WRITE_THREAD_N+1];
    WriteThreadArg writeThreadArgArr[WRITE_THREAD_N+1];
    int countArr[WRITE_THREAD_N];
    for (int j = 0; j < thread_count; ++j) {
        countArr[j] = 0;
//        threadArr[j] = new thread(writeThread, &wal, &countArr[j], j, argc, argv);
        writeThreadArgArr[j].wal = &wal;
        writeThreadArgArr[j].count = &countArr[j];
        writeThreadArgArr[j].index = j;
        writeThreadArgArr[j].argc = argc;
        writeThreadArgArr[j].argv = argv;
        pthread_create(&threadArr[j], NULL, writeThreadEntry, &writeThreadArgArr[j]);
    }
    thread *calcThread_ = new thread(calcThread, countArr, thread_count, argc, argv);

    for (int k = 0; k < thread_count; ++k) {
//        threadArr[k]->join();
        pthread_join(threadArr[k], NULL);
    }
#endif

    return 0;
}