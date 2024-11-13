#include "timer_container.h"
#include "thread_pool/thread_pool.h"
std::once_flag TimerContainer::init_flag;
std::atomic<bool> TimerContainer::stop;
TimerContainer* TimerContainer::instance = nullptr;
thread_pool* TimerContainer::thread_pool_instance = nullptr;


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
	thread_pool_instance=thread_pool::getInstance();
	thread_pool_instance->task_in(&TimerContainer::work, this);

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