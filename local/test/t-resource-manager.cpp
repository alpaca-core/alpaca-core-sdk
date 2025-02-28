// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <doctest/doctest.h>

#include <ac/local/ResourceManager.hpp>

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

struct StrResource : public ac::local::Resource {
    StrResource(std::string s)
        : m_str(std::move(s))
    {}

    std::string m_str;
};

TEST_CASE("basic") {
    ac::local::ResourceManager<std::string> rm;
    {
        // empty
        auto res = rm.findResource<StrResource>("empty");
        CHECK(!res);
    }

    auto orig = rm.addResource("orig", std::make_shared<StrResource>("orig"));

    {
        // not empty
        auto someKey = rm.addResource("some-key", std::make_shared<StrResource>("value"));
        auto res = rm.findResource<StrResource>("some-key");
        CHECK(res);
        CHECK(someKey.get() == res.get());
        CHECK(res->m_str == "value");
    }

    {
        auto res = rm.findOrCreateResource<StrResource>("some-key", []() {
            return std::make_shared<StrResource>("value2");
        });

        CHECK(res);
        CHECK(res->m_str == "value");
    }

    {
        auto res = rm.findResource<StrResource>("some-key");
        auto res2 = rm.findOrCreateResource<StrResource>("some-key", []() {
            return std::make_shared<StrResource>("value2");
        });

        CHECK(res.get() == res2.get());
    }

    {
        auto res = rm.findOrCreateResource<StrResource>("some-other-key", []() {
            return std::make_shared<StrResource>("value2");
        });

        res.get()->maxAge = std::chrono::seconds(0);

        CHECK(res);
        CHECK(res->m_str == "value2");
    }

    // Collect resources that expired
    rm.garbageCollect();

    {
        auto res = rm.findResource<StrResource>("some-key");
        CHECK(res);

        auto res2 = rm.findResource<StrResource>("some-other-key");
        CHECK(!res2);
    }

    {
        // test that if we hold the resource, it won't be collected
        auto res = rm.findOrCreateResource<StrResource>("some-other-key", []() {
            return std::make_shared<StrResource>("value2");
        });

        res.get()->maxAge = std::chrono::seconds(0);

        CHECK(res);
        CHECK(res->m_str == "value2");

        rm.garbageCollect();

        CHECK(res->m_str == "value2");

        auto res2 = rm.findResource<StrResource>("some-other-key");
        CHECK(res.get() == res2.get());
    }

    // Force to garbage collect all resources
    rm.garbageCollect(true);

    {
        // Check that all resources are collected, but the one we hold is not
        auto orig2 = rm.findResource<StrResource>("orig");
        CHECK(orig2);

        auto res = rm.findResource<StrResource>("some-key");
        CHECK(!res);
    }
}

class ThreadPool {
public:
    ThreadPool(size_t numThreads)
        : stop(false)
    {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] {
                            return stop || !tasks.empty();
                        });

                        if (stop && tasks.empty()) {
                            return;
                        }

                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    template<class F, class... Args>
    std::future<typename std::invoke_result<F, Args...>::type> enqueue(F&& f, Args&&... args) {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> result = task->get_future();

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            if (stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            tasks.emplace([task]() { (*task)(); });
        }

        condition.notify_one();
        return result;
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }

        condition.notify_all();

        for (std::thread &worker : workers) {
            worker.join();
        }
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

TEST_CASE("threading") {
    constexpr size_t numThreads = 20;
    ThreadPool tpool(numThreads);

    ac::local::ResourceManager<std::string> rm;

    {
        // concurrent add
        std::vector<std::future<void>> tasks;
        for (size_t i = 0; i < 100; i++) {
            tasks.push_back(tpool.enqueue([&rm, i] {
                rm.addResource("key" + std::to_string(i), std::make_shared<StrResource>("value" + std::to_string(i)));
            }));
        }

        for (size_t i = 0; i < tasks.size(); i++) {
            tasks[i].wait();
        }
        tasks.clear();

        bool allExist = true;
        for (size_t i = 0; i < 100; i++) {
            auto res = rm.findResource<StrResource>("key" + std::to_string(i));
            if (!res) {
                allExist = false;
                break;
            }
        }
        CHECK(allExist);
    }

    rm.garbageCollect(true);

    {
        // concurrent find
        std::vector<std::pair<std::string, std::future<ac::local::ResourceLock<StrResource>>>> tasks;
        for (size_t i = 0; i < 100; i++) {
            auto val = "value" + std::to_string(i);
            tasks.push_back(std::make_pair(val, tpool.enqueue([&rm, i, val] {
                return rm.addResource("key" + std::to_string(i), std::make_shared<StrResource>(val));
            })));
        }

        for (size_t i = 0; i < tasks.size(); i++) {
            tasks[i].second.wait();
            CHECK(tasks[i].first == tasks[i].second.get()->m_str);
        }
    }

    rm.garbageCollect(true);

    {
        // concurrent findOrCreate
        std::vector<std::future<ac::local::ResourceLock<StrResource>>> tasks;
        const auto val = "unique";
        for (size_t i = 0; i < 100; i++) {
            tasks.push_back(tpool.enqueue([&rm, val] {
                return rm.findOrCreateResource<StrResource>(val, [val] {
                    return std::make_shared<StrResource>(val);
                });
            }));
        }

        auto resource = tasks[0].get();

        for (size_t i = 1; i < tasks.size(); i++) {
            CHECK(resource.get() == tasks[i].get().get());
        }
    }

    rm.garbageCollect(true);

    {
        // concurrent garbageCollect
        std::vector<std::future<void>> tasks;
        for (size_t i = 0; i < 100; i++) {
            const auto val = "unique" + std::to_string(i);
            rm.findOrCreateResource<StrResource>(val, [val] {
                return std::make_shared<StrResource>(val);
            });
        }

        for (size_t i = 0; i < numThreads; i++) {
            bool shouldForce = i % 2 == 0;
            tasks.push_back(tpool.enqueue([&rm, shouldForce] {
                rm.garbageCollect(shouldForce);
            }));
        }

        for (size_t i = 0; i < 100; i++) {
            const auto val = "unique" + std::to_string(i);
            auto res = rm.findResource<StrResource>(val);
            CHECK(!res);
        }
    }

    {
        // lock handling
        std::vector<std::future<int>> tasks;
        const auto val = "unique";
        std::atomic<int> callsCount = 1;
        // rm.addResource(val, std::make_shared<StrResource>(val));

        tasks.push_back(tpool.enqueue([&rm, &callsCount, val] {
            rm.findOrCreateResource<StrResource>(val, [&val] {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                return std::make_shared<StrResource>(val);
            });

            return callsCount++;
        }));

        tasks.push_back(tpool.enqueue([&rm, &callsCount, &val] {
            rm.findResource<StrResource>(val);
            return callsCount++;
        }));

        // check that we don't lock whole manager in findOrCreateResource
        CHECK(tasks[0].get() == 2);
        CHECK(tasks[1].get() == 1);
    }
}
