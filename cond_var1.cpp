#include <iostream>
#include <deque>
#include <thread>
#include <mutex>

std::deque<int> q;						//双端队列标准容器全局变量
std::mutex mu;							//互斥锁全局变量
//生产者，往队列放入数据
void function_1() {
    int count = 10;
    while (count > 0) {
        std::unique_lock<std::mutex> locker(mu);
        q.push_front(count);			//数据入队锁保护
        locker.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(1));		//延时1秒
        count--;
    }
}
//消费者，从队列提取数据
void function_2() {
    int data = 0;
    while ( data != 1) {
        std::unique_lock<std::mutex> locker(mu);
        if (!q.empty()) {			//判断队列是否为空
            data = q.back();
            q.pop_back();			//数据出队锁保护
            locker.unlock();
            std::cout << "t2 got a value from t1: " << data << std::endl;
        } else {
            locker.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));		//延时500毫秒
        }
    }
}

int main() {
    std::thread t1(function_1);
    std::thread t2(function_2);
    t1.join();
    t2.join();

    getchar();
    return 0;
}