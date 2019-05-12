#include <vector>
#include <thread>
#include <future>
#include <numeric>
#include <iostream>
#include <chrono>
 
int accumulate(std::vector<int>::iterator first,
                std::vector<int>::iterator last)
{
    int sum = std::accumulate(first, last, 0);
    return sum;
}
 
int main()
{
    // 演示用 packaged_task 在线程间传递结果。
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };
    std::packaged_task<int(std::vector<int>::iterator,std::vector<int>::iterator)> accumulate_task(accumulate);
    std::future<int> accumulate_future = accumulate_task.get_future();
    std::thread work_thread(std::move(accumulate_task), numbers.begin(), numbers.end());
    accumulate_future.wait();  //等待结果
    std::cout << "result=" << accumulate_future.get() << '\n';
    work_thread.join();  //阻塞等待线程执行完成
 
    getchar();
    return 0;
}