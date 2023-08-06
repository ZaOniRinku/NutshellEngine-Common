#pragma once
#include "ntshengn_job_queue.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <algorithm>
#include <cstdint>

namespace NtshEngn {

	struct JobSharedData {
		JobQueue<std::function<void()>> jobQueue;
		std::condition_variable wakeCondition;
		std::mutex wakeMutex;
		std::atomic<uint32_t> currentJobs;
		bool running;
	};

	struct JobDispatchArguments {
		uint32_t workerIndex;
		uint32_t jobIndex;
	};

	class JobSystem {
	public:
		void init();
		void destroy();

		void execute(const std::function<void()>& job);
		void dispatch(uint32_t jobCount, uint32_t jobsPerWorker, const std::function<void(JobDispatchArguments)>& job);

		bool isBusy();
		
		void wait();

		uint32_t getNumThreads() const;

	private:
		uint32_t m_numThreads = 0;
		std::vector<std::thread> m_threads;
		JobSharedData m_sharedData;
	};

}