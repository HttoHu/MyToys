#include <mutex>
#include <queue>
#include <condition_variable>

namespace Task
{
    template <typename T>
    class TSqueue
    {
    public:
        void push(T value) {
            std::lock_guard<std::mutex> lk(mut);
            data_queue.push(value);
            data_cond.notify_one();
        }
        void wait_and_pop(T &value) {
            std::unique_lock<std::mutex> lk(mut);
            data_cond.wait(lk,[this]{
                return !data_queue.empty();
            });
            value = data_queue.front();
            data_queue.pop();
        }

    private:
        std::mutex mut;
        std::queue<T> data_queue;
        std::condition_variable data_cond;
    };
}