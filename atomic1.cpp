#include <chrono>
#include <atomic>
#include <thread>
#include <iostream> 

std::chrono::milliseconds interval(100);

std::atomic<bool> readyFlag(false);     //原子布尔类型，取代互斥量
std::atomic<int> job_shared(0); //两个线程都能修改'job_shared',将该变量特化为原子类型
int job_exclusive = 0; //只有一个线程能修改'job_exclusive',不需要保护

//此线程只能修改 'job_shared'
void job_1()
{   
    std::this_thread::sleep_for(5 * interval);
    job_shared.fetch_add(1);
    std::cout << "job_1 shared (" << job_shared.load() << ")\n";
    readyFlag.store(true);      //改变布尔标记状态为真
}

// 此线程能修改'job_shared'和'job_exclusive'
void job_2()
{
    while (true) {    //无限循环，直到可访问并修改'job_shared'
        if (readyFlag.load()) {     //判断布尔标记状态是否为真，为真则修改‘job_shared’
            job_shared.fetch_add(1);
            std::cout << "job_2 shared (" << job_shared.load() << ")\n";
            return;
        } else {      //布尔标记为假,则修改'job_exclusive'
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