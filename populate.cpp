#include <chrono>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(int argc, char** argv){
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    srand(439); 

    // you need some file, you can change the name / permissions though
    // the return type of open is a bit different from our Shared<Node>
    // from p5. How can you possibly represent a file in an integer?
    //
    // This is called a file descriptor. You'll learn more about it later, 
    // but basically this number corresponds to the file that you care about.
    // The OS has a special map, so now if you call read / write / mmap using
    // this file descriptor, the OS knows which file you actually wanted to
    // read / write / mmap

    int fd = open("data", O_RDWR);
    struct stat buf;
    fstat(fd, &buf);
    off_t size = buf.st_size;

    int thing = 0;
    REMAP:
    void* addr;

    // easy / hacky way to pass cmdline args
    if (argc == 1){
        // do demand paging if no additional args
        // printf("Demand paging\n");
        addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    }
    else{
        // load all at once if additional args
        // printf("Map populate\n");
        addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, 0);
    }

    // Always check your return values!
    if(addr == MAP_FAILED){
        printf("Oh dear, something went wrong with mmap()! %s\n", strerror(errno));    
    }

    // TODO: run benchmark here
    char* data = (char*) addr;
    volatile char c;
    for (int j = 0; j < size / 0x1000; j++){
        c = data[j * 0x1000] + 1;
    }
    thing++;
    if (thing < 5000){
        goto REMAP;
    }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;


    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "finished computation at " << std::ctime(&end_time) << "elapsed time: " << elapsed_seconds.count() << "s\n";
    

    munmap(addr, 1);
}

