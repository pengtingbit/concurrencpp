#ifndef CONCURRENCPP_EXECUTOR_TEST_HELPERS_H
#define CONCURRENCPP_EXECUTOR_TEST_HELPERS_H

#include "../../../include/executors/executor.h"
#include "../../helpers/assertions.h"
#include "executors/constants.h"

namespace concurrencpp::tests {
	struct executor_shutdowner {
		std::shared_ptr<concurrencpp::Executor> executor;

		executor_shutdowner(std::shared_ptr<concurrencpp::Executor> executor) noexcept :
			executor(std::move(executor)) {}

		~executor_shutdowner() noexcept {
			executor->shutdown();
		}
	};
}

#endif
