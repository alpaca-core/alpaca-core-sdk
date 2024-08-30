#include <xec/TaskExecutor.hpp>
#include <xec/ThreadExecution.hpp>
#include <astl/move_capture.hpp>
#include <astl/move.hpp>
#include <string>
#include <cassert>
#include <utility>
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
        return std::exchange(m_handle, nullptr);
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

template <typename T>
struct WrapCallbackAwaitable {
    std::mutex m_mutex;
    CoTask::Handle m_handle = nullptr;
    std::optional<T> m_value;

    std::function<void(T)> getCallback() {
        return [this](T value) {
            std::lock_guard lock(m_mutex);
            m_value = astl::move(value);
            if (!m_handle) return;
            m_handle.promise().m_executor->pushTask([h = m_handle]() {
                h.resume();
            });
        };
    }

    bool await_ready() const noexcept { return false; }
    bool await_suspend(CoTask::Handle handle) {
        std::lock_guard lock(m_mutex);
        if (m_value.has_value()) {
            return false;
        }
        else {
            m_handle = handle;
            return true;
        }
    }
    T await_resume() noexcept {
        assert(m_value);
        return astl::move(*m_value);
    }
};

template <typename T>
struct CallbackAwaitable {
    using FF = std::function<void(std::function<void(T)>)>;
    FF m_producer;
    std::optional<T> m_value;

    CallbackAwaitable(FF producer) : m_producer(astl::move(producer)) {}

    bool await_ready() const noexcept { return false; }
    void await_suspend(CoTask::Handle handle) {
        m_producer([this, handle](T value) {
            m_value = astl::move(value);
            handle.promise().m_executor->pushTask([handle]() {
                handle.resume();
            });
        });
    }
    T await_resume() noexcept {
        assert(m_value);
        return *m_value;
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
    std::cout << "test start\n";

    auto a = co_await IntAwaitable(p);
    std::cout << "a: " << a << "\n";

    WrapCallbackAwaitable<std::string> ab;
    p.produceString(ab.getCallback());
    auto b = co_await ab;
    std::cout << "b: " << b << "\n";

    auto c = co_await CallbackAwaitable<std::string>([&p](auto cb) {
        p.produceString(cb);
    });
    std::cout << "c: " << c << "\n";

    std::cout << "test end\n";
}

int main() {
    Producer producer;
    Consumer consumer(producer);

    consumer.co_splice(test(producer));

    std::cout << "start\n";
    consumer.run();
    return 0;
}
