#include "concurrencpp.h"
#include "executor_test_helpers.h"

#include "../all_tests.h"

#include "../../tester/tester.h"
#include "../../helpers/assertions.h"
#include "../../helpers/object_observer.h"

#include "executors/constants.h"

namespace concurrencpp::tests {
	void test_thread_executor_name();

	void test_thread_executor_shutdown_enqueue();
	void test_thread_executor_shutdown_join();
	void test_thread_executor_shutdown();

	void test_thread_executor_max_concurrency_level();

	void test_thread_executor_post();
	void test_thread_executor_submit();

	void test_thread_executor_bulk_post();
	void test_thread_executor_bulk_submit();

	void assert_execution_threads(
		const std::unordered_map<size_t, size_t>& execution_map,
		const size_t expected_thread_count) {
		assert_equal(execution_map.size(), expected_thread_count);

		for (const auto& thread_pair : execution_map) {
			assert_not_equal(thread_pair.first, size_t(0));
			assert_equal(thread_pair.second, size_t(1));
		}
	}
}

void concurrencpp::tests::test_thread_executor_name() {
	auto executor = std::make_shared<concurrencpp::Thread_executor>();
	executor_shutdowner shutdown(executor);

	assert_equal(executor->name, concurrencpp::details::consts::k_thread_executor_name);
}

void concurrencpp::tests::test_thread_executor_shutdown_enqueue() {
	auto executor = std::make_shared<Thread_executor>();
	assert_false(executor->shutdown_requested());

	executor->shutdown();
	assert_true(executor->shutdown_requested());

	assert_throws<concurrencpp::errors::executor_shutdown>([executor] {
		executor->enqueue(std::coroutine_handle{});
	});

	assert_throws<concurrencpp::errors::executor_shutdown>([executor] {
		std::coroutine_handle<> array[4];
		std::span<std::coroutine_handle<>> span = array;
		executor->enqueue(span);
	});
}

void concurrencpp::tests::test_thread_executor_shutdown_join() {
	//the executor returns only when all tasks are done.
	auto executor = std::make_shared<Thread_executor>();
	object_observer observer;
	const size_t task_count = 16;

	for (size_t i = 0; i < task_count; i++) {
		executor->post(observer.get_testing_stub(std::chrono::milliseconds(250)));
	}

	executor->shutdown();
	assert_true(executor->shutdown_requested());

	assert_equal(observer.get_execution_count(), task_count);
	assert_equal(observer.get_destruction_count(), task_count);
}

void concurrencpp::tests::test_thread_executor_shutdown() {
	test_thread_executor_shutdown_enqueue();
	test_thread_executor_shutdown_join();
}

void concurrencpp::tests::test_thread_executor_max_concurrency_level() {
	auto executor = std::make_shared<Thread_executor>();
	executor_shutdowner shutdown(executor);

	assert_equal(executor->max_concurrency_level(),
		concurrencpp::details::consts::k_thread_executor_max_concurrency_level);
}

void concurrencpp::tests::test_thread_executor_post() {
	object_observer observer;
	const size_t task_count = 128;
	auto executor = std::make_shared<Thread_executor>();
	executor_shutdowner shutdown(executor);

	for (size_t i = 0; i < task_count; i++) {
		executor->post(observer.get_testing_stub(std::chrono::milliseconds(200)));
	}

	assert_true(observer.wait_execution_count(task_count, std::chrono::minutes(1)));
	assert_true(observer.wait_destruction_count(task_count, std::chrono::minutes(1)));

	assert_execution_threads(observer.get_execution_map(), task_count);
}

void concurrencpp::tests::test_thread_executor_submit() {
	object_observer observer;
	const size_t task_count = 128;
	auto executor = std::make_shared<Thread_executor>();
	executor_shutdowner shutdown(executor);

	std::vector<result<size_t>> results;
	results.resize(task_count);

	for (size_t i = 0; i < task_count; i++) {
		results[i] = executor->submit(observer.get_testing_stub(i, std::chrono::milliseconds(200)));
	}

	assert_true(observer.wait_execution_count(task_count, std::chrono::minutes(1)));
	assert_true(observer.wait_destruction_count(task_count, std::chrono::minutes(1)));

	assert_execution_threads(observer.get_execution_map(), task_count);

	for (size_t i = 0; i < task_count; i++) {
		assert_equal(results[i].get(), i);
	}
}

void concurrencpp::tests::test_thread_executor_bulk_post() {
	object_observer observer;
	const size_t task_count = 128;
	auto executor = std::make_shared<Thread_executor>();
	executor_shutdowner shutdown(executor);

	std::vector<testing_stub> stubs;
	stubs.reserve(task_count);

	for (size_t i = 0; i < task_count; i++) {
		stubs.emplace_back(observer.get_testing_stub(std::chrono::milliseconds(200)));
	}

	executor->template bulk_post<testing_stub>(stubs);

	assert_true(observer.wait_execution_count(task_count, std::chrono::minutes(1)));
	assert_true(observer.wait_destruction_count(task_count, std::chrono::minutes(1)));

	assert_execution_threads(observer.get_execution_map(), task_count);
}

void concurrencpp::tests::test_thread_executor_bulk_submit() {
	object_observer observer;
	const size_t task_count = 128;
	auto executor = std::make_shared<Thread_executor>();
	executor_shutdowner shutdown(executor);

	std::vector<value_testing_stub> stubs;
	stubs.reserve(task_count);

	for (size_t i = 0; i < task_count; i++) {
		stubs.emplace_back(observer.get_testing_stub(i, std::chrono::milliseconds(200)));
	}

	auto results = executor->template bulk_submit<value_testing_stub>(stubs);

	assert_true(observer.wait_execution_count(task_count, std::chrono::minutes(1)));
	assert_true(observer.wait_destruction_count(task_count, std::chrono::minutes(1)));

	assert_execution_threads(observer.get_execution_map(), task_count);

	for (size_t i = 0; i < task_count; i++) {
		assert_equal(results[i].get(), i);
	}
}

void concurrencpp::tests::test_thread_executor() {
	tester tester("thread_executor test");

	tester.add_step("shutdown", test_thread_executor_shutdown);
	tester.add_step("name", test_thread_executor_name);
	tester.add_step("max_concurrency_level", test_thread_executor_max_concurrency_level);
	tester.add_step("post", test_thread_executor_post);
	tester.add_step("submit", test_thread_executor_submit);
	tester.add_step("bulk_post", test_thread_executor_bulk_post);
	tester.add_step("bulk_submit", test_thread_executor_bulk_submit);

	tester.launch_test();
}
