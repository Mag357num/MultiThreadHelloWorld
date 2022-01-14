#include <iostream>
#include <windows.h>
#include <PROCESS.H>

using namespace std;

/*
临界区（Critical Section） 保证在某一时刻只有一个线程能访问数据的简便办法。在任意时刻只允许一个线程对共享资源进行访问。
如果有多个线程试图同时访问临界区，那么在有一个线 程进入后其他所有试图访问此临界区的线程将被挂起，并一直持续到进入临界区的线程离开。
临界区在被释放后，其他线程可以继续抢占，并以此达到用原子方式操 作共享资源的目的。
临界区包含两个操作原语： EnterCriticalSection（） 进入临界区 LeaveCriticalSection（） 离开临界区 EnterCriticalSection（）
语句执行后代码将进入临界区以后无论发生什么，必须确保与之匹配的 LeaveCriticalSection（）都能够被执行到。否则临界区保护的共享资源将永远不会被释放。
虽然临界区同步速度很快，但却只能用来同步本 进程内的线程，而不可用来同步多个进程中的线程。

使用临界区的步骤
1. 申请一个临界区变量  CRITICAL_SECTION gSection;
2. 初始化临界区  InitializeCriticalSection( &gSection );
3. 使用临界区 EnterCriticalSection( &gSection ); ..省略代码..LeaveCriticalSection( &gSection );
4. 释放临界区 DeleteCriticalSection( &gSection );
*/

//用一个CRITICAL_SECTION 结构体和需要同步的数据关联起来
typedef struct tagSafePtr
{
    int* pInt;
    CRITICAL_SECTION cs;
}StSafePtr, * PStSafePtr;

CRITICAL_SECTION gSection;

class auto_lock
{
public:
    auto_lock( PCRITICAL_SECTION pcs )
    {
        _pcs = pcs;
        EnterCriticalSection( _pcs );
    }
    ~auto_lock()
    {
        LeaveCriticalSection( _pcs );
    }
private:
    PCRITICAL_SECTION _pcs;
};
unsigned _stdcall thread1( void* ptr )
{
    EnterCriticalSection( &gSection );
    cout << "thread 1" << endl;
    Sleep( 2000 );
    PStSafePtr pSafePtr = (PStSafePtr) ptr;
    LeaveCriticalSection( &gSection );
    if (pSafePtr)
    {
        //在获取被同步的数据前，要先进入临界区，保证同时只有一个线程访问
        //EnterCriticalSection(&(pSafePtr->cs));
        auto_lock m( &pSafePtr->cs );
        Sleep( 10 );
        delete pSafePtr->pInt;
        pSafePtr->pInt = 0;
        cout << "delete ptr!" << endl;
        //多线程任务退出临界区
        //LeaveCriticalSection(&(pSafePtr->cs));
    }
    return 0;
}

unsigned _stdcall thread2( void* ptr )
{
    EnterCriticalSection( &gSection );
    cout << "therad 2" << endl;
    Sleep( 2000 );
    PStSafePtr pSafePtr = (PStSafePtr) ptr;
    LeaveCriticalSection( &gSection );
    if (pSafePtr)
    {
        //在获取被同步的数据前，先要进入临界区，保证同一时刻只有一个线程访问
        //EnterCriticalSection(&(pSafePtr->cs));
        // 利用类 进入临界区 
        auto_lock m( &pSafePtr->cs );
        int* p = pSafePtr->pInt;
        if (p)
        {
            *p = 100;
            cout << *p << endl;
        }
        else
        {
            cout << " p is null.." << endl;
        }
        //任务结束，退出临界区
        //LeaveCriticalSection(&(pSafePtr->cs));
    }
    return 0;
}

int main()
{
    StSafePtr st;
    //初始化临界区
    InitializeCriticalSection( &st.cs );
    InitializeCriticalSection( &gSection );
    st.pInt = new int( 10 );
    HANDLE h1 = (HANDLE) _beginthreadex( 0, 0, thread1, &st, 0, 0 );
    HANDLE h2 = (HANDLE) _beginthreadex( 0, 0, thread2, &st, 0, 0 );
    ::WaitForSingleObject( h1, INFINITE );
    CloseHandle( h1 );
    ::WaitForSingleObject( h2, INFINITE );
    CloseHandle( h2 );
    delete st.pInt;
    //清理资源
    DeleteCriticalSection( &st.cs );
    DeleteCriticalSection( &gSection );
    return 0;
}