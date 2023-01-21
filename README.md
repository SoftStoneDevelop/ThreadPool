# ThreadPool
A simple template of thread pool.


```C++
void routine(int sec)
{
    std::this_thread::sleep_for(std::chrono::seconds(sec));
    printf("Bong\n");
}

ThreadPool::ThreadPool pool(1);//create hread with 1 thread
pool.enqueue(routine, 60);//nothing
pool.enqueue(routine, 60);//create new thread in pool(Thread count == 2)
pool.enqueue(routine, 10);//create new thread in pool(Thread count == 3)
pool.enqueue(routine, 10);//create new thread in pool(Thread count == 4)
std::this_thread::sleep_for(std::chrono::seconds(12));
pool.enqueue(routine, 5);//nothing
pool.enqueue(routine, 5);//nothing
std::this_thread::sleep_for(std::chrono::seconds(50));
//delete 2 threads from pool(Thread count == 2)
pool.enqueue(routine, 5);//nothing
pool.enqueue(routine, 5);//nothing
std::this_thread::sleep_for(std::chrono::seconds(50));
//delete 1 threads
//1 thread live in pool - because is minimal size
std::this_thread::sleep_for(std::chrono::seconds(10));

```
