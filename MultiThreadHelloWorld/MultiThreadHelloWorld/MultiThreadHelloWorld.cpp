// MultiThreadHelloWorld.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <thread> // std::thread / std::this_thread::sleep_for

void do_something()
{
}

class background_task
{
public:
	void operator()() const
	{
		do_something();
	}
};

int main()
{
    std::cout << "MultiThread Hello World!\n";

	// 创建
	std::thread my_thread0; // 创建thread对象, 但是并不会创建子线程
	std::thread my_thread1(do_something); // 创建子线程方式一, 用一个Callable可调用的函数
	background_task f;
	std::thread my_thread2( f ); // 创建子线程方式2, 用一个Callable可调用的对象

	//等待线程结束
	//如果不等待子线程结束, 会报错"abort() has been called", 子线程使用的资源都随着主线程结束消失了
	//如果不等待线程，就必须保证线程结束之前，可访问的数据得有效性
	//my_thread0.join(); // error: 不能join一个没有开始的线程
	my_thread1.join(); // 加入式
	my_thread2.join(); // 分离式

	getchar();
}
