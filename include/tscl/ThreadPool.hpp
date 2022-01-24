/**
 * @file ThreadPool.hpp
 * @brief Simple thread pool implementation using std::threads and a task queue
 * @author Mathys JAM
 * @date 13/12/2021
 *
 */
#pragma once

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <future>

namespace tscl {

  class TaskBase {
  public :

    virtual void run() = 0;

  private :
  };

  template<typename T, typename ... Args>
  class Task : public TaskBase {
  public :

    explicit Task(Args &&... a) : args(std::forward<Args>(a)...) {
    }

    template<typename Res = std::invoke_result_t<std::function<T>, Args...>>
    std::future<Res> setFunc(std::function<T> func) {

      std::packaged_task<T> t(func);
      std::future<Res> res(t.get_future());
      task = std::move(t);
      return res;
    }

    void run() override {
      std::apply(this->task, this->args);
    }

  private :
    std::packaged_task<T> task;
    std::tuple<Args...> args;
  };

  class ThreadPool {
  public :
    explicit ThreadPool(size_t pool_size) : stop(false) {
      threads.reserve(pool_size);
      for (size_t i = 0; i < pool_size; i++) {
        threads.emplace_back(&ThreadPool::threadLoop, this);
      }
    }

    ~ThreadPool() {
      releaseThreads();
    }

    void releaseThreads() {
      std::unique_lock lock(mutex);
      stop = true;
      waiting_condition.notify_all();
      lock.unlock();
      for (std::thread &worker: threads) {
        worker.join();
      }
    }

    template<typename T, typename ... Args>
    auto enqueue(std::function<T> func, Args &&... args) {
      std::unique_ptr<Task<T, Args...>> task = std::make_unique<Task<T, Args...>>(args...);
      auto future = task->setFunc(func);

      std::lock_guard<std::mutex> lock(mutex);
      task_queue.push(std::move(task));
      waiting_condition.notify_one();
      return future;
    }


  private :

    void threadLoop() {
      std::unique_lock lock(mutex);
      while (not stop) {
        waiting_condition.wait(lock, [this] { return not task_queue.empty() || stop; });
        if (task_queue.empty()) {
          continue;
        }
        std::cout << "Thread " << std::this_thread::get_id() << " is running !" << std::endl;
        std::flush(std::cout);
        auto task = std::move(task_queue.front());
        task_queue.pop();
        lock.unlock();
        task->run();
        lock.lock();
      }
    }

    std::atomic_bool stop;
    std::vector<std::thread> threads;
    std::queue<std::unique_ptr<TaskBase>> task_queue;
    std::mutex mutex;
    std::condition_variable waiting_condition;
  };
}


