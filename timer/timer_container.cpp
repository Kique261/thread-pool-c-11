#include "timer_container.h"

std::once_flag TimerContainer::init_flag;
std::atomic<bool> TimerContainer::stop;
TimerContainer* TimerContainer::instance = nullptr;


void TimerContainer::work()
{
	while (true) {
		std::unique_lock<std::mutex> queue_lock(queue_mutex,std::defer_lock);
		std::unique_lock<std::mutex> delete_lock(delete_mutex,std::defer_lock);
		std::lock(queue_lock,delete_lock);
		if(stop.load() && timer_queue.empty()) {
			break;
		}
		if (timer_queue.empty()) {
			work_notify.wait(queue_lock);
		}
		else {
			auto now = std::chrono::steady_clock::now();
			if (!delete_queue.empty()&&delete_queue.top()==timer_queue.top()) {
				delete_queue.pop();
				timer_queue.pop();
				continue;
			}
			auto next_expire_time = timer_queue.top()->get_expire_time();
			if(now>=next_expire_time) {
				timer_queue.top()->execute();
				timer_queue.pop();
			}
			else {
				work_notify.wait_until(queue_lock, next_expire_time);
			}

		}
	}
}

void TimerContainer::add_timer(const std::shared_ptr<Timer>& timer) {
	std::unique_lock<std::mutex> lock(queue_mutex);
	timer_queue.emplace(timer);
	work_notify.notify_all();
}

void TimerContainer::delete_timer(const std::shared_ptr<Timer>& timer)
{
	std::unique_lock<std::mutex> lock(delete_mutex);
	delete_queue.emplace(timer);
}

TimerContainer::TimerContainer()
{
	stop.store(false);
	std::thread(&TimerContainer::work, this).detach();
}

TimerContainer::~TimerContainer() {
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop.store(true);
	}
	work_notify.notify_all();
	//std::cout<<"TimerContainer is shutting down\n";
}