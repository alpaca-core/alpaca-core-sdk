#include <xec/TaskExecutor.hpp>
#include <xec/ThreadExecution.hpp>
#include <astl/move_capture.hpp>
#include <string>
#include <iostream>
#include <coroutine>

class Producer : public xec::TaskExecutor {
    xec::ThreadExecution m_execution;

    int m_counter = 42;
public:
    Producer() : m_execution(*this) {
        m_execution.launchThread("producer");
    }

    void produceInt(std::function<void(int)> cb) {
        pushTask([this, movecap(cb)]() mutable {
            cb(m_counter);
            ++m_counter;
        });
    }

    void produceString(std::function<void(std::string)> cb) {
        pushTask([this, movecap(cb)]() mutable {
            cb("hello " + std::to_string(m_counter));
            ++m_counter;
        });
    }
};

struct CoTask {
    struct promise_type {
        xec::TaskExecutor* m_executor = nullptr;

        CoTask get_return_object() noexcept {
            return CoTask{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        void unhandled_exception() noexcept { /* terminate? */ }

        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }

        void return_void() noexcept {}
    };
    using Handle = std::coroutine_handle<promise_type>;

    CoTask(CoTask&& other) noexcept : m_handle(std::exchange(other.m_handle, nullptr)) {}
    ~CoTask() {
        if (m_handle) {
            m_handle.destroy();
        }
    }

    Handle take_handle() noexcept {
        auto h = m_handle;
        m_handle = nullptr;
        return h;
    }
private:
    Handle m_handle;
    CoTask(Handle handle) noexcept : m_handle(handle) {}
};

class Consumer : public xec::TaskExecutor {
    xec::LocalExecution m_execution;
    Producer& m_producer;
public:
    Consumer(Producer& producer) : m_execution(*this), m_producer(producer) {}

    void co_splice(CoTask task) {
        pushTask([this, h = task.take_handle()]() {
            h.promise().m_executor = this;
            h.resume();
        });
    }

    void run() {
        m_execution.run();
    }
};

struct IntAwaitable {
    Producer& m_producer;
    int m_producedValue;
    IntAwaitable(Producer& producer) : m_producer(producer) {};

    bool await_ready() const noexcept { return false; }
    void await_suspend(CoTask::Handle handle) {
        m_producer.produceInt([this, handle](int i) {
            m_producedValue = i;
            handle.promise().m_executor->pushTask([handle]() {
                handle.resume();
            });
        });
    }
    int await_resume() { return m_producedValue; }
};

CoTask test(Producer& p) {
    std::cout << "test\n";
    int a = co_await IntAwaitable(p);
    std::cout << "a: " << a << "\n";
    int b = co_await IntAwaitable(p);
    std::cout << "b: " << b << "\n";
}

int main() {
    Producer producer;
    Consumer consumer(producer);

    consumer.co_splice(test(producer));

    std::cout << "start\n";
    consumer.run();
    return 0;
}
