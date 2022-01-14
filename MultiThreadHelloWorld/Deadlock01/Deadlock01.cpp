#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

/* 该程序用于展示死锁的常见发生情况
    死锁的必要条件
        1. 互斥条件：
            某资源只能被一个进程使用，其它进程请求该资源时，只能等待 ，直到资源使用完毕后释放资源
        2. 请求和保持条件：
            程序己经保持了至少一个资源，但是又提出了新的要求，而这个资源被其它进程占用， 自己占用资源却保持不放
        3. 不可抢占条件：
            进程己获得的资源没有使用完，不能被抢占
        4. 循环等待条件：
            必然存在一个循环链
    解决死锁
        1. 对不同mutex上锁时保持同一顺序
        2. 使用std::lock+std::adopt_lock, 见Deadlock02
*/
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