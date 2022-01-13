#include <iostream>                // std::cout
#include <thread>                // std::thread
#include <mutex>                // std::mutex, std::unique_lock
#include <condition_variable>    // std::condition_variable

std::mutex mtx; // 全局互斥锁.
std::condition_variable cv; // 全局条件变量.
bool ready = false; // 全局标志位.

void do_print_id(int id)
{
	std::unique_lock<std::mutex> lck(mtx); // 用同一个互斥锁来给条件变量对应的线程上锁
	if (!ready) // 如果标志位不为 true, 则等待...
	{
		cv.wait(lck);
	} // 当前线程被阻塞
	
	// 当全局标志位变为 true 之后, 线程被唤醒, 继续往下执行打印线程编号id.
	std::cout << "thread " << id << '\n';
}

void go()
{
	//std::unique_lock <std::mutex> lck(mtx);
	ready = true; // 设置全局标志位为 true.
	cv.notify_all(); // 唤醒所有线程. 所有线程之间产生竞争, 先后顺序无法确定
}

int main()
{
	std::thread threads[10]; // spawn 10 threads

	for (int i = 0; i < 10; ++i)
	{
		threads[i] = std::thread( do_print_id, i ); // 使用函数do_print_id和其参数i初始化线程
	}
	std::cout << "10 threads ready to race...\n";

	go(); // go!

	for (auto& th : threads)
	{
		th.join(); // 阻塞, 等待所有线程结束
	}

	getchar();
	return 0;
}