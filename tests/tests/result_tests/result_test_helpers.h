#ifndef CONCURRENCPP_RESULT_TEST_HELPERS_H
#define CONCURRENCPP_RESULT_TEST_HELPERS_H

#include "../../helpers/assertions.h"
#include "../../helpers/random.h"

#include "concurrencpp.h"

#include <stdexcept>
#include <type_traits>
#include <string_view>
#include <memory>

namespace concurrencpp::tests {
	template<class type>
	struct result_factory {
		static type get() { return type(); }

		static type throw_ex() {
			throw std::underflow_error("");
			return get();
		}
	};

	template<>
	struct result_factory<int> {
		static int get() { return 123456789; }

		static int throw_ex() {
			throw std::underflow_error("");
			return get();
		}
	};

	template<>
	struct result_factory<std::string> {
		static std::string get() { return "abcdefghijklmnopqrstuvwxyz123456789!@#$%^&*()"; }

		static std::string throw_ex() {
			throw std::underflow_error("");
			return get();
		}
	};

	template<>
	struct result_factory<void> {
		static void get() {}

		static void throw_ex() { throw std::underflow_error(""); }
	};

	template<>
	struct result_factory<int&> {
		static int& get() {
			static int i = 0;
			return i;
		}

		static int& throw_ex() {
			throw std::underflow_error("");
			return get();
		}
	};

	template<>
	struct result_factory<std::string&> {
		static std::string& get() {
			static std::string str;
			return str;
		}

		static std::string& throw_ex() {
			throw std::underflow_error("");
			return get();
		}
	};
}

namespace concurrencpp::tests {

	template<class type>
	void test_ready_result_result(::concurrencpp::result<type> result) {
		assert_true(static_cast<bool>(result));
		assert_equal(result.status(), concurrencpp::result_status::value);

		if constexpr (std::is_reference_v<type>) {
			assert_equal(&result.get(), &result_factory<type>::get());
		}
		else if constexpr (!std::is_void_v<type>) {
			assert_equal(result.get(), result_factory<type>::get());
		}
		else {
			result.get(); //just make sure no exception is thrown.
		}
	}

	struct costume_exception : public std::exception {
		const intptr_t id;

		costume_exception(intptr_t id) noexcept : id(id) {}
	};

	template<class type>
	void test_ready_result_costume_exception(concurrencpp::result<type> result, const intptr_t id) {
		assert_true(static_cast<bool>(result));
		assert_equal(result.status(), concurrencpp::result_status::exception);

		try {
			result.get();
		}
		catch (costume_exception e) {
			return assert_equal(e.id, id);
		}
		catch (...) {}

		assert_true(false);
	}
}

namespace concurrencpp::tests {
	class test_executor : public ::concurrencpp::executor {

	private:
		std::thread m_execution_thread;
		std::thread m_setting_thread;

	public:
		test_executor() noexcept : executor("test_executor") {}

		~test_executor() noexcept {
			if (m_execution_thread.joinable()) {
				m_execution_thread.detach();
			}

			if (m_setting_thread.joinable()) {
				m_setting_thread.detach();
			}
		}

		int max_concurrency_level() const noexcept override {
			return 0;
		}

		bool shutdown_requested() const noexcept override {
			return false;
		}

		void shutdown() noexcept override {
			//do nothing
		}

		bool scheduled_async() const noexcept {
			return std::this_thread::get_id() == m_execution_thread.get_id();
		}

		bool scheduled_inline() const noexcept {
			return std::this_thread::get_id() == m_setting_thread.get_id();
		}

		void enqueue(std::experimental::coroutine_handle<> task) override {
			m_execution_thread = std::thread(std::move(task));
		}

		void enqueue(std::span<std::experimental::coroutine_handle<>> span) override {
			for (auto task : span) {
				enqueue(task);
			}
		}

		template<class type>
		void set_rp_value(result_promise<type> rp) {
			m_setting_thread = std::thread([rp = std::move(rp)]() mutable {
				std::this_thread::sleep_for(std::chrono::seconds(2));
				rp.set_from_function(result_factory<type>::get);
			});
		}

		template<class type>
		size_t set_rp_err(result_promise<type> rp) {
			random randomizer;
			const auto id = static_cast<size_t>(randomizer());
			m_setting_thread = std::thread([id, rp = std::move(rp)]() mutable {
				std::this_thread::sleep_for(std::chrono::seconds(2));
				rp.set_exception(std::make_exception_ptr(costume_exception(id)));
			});
			return id;
		}
	};

	inline std::shared_ptr<test_executor> make_test_executor() {
		return std::make_shared<test_executor>();
	}
}

namespace concurrencpp::tests {
	struct throwing_executor : public concurrencpp::executor {

		throwing_executor() : executor("throwing_executor") {}

		void enqueue(std::experimental::coroutine_handle<>) override {
			throw std::runtime_error("executor exception");
		}

		void enqueue(std::span<std::experimental::coroutine_handle<>>) override {
			throw std::runtime_error("executor exception");
		}

		int max_concurrency_level() const noexcept override {
			return 0;
		}

		bool shutdown_requested() const noexcept override {
			return false;
		}

		void shutdown() noexcept override {
			//do nothing
		}
	};

	template<class type>
	void test_executor_error_thrown(
		concurrencpp::result<type> result,
		std::shared_ptr<concurrencpp::executor> throwing_executor) {
		assert_equal(result.status(), concurrencpp::result_status::exception);
		try {
			result.get();
		}
		catch (concurrencpp::errors::executor_exception ex) {
			assert_equal(ex.throwing_executor.get(), throwing_executor.get());

			try {
				std::rethrow_exception(ex.thrown_exception);
			}
			catch (std::runtime_error re) {
				assert_equal(std::string(re.what()), "executor exception");
			}
			catch (...) {
				assert_false(true);
			}
		}
		catch (...) {
			assert_true(false);
		}
	}
}

namespace concurrencpp::tests {
	template<class type>
	class await_to_resolve_coro {

	private:
		result<type> m_result;
		std::shared_ptr<concurrencpp::executor> m_test_executor;
		bool m_force_rescheduling;

	public:
		await_to_resolve_coro(
			result<type> result,
			std::shared_ptr<concurrencpp::executor> test_executor,
			bool force_rescheduling) noexcept :
			m_result(std::move(result)),
			m_test_executor(std::move(test_executor)),
			m_force_rescheduling(force_rescheduling) {}

		result<type> operator() () {
			co_return co_await m_result.await_via(std::move(m_test_executor), m_force_rescheduling);
		}
	};
}

#endif //CONCURRENCPP_RESULT_HELPERS_H
