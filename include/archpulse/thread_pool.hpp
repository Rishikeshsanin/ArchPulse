#pragma once
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
namespace archpulse { class ThreadPool { public: explicit ThreadPool(std::size_t thread_count); ~ThreadPool(); ThreadPool(const ThreadPool&)=delete; ThreadPool& operator=(const ThreadPool&)=delete; template<class F,class...Args> auto submit(F&&f,Args&&...args)->std::future<std::invoke_result_t<F,Args...>>{using R=std::invoke_result_t<F,Args...>;auto task=std::make_shared<std::packaged_task<R()>>(std::bind(std::forward<F>(f),std::forward<Args>(args)...));auto result=task->get_future();{std::lock_guard<std::mutex> lock(mutex_);if(stopping_)throw std::runtime_error("submit on stopped ThreadPool");tasks_.emplace([task](){(*task)();});}condition_.notify_one();return result;} [[nodiscard]] std::size_t size() const{return workers_.size();} private: std::vector<std::thread> workers_; std::queue<std::function<void()>> tasks_; mutable std::mutex mutex_; std::condition_variable condition_; bool stopping_{false}; }; }
