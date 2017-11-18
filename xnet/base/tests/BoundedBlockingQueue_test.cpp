//
// Created by zhangkuo on 17-11-18.
//
#include <vector>
#include <algorithm>
#include <xnet/base/BoundedBlockingQueue.h>
#include <xnet/base/Thread.h>
#include <xnet/base/CountDownLatch.h>
#include <xnet/base/CurrentThread.h>
using namespace std;
using namespace xnet;

class Test
{
    using ThreadPtr = std::unique_ptr<Thread>;
public:
    Test(int numThreads):
        queue_(10),
        latch_(numThreads)
    {
        for(int i=0; i<numThreads; i++)
        {
            threads_.emplace_back(new Thread(std::bind(&Test::threadFunc, this), "work thread " + to_string(i)));
        }

        for_each(threads_.begin(), threads_.end(), std::bind(&Thread::start, placeholders::_1));
    }

    void run(int times)
    {
        latch_.wait();

        for(int i=0; i<times; i++)
        {
            string data = "I love cpp! - " + to_string(i);
            queue_.put(data);

            printf("tid = %d, put data: %s, queue size: %zd\n",
                   CurrentThread::tid(),
                   data.data(),
                   queue_.size());
        }
    }

    void joinAll()
    {
        for(int i=0; i<threads_.size(); ++i)
        {
            queue_.put("stop");
        }

        for_each(threads_.begin(), threads_.end(), std::bind(&Thread::join, placeholders::_1));
    }

private:
    void threadFunc()
    {
        printf("tid = %d, %s started...\n", CurrentThread::tid(), CurrentThread::name());
        latch_.countDown();

        bool running = true;
        while(running)
        {
            string data(queue_.take());

            printf("tid = %d, get data: %s, queue size: %zd\n",
                   CurrentThread::tid(),
                   data.data(),
                   queue_.size());
            running = (data != "stop");
        }

        printf("tid = %d, %s stop...\n", CurrentThread::tid(), CurrentThread::name());
    }

private:
    BoundedBlockingQueue<string>    queue_;
    vector<ThreadPtr>               threads_;
    CountDownLatch                  latch_;
};



int main()
{
    Test t(5);
    t.run(1000);
    t.joinAll();
}