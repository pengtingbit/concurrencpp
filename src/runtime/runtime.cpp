#include "runtime.h"

#include "../../include/executors/inline_Executor.h"
#include "../../include/executors/manual_Executor.h"
#include "../../include/executors/thread_Executor.h"
#include "../../include/executors/thread_pool_Executor.h"
#include "../../include/executors/worker_thread_Executor.h"
#include "../../include/timers/Timer_queue.h"
#include "constants.h"

#include "../executors/constants.h"

namespace concurrencpp::details {
	size_t default_max_cpu_workers() noexcept {
		return static_cast<size_t>(thread::hardware_concurrency() * consts::k_cpu_threadpool_worker_count_factor);
	}

	size_t default_max_background_workers() noexcept {
		return static_cast<size_t>(thread::hardware_concurrency() * consts::k_background_threadpool_worker_count_factor);
	}

	constexpr static auto k_default_max_worker_wait_time = std::chrono::seconds(consts::k_max_worker_waiting_time_sec);
}

using concurrencpp::runtime;
using concurrencpp::runtime_options;
using concurrencpp::details::executor_collection;

/*
	executor_collection;
*/

void executor_collection::register_executor(std::shared_ptr<Executor> executor) {
	std::unique_lock<decltype(m_lock)> lock(m_lock);
	assert(std::find(m_executors.begin(), m_executors.end(), executor) == m_executors.end());
	m_executors.emplace_back(std::move(executor));
}

void executor_collection::shutdown_all() noexcept {
	std::unique_lock<decltype(m_lock)> lock(m_lock);
	for (auto& executor : m_executors) {
		assert(static_cast<bool>(executor));
		executor->shutdown();
	}

	m_executors = {};
}

/*
	runtime_options
*/

runtime_options::runtime_options() noexcept :
	max_cpu_threads(details::default_max_cpu_workers()),
	max_cpu_thread_waiting_time(details::k_default_max_worker_wait_time),
	max_background_threads(details::default_max_background_workers()),
	max_background_thread_waiting_time(details::k_default_max_worker_wait_time) {}

/*
	runtime
*/

runtime::runtime() : runtime(runtime_options()) {}

runtime::runtime(const runtime_options& options) {
	m_timer_queue = std::make_shared<::concurrencpp::Timer_queue>();

	m_inline_executor = std::make_shared<::concurrencpp::Inline_executor>();
	m_registered_executors.register_executor(m_inline_executor);

	m_thread_pool_executor = std::make_shared<::concurrencpp::Thread_pool_executor>(
			details::consts::k_thread_pool_executor_name,
			options.max_cpu_threads,
			options.max_cpu_thread_waiting_time);
	m_registered_executors.register_executor(m_thread_pool_executor);

	m_background_executor = std::make_shared<::concurrencpp::Thread_pool_executor>(
		details::consts::k_background_executor_name,
		options.max_background_threads,
		options.max_background_thread_waiting_time);
	m_registered_executors.register_executor(m_background_executor);

	m_thread_executor = std::make_shared<::concurrencpp::Thread_executor>();
	m_registered_executors.register_executor(m_thread_executor);
}

concurrencpp::runtime::~runtime() noexcept {
	m_timer_queue.reset();
	m_registered_executors.shutdown_all();
}

std::shared_ptr<concurrencpp::Timer_queue> runtime::timer_queue() const noexcept {
	return m_timer_queue;
}

std::shared_ptr<concurrencpp::Inline_executor> runtime::inline_executor() const noexcept {
	return m_inline_executor;
}

std::shared_ptr<concurrencpp::Thread_pool_executor> runtime::thread_pool_executor() const noexcept {
	return m_thread_pool_executor;
}

std::shared_ptr<concurrencpp::Thread_pool_executor> runtime::background_executor() const noexcept {
	return m_background_executor;
}

std::shared_ptr<concurrencpp::Thread_executor> runtime::thread_executor() const noexcept {
	return m_thread_executor;
}

std::shared_ptr<concurrencpp::worker_thread_executor> runtime::make_worker_thread_executor() {
	auto executor = std::make_shared<worker_thread_executor>();
	m_registered_executors.register_executor(executor);
	return executor;
}

std::shared_ptr<concurrencpp::manual_executor> runtime::make_manual_executor() {
	auto executor = std::make_shared<concurrencpp::manual_executor>();
	m_registered_executors.register_executor(executor);
	return executor;
}

std::tuple<unsigned int, unsigned int, unsigned int> runtime::version() noexcept {
	return {
		details::consts::k_concurrencpp_version_major,
		details::consts::k_concurrencpp_version_minor,
		details::consts::k_concurrencpp_version_revision
	};
}