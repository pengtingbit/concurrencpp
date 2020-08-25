#ifndef CONCURRENCPP_FORWARD_DECLERATIONS_H
#define CONCURRENCPP_FORWARD_DECLERATIONS_H

namespace concurrencpp {
	struct null_result;

	template<class type> class result;
	template<class type> class result_promise;

	class runtime;

	class Timer_queue;
	class timer;

	class Executor;
	class Inline_executor;
	class Thread_pool_executor;
	class Thread_executor;
	class worker_thread_executor;
	class manual_executor;
}

#endif //FORWARD_DECLERATIONS_H
