#include <chrono>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <sys/mman.h>

#define LARGE 400000000
#define MAP_HUGE_2MB    (21 << MAP_HUGE_SHIFT)

int main(int argc, char** argv){
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    srand(439); 

    // No file for this one, our Linux kernel doesn't have support for file
    // backed huge pages. This will be like our naive_mmap(nullptr, length)
    // What is the flag called on Linux?

    void* addr;
    int size = 0x400000;

    // easy / hacky way to pass cmdline args
    if (argc == 1){
        // use 4k page size if no additional args
        std::cout << "Page size: 4k\n";
        addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    }
    else{
        // use 2MB page size if additional args
        // Note reserving huge pages requires some setup before you actually
        // run the program. You need special permissions which the CS
        // department was nice enough to give us for the day :D
        //
        // The superpages are already reserved, so you should be able to mmap
        // them as long as one is available (you might have to wait a few
        // seconds if other students are using all of the available superpages).
        //
        // If you want to learn more about this special setup, check out 
        // https://docs.kernel.org/admin-guide/mm/hugetlbpage.html
        std::cout << "Page size: 2MB\n";
        addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    }

    // Always check your return values!
    if(addr == MAP_FAILED){
        std::cout << "Oh dear, something went wrong with mmap()! " << strerror(errno) << "\n";    
    }

    // TODO: run benchmark here

    for (int j = 0; j < size / 0x1000; j++){
        ((char*) addr)[j * 0x1000] = 1;
    }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;


    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "finished computation at " << std::ctime(&end_time) << "elapsed time: " << elapsed_seconds.count() << "s\n";
    munmap(addr, 1);
}

