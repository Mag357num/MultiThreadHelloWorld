#include<iostream>
#include<thread>
#include<mutex>
#include<queue>
#include<condition_variable>

using namespace std;

//缓冲区存储的数据类型 
struct CacheData
{
    //商品id 
    int id;
    //商品属性 
    string data;
};

queue<CacheData> Q;
//缓冲区最大空间 
const int MAX_CACHEDATA_LENGTH = 10;
//互斥量，生产者之间，消费者之间，生产者和消费者之间，同时都只能一个线程访问缓冲区 
mutex m;
condition_variable condConsumer;
condition_variable condProducer;
//全局商品id 
int ID = 1;

//消费者动作 
void ConsumerActor()
{
    unique_lock<mutex> lockerConsumer( m );
    cout << "[" << this_thread::get_id() << "] 获取了锁" << endl;
    while (Q.empty())
    {
        cout << "因为队列为空，所以消费者Sleep" << endl;
        cout << "[" << this_thread::get_id() << "] 不再持有锁" << endl;
        //队列空， 消费者停止，等待生产者唤醒 
        condConsumer.wait( lockerConsumer );
        cout << "[" << this_thread::get_id() << "] Weak, 重新获取了锁" << endl;
    }
    cout << "[" << this_thread::get_id() << "] ";
    CacheData temp = Q.front();
    cout << "- ID:" << temp.id << " Data:" << temp.data << endl;
    Q.pop();
    condProducer.notify_one();
    cout << "[" << this_thread::get_id() << "] 释放了锁" << endl;
}

//生产者动作 
void ProducerActor()
{
    unique_lock<mutex> lockerProducer( m );
    cout << "[" << this_thread::get_id() << "] 获取了锁" << endl;
    while (Q.size() > MAX_CACHEDATA_LENGTH) // 条件变量对应的条件.
    {
        cout << "因为队列为满，所以生产者Sleep" << endl;
        cout << "[" << this_thread::get_id() << "] 不再持有锁" << endl;
        //对列慢，生产者停止，等待消费者唤醒 
        condProducer.wait( lockerProducer ); // 这里也可以去掉上面的while, 换成用wait(locker，Predicate)这个函数
        cout << "[" << this_thread::get_id() << "] Weak, 重新获取了锁" << endl;
    }
    cout << "[" << this_thread::get_id() << "] ";
    CacheData temp;
    temp.id = ID++;
    temp.data = "*****";
    cout << "+ ID:" << temp.id << " Data:" << temp.data << endl;
    Q.push( temp );
    condConsumer.notify_one();
    cout << "[" << this_thread::get_id() << "] 释放了锁" << endl;
}

//消费者 
void ConsumerTask()
{
    while (1)
    {
        ConsumerActor();
    }
}

//生产者 
void ProducerTask()
{
    while (1)
    {
        ProducerActor();
    }
}

//管理线程的函数 
void Dispatch( int ConsumerNum, int ProducerNum )
{
    vector<thread> thsC;
    for (int i = 0; i < ConsumerNum; ++i)
    {
        thsC.push_back( thread( ConsumerTask ) );
    }

    vector<thread> thsP;
    for (int j = 0; j < ProducerNum; ++j)
    {
        thsP.push_back( thread( ProducerTask ) );
    }

    for (int i = 0; i < ConsumerNum; ++i)
    {
        if (thsC[i].joinable())
        {
            thsC[i].join();
        }
    }

    for (int j = 0; j < ProducerNum; ++j)
    {
        if (thsP[j].joinable())
        {
            thsP[j].join();
        }
    }
}

int main()
{
    //一个消费者线程，5个生产者线程，则生产者经常要等待消费者 
    Dispatch( 1, 5 );
    return 0;
}