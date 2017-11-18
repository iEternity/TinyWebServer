//
// Created by zhangkuo on 17-11-18.
//
#include <xnet/base/Thread.h>
#include <xnet/base/BlockingQueue.h>
#include <xnet/base/CountDownLatch.h>
#include <xnet/base/CurrentThread.h>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

using namespace xnet;
using namespace std;

class Test
{
    using ThreadPtr = std::unique_ptr<Thread>;
public:
    Test(int numThreads):
        latch_(numThreads)
    {
        threads_.reserve(numThreads);
        for(int i=0; i<numThreads; i++)
        {
            char name[32];
            snprintf(name, sizeof name, "work thread %d", i);
            ThreadPtr thread(new Thread(std::bind(&Test::threadFunc, this), name));
            threads_.push_back(std::move(thread));
        }

        for_each(threads_.begin(), threads_.end(), std::bind(&Thread::start, std::placeholders::_1));
    }



    void run(int times)
    {
        latch_.wait();

        for(int i=0; i<times; i++)
        {
            std::string data("hello"+ std::to_string(i));

            queue_.put(data);
            printf("tid = %d, put data: %s, queue size = %zd\n", CurrentThread::tid(), data.data(), queue_.size());
        }
    }

    void joinAll()
    {
        for(int i=0; i<queue_.size(); i++)
        {
            queue_.put("stop");
        }

        for_each(threads_.begin(), threads_.end(), std::bind(&Thread::join, std::placeholders::_1));
    }
private:
    void threadFunc()
    {
        printf("tid = %d, %s started\n", CurrentThread::tid(), CurrentThread::name());

        latch_.countDown();

        bool running = true;
        while(running)
        {
            std::string data(queue_.take());
            printf("tid = %d, get data: %s, queue size = %zd\n",
                   xnet::CurrentThread::tid(),
                   data.data(),
                   queue_.size()
            );
            running = (data != "stop");
        }

        printf("tid = %d, %s stop\n", CurrentThread::tid(), CurrentThread::name());
    }

private:
    std::vector<ThreadPtr> threads_;
    BlockingQueue<std::string> queue_;
    CountDownLatch latch_;
};

int main()
{
    Test test(4);
    test.run(100);
    test.joinAll();
}