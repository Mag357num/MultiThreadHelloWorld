/*
作者：徐飞
链接：https ://zhuanlan.zhihu.com/p/107092432
来源：知乎
著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。
*/

// 本demo用于展示：两个线程就算只竞争一个变量, 也会因为竞争造成不符合预期的结果, 使用std::atomic解决这个问题的正面例子见Atomic02

#include <iostream>
#include <thread>
using namespace std;
static long long total = 0;

void func()
{
    long long i;
    for (i = 0; i < 100000000LL; i++)
    {
        total += i;
    }
}

int main()
{
    std::thread thread1( func );
    std::thread thread2( func );
    thread1.join();
    thread2.join();
    cout << total << endl;//9999999900000000
    return 0;
}