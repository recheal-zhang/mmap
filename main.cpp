#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>

#include "Md5.h"

using namespace std;

int main(){
    cout << "hello mmap" << endl;

    cout << "sysconf(_SC_PAGE_SIZE) : "
        << sysconf(_SC_PAGE_SIZE) << endl;

    const long SLICESIZE = 1024 * 1024;

    string fileRead = "./ref/victory";
    string fileWrite = "./he";

    //------------------------------------------------------------
    //open fileRead
    int fdRead = open(fileRead.c_str(), O_RDONLY, (mode_t)0600);
    if(fdRead == -1){
        cout << "open read file error" << endl;
        exit(EXIT_FAILURE);
    }
    //get fileRead size
    struct stat readFileInfo = {0};
    if(fstat(fdRead, &readFileInfo) == -1){
        cout << "get read file info error" << endl;
        exit(EXIT_FAILURE);
    }
    if(readFileInfo.st_size == 0){
        cout << "read file is empty" << endl;
        exit(EXIT_FAILURE);
    }

    //cacultate slice num
    long readFileSize = static_cast<long>(readFileInfo.st_size);
    long sliceNum = readFileSize / SLICESIZE;
    if(readFileSize % SLICESIZE){
        sliceNum += 1;
    }

    //write file
    //-----------------------------------------------------------
    int fdWrite = open(fileWrite.c_str(),
            O_RDWR | O_CREAT | O_TRUNC,
            (mode_t)0600);
    if(fdWrite == -1){
        cout << "open write file error" << endl;
        exit(EXIT_FAILURE);
    }
    //get writeFileSize
    long writeFileSize = readFileSize;
    if(lseek(fdWrite, writeFileSize - 1, SEEK_SET) == -1){
        close(fdWrite);
        cout << "lseek to strech the file error" << endl;
        exit(EXIT_FAILURE);
    }
    //check if it can write
    if(write(fdWrite, "", 1) == -1){
        close(fdWrite);
        cout << "write check error" << endl;
        exit(EXIT_FAILURE);
    }
    //mmap
    void *map =
        mmap(0, writeFileSize,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fdWrite, 0);
    char *mapWriteFile = static_cast<char*>(map);
    if(mapWriteFile == MAP_FAILED){
        close(fdWrite);
        cout << "map write file error" << endl;
        exit(EXIT_FAILURE);
    }
    //lseek & read & write to fileWrite
    char buf[SLICESIZE];
    int countIfSyncDisk = 0;
    for(long i = 0; i < sliceNum; i++){
        //read buf
        lseek(fdRead, i * SLICESIZE, SEEK_SET);
        int nread = read(fdRead, buf, SLICESIZE);
        if(nread == -1){
            cout << "read error" << endl;
        }
        //write buf
        memcpy(mapWriteFile + i * SLICESIZE,
                buf,
                nread);
        ++countIfSyncDisk;
        if(countIfSyncDisk == 512){
            if(msync(mapWriteFile, writeFileSize, MS_SYNC) == -1){
                cout << "cannot sync the file to disk" << endl;
            }
            countIfSyncDisk = 0;
        }
    }
    if(msync(mapWriteFile, writeFileSize, MS_SYNC) == -1){
        cout << "cannot sync the file to disk" << endl;
    }


    //close & unmap
    close(fdRead);
    if(munmap(mapWriteFile, writeFileSize) == -1){
        close(fdWrite);
        cout << "unmap write file error" << endl;
        exit(EXIT_FAILURE);
    }
    close(fdWrite);

    cout << "read file Md5 : "
        << md5file(fileRead.c_str()) <<endl;

    cout << "write file Md5 : "
        << md5file(fileWrite.c_str()) << endl;

    return 0;
}
