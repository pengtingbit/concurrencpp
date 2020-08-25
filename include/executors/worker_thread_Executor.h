#ifndef CONCURRENCPP_WORKER_THREAD_EXECUTOR_H
#define CONCURRENCPP_WORKER_THREAD_EXECUTOR_H

#include "../threads/thread.h"

#include <deque>

#include "executor.h"

namespace concurrencpp {
	class alignas(64) worker_thread_executor final : public Executor {

	private:
		std::deque<std::coroutine_handle<>> m_private_queue;
		std::atomic_bool m_private_atomic_abort;
		details::thread m_thread;
		const char m_padding[64] = {};
		std::mutex m_lock;
		std::deque<std::coroutine_handle<>> m_public_queue;
		std::condition_variable m_condition;
		bool m_abort;
		std::atomic_bool m_atomic_abort;

		void join();
		void destroy_tasks() noexcept;

		bool drain_queue_impl();
		bool drain_queue();
		void work_loop() noexcept;

		void enqueue_local(std::coroutine_handle<> task);
		void enqueue_local(std::span<std::coroutine_handle<>> task);

		void enqueue_foreign(std::coroutine_handle<> task);
		void enqueue_foreign(std::span<std::coroutine_handle<>> task);

	public:
		worker_thread_executor();
		~worker_thread_executor() noexcept;

		void enqueue(std::coroutine_handle<> task) override;
		void enqueue(std::span<std::coroutine_handle<>> tasks) override;

		int max_concurrency_level() const noexcept override;

		bool shutdown_requested() const noexcept override;
		void shutdown() noexcept override;
	};
}

#endif