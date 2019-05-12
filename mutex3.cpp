#include <chrono>
#include <mutex>
#include <thread>
#include <iostream> 

std::chrono::milliseconds interval(100);
 
std::timed_mutex tmutex;

int job_shared = 0; //两个线程都能修改'job_shared',mutex将保护此变量
int job_exclusive = 0; //只有一个线程能修改'job_exclusive',不需要保护

//此线程只能修改 'job_shared'
void job_1()
{
    std::lock_guard<std::timed_mutex> lockg(tmutex);    //获取RAII智能锁，离开作用域会自动析构解锁
    std::this_thread::sleep_for(5 * interval);  //令‘job_1’持锁等待
    ++job_shared;
    std::cout << "job_1 shared (" << job_shared << ")\n";
}

// 此线程能修改'job_shared'和'job_exclusive'
void job_2()
{
    while (true) {    //无限循环，直到获得锁并修改'job_shared'
        std::unique_lock<std::timed_mutex> ulock(tmutex,std::defer_lock);   //创建一个智能锁但先不锁定
        //尝试获得锁成功则修改'job_shared'
        if (ulock.try_lock_for(3 * interval)) {     //在3个interval时间段内尝试获得锁
            ++job_shared;
            std::cout << "job_2 shared (" << job_shared << ")\n";
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