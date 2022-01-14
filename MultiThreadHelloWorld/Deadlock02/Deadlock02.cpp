#include <mutex>
#include <thread>

// 该demo用于表示如何使用 std::lock std::adopt_lock std::defer_lock三者来防止死锁

struct bank_account
{
    explicit bank_account( int balance ) : balance( balance ) { }
    int balance;
    std::mutex m;
};

void transfer( bank_account& from, bank_account& to, int amount )
{
    if (&from == &to) return; // avoid deadlock in case of self transfer

    std::lock( from.m, to.m );  // lock both mutexes without deadlock

    // 添加std::adopt_lock作为参数表示这个锁是收养来的, 不会在创建lock1时给from.m这个mutex上锁, 但是会在其析构时为其解锁
    // make sure both already-locked mutexes are unlocked at the end of scope
    std::lock_guard<std::mutex> lock1( from.m, std::adopt_lock );
    std::lock_guard<std::mutex> lock2( to.m, std::adopt_lock );

    // std::defer_lock延迟锁和收养锁类似
    // equivalent approach:
    // std::unique_lock<std::mutex> lock1(from.m, std::defer_lock);
    // std::unique_lock<std::mutex> lock2(to.m, std::defer_lock);
    // std::lock(lock1, lock2);

    from.balance -= amount;
    to.balance += amount;
}

int main()
{
    bank_account my_account( 100 );
    bank_account your_account( 50 );

    std::thread t1( transfer, std::ref( my_account ), std::ref( your_account ), 10 );
    std::thread t2( transfer, std::ref( your_account ), std::ref( my_account ), 5 );

    t1.join();
    t2.join();
}