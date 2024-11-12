#include <chrono>
#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <atomic>
#include <vector>
#include "timer.h"
#include "timer_container.h"


void TimerContainer::work()
{
	while (true) {
		std::unique_lock<std::mutex> lock(queue_mutex);
		if(stop.load() && timer_queue.empty()) {
			break;
		}
		if (timer_queue.empty()) {
			work_notify.wait(lock);
		}
		else {
			auto now = std::chrono::steady_clock::now();
			auto next_expire_time = timer_queue.top().get_expire_time();
			if(now>=next_expire_time) {
				timer_queue.top().execute();
				timer_queue.pop();
			}
			else {
				work_notify.wait_until(lock, next_expire_time);
			}

		}
	}
}

TimerContainer::TimerContainer()
{
	stop.store(false);
	instance = nullptr;
	timer_manager = std::thread(&TimerContainer::work, this);

}

TimerContainer::~TimerContainer() {
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop.store(true);
	}
	work_notify.notify_all();
	if (timer_manager.joinable()) {
		timer_manager.join();
	}
}