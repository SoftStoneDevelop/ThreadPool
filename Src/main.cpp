#pragma once

#include <iostream>
#include <stdio.h>

#include "ThreadPool.hpp"

void routine(int sec)
{
    std::this_thread::sleep_for(std::chrono::seconds(sec));
    printf("Bong\n");
}

int main(int argc, char* argv[])
{
    try
    {
        {
            ThreadPool::ThreadPool pool(1);//create hread
            pool.enqueue(routine, 60);//nothing
            pool.enqueue(routine, 60);//create hread
            pool.enqueue(routine, 10);//create hread
            pool.enqueue(routine, 10);//create hread
            std::this_thread::sleep_for(std::chrono::seconds(12));
            pool.enqueue(routine, 5);//nothing
            pool.enqueue(routine, 5);//nothing
            std::this_thread::sleep_for(std::chrono::seconds(50));
            //delete 2 threads
            pool.enqueue(routine, 5);//nothing
            pool.enqueue(routine, 5);//nothing
            //only 4 threads created
            std::this_thread::sleep_for(std::chrono::seconds(50));
            //delete 1 threads
            //1 thread live in pool - because is minimal size
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}