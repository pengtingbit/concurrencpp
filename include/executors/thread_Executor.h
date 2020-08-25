#ifndef CONCURRENCPP_THREAD_EXECUTOR_H
#define CONCURRENCPP_THREAD_EXECUTOR_H

#include "constants.h"

#include "../threads/thread.h"

#include <list>
#include <span>
#include <mutex>
#include <condition_variable>
#include <coroutine>

#include "executor.h"

namespace concurrencpp::details {
	class thread_worker {

	private:
		thread m_thread;
		Thread_executor& m_parent_pool;

		void execute_and_retire(
			std::coroutine_handle<> task,
			typename std::list<thread_worker>::iterator self_it);

	public:
		thread_worker(Thread_executor& parent_pool) noexcept;
		~thread_worker() noexcept;

		void start(
			const std::string worker_name,
			std::coroutine_handle<> task,
			std::list<thread_worker>::iterator self_it);
	};
}

namespace concurrencpp {
	class alignas(64) Thread_executor final : public Executor {

		friend class ::concurrencpp::details::thread_worker;

	private:
		std::mutex m_lock;
		std::list<details::thread_worker> m_workers;
		std::condition_variable m_condition;
		std::list<details::thread_worker> m_last_retired;
		std::atomic_bool m_atomic_abort;

		void enqueue_impl(std::coroutine_handle<> task);

		void retire_worker(std::list<details::thread_worker>::iterator it);

	public:
		Thread_executor() :
			Executor(details::consts::k_thread_executor_name),
			m_atomic_abort(false) {}

		~Thread_executor() noexcept;

		void enqueue(std::coroutine_handle<> task) override;
		void enqueue(std::span<std::coroutine_handle<>> tasks) override;

		int max_concurrency_level() const noexcept override;

		bool shutdown_requested() const noexcept override;
		void shutdown() noexcept override;
	};
}

#endif
