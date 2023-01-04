#pragma once

#include <thread>
#include <unordered_map>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <functional>
#include <queue>
#include <future>
#include <stdexcept>
#include <chrono>

namespace ThreadPool
{
	class ThreadPool
	{
	public:
		using Task = std::function<void()>;

		ThreadPool(std::size_t minThreads) : minThreads_{ minThreads }, freeThreads_{ minThreads }
		{
			start();
		}

		~ThreadPool()
		{
			stop();
		}

		template <class F, class... Args>
		std::future<std::result_of_t<F(Args...)>> enqueue(F&& f, Args &&...args)
		{
			/* The return type of task `F` */
			using return_type = std::result_of_t<F(Args...)>;

			/* wrapper for no arguments */
			auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
			{
				std::lock_guard<std::mutex> lg(m_);

				if (!isRun_)
					throw std::runtime_error("The thread pool has been stop.");

				/* wrapper for no returned value */
				queue_.emplace([=]() -> void { (*task)(); });
				if (queue_.size() > freeThreads_)
				{
					std::lock_guard<std::mutex> lg(mThreads_);
					auto newThread = std::thread(&ThreadPool::threadRoutine, this);
					newThread.detach();
					threadsCount_++;
					freeThreads_++;
				}
			}
			cv_.notify_one();
			return task->get_future();
		}

	private:
		std::size_t minThreads_;
		std::atomic<int> freeThreads_;

		std::queue<Task> queue_;
		std::condition_variable cv_;
		std::mutex m_;
		
		std::unordered_map<long, std::thread> threads_;
		std::mutex mThreads_;
		int threadsCount_ = 0;

		bool isRun_ = true;

		void start()
		{
			{
				std::lock_guard<std::mutex> lg(mThreads_);
				for (size_t i = 0; i < minThreads_; i++)
				{
					auto newThread = std::thread(&ThreadPool::threadRoutine, this);
					newThread.detach();
					threadsCount_++;
				}
			}
		}

		void threadRoutine()
		{
			printf("Create thread in pool\n");
			auto currentTime = std::chrono::high_resolution_clock::now();
			bool needDecrease = true;
			while (isRun_ || !queue_.empty())
			{
				Task task;
				{
					std::unique_lock<std::mutex> ul(m_);
					cv_.wait_for(ul, std::chrono::seconds(10), [&] {return !isRun_ || !queue_.empty(); });
					auto newTime = std::chrono::high_resolution_clock::now();
					if (!queue_.empty())
					{
						task = std::move(queue_.front());
						queue_.pop();
						currentTime = newTime;
						freeThreads_--;
					}
					else
					{
						float diff = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
						if (diff > 30)
						{
							{
								std::lock_guard<std::mutex> lgThreads(mThreads_);
								if (threadsCount_ > minThreads_)
								{
									threadsCount_--;
									needDecrease = false;
									freeThreads_--;
									break;
								}
								
								freeThreads_--;
								continue;
							}
						}
						else
						{
							continue;
						}
					}
				}

				task();
				freeThreads_++;
			}
			
			if (needDecrease)
			{
				{
					std::lock_guard<std::mutex> lgThreads(mThreads_);
					threadsCount_--;
				}
			}
			printf("Destroy thread in pool\n");
		}

		void stop() noexcept
		{
			{
				std::lock_guard<std::mutex> lg(m_);
				isRun_ = false;
			}

			cv_.notify_all();
			while (true)
			{
				{
					std::lock_guard<std::mutex> lgThreads(mThreads_);
					if (threadsCount_ != 0)
					{
						continue;
					}
				}

				break;
			}
		}
	};
}