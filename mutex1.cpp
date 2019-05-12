#include <chrono>
#include <mutex>
#include <thread>
#include <iostream> 

std::chrono::milliseconds interval(100);
 
std::mutex mutex;
int job_shared = 0; //两个线程都能修改'job_shared',mutex将保护此变量
int job_exclusive = 0; //只有一个线程能修改'job_exclusive',不需要保护

//此线程只能修改 'job_shared'
void job_1()
{
    mutex.lock();
    std::this_thread::sleep_for(5 * interval);  //令‘job_1’持锁等待
    ++job_shared;
    std::cout << "job_1 shared (" << job_shared << ")\n";
    mutex.unlock();
}

// 此线程能修改'job_shared'和'job_exclusive'
void job_2()
{
    while (true) {    //无限循环，直到获得锁并修改'job_shared'
        if (mutex.try_lock()) {     //尝试获得锁成功则修改'job_shared'
            ++job_shared;
            std::cout << "job_2 shared (" << job_shared << ")\n";
            mutex.unlock();
            return;
        } else {      //尝试获得锁失败,接着修改'job_exclusive'
            ++job_exclusive;
            std::cout << "job_2 exclusive (" << job_exclusive << ")\n";
            std::this_thread::sleep_for(interval);
        }
    }
}

int main() 
{
    std::thread thread_1(job_1);
    std::thread thread_2(job_2);
 
    thread_1.join();
    thread_2.join();

    getchar();
    return 0;
}