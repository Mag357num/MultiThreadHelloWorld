#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

// 该程序用于展示死锁的常见发生情况
// https ://zhuanlan.zhihu.com/p/343539779
mutex mt1;
mutex mt2;
void thread1()
{
	cout << "thread1 begin" << endl;
	lock_guard<mutex> guard1( mt1 );
	this_thread::sleep_for( chrono::seconds( 1 ) );
	lock_guard<mutex> guard2( mt2 );
	cout << "hello thread1" << endl;
}
void thread2()
{
	cout << "thread2 begin" << endl;
	lock_guard<mutex> guard1( mt2 );
	this_thread::sleep_for( chrono::seconds( 1 ) );
	lock_guard<mutex> guard2( mt1 );
	cout << "hello thread2" << endl;
}

int main()
{
	thread t1( thread1 );
	thread t2( thread2 );
	t1.join();
	t2.join();
	cout << "thread end" << endl;
	return 0;
}