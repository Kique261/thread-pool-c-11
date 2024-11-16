#include "log.h"

log::log()
	:stop(false), fileopen(*(new std::fstream("out.out")))
{
	std::thread([&]() {worker(); }).detach();
	work_notify.notify_all();
}

log* log::getInstance()
{
	std::call_once(init_flag, [&]()->void {instance = new log(); });
	return instance;
}

void log::log_in(const std::string& words)
{
	std::string in_word = getTime();
	in_word += words; 
	in_word += '\n';
	{
		std::unique_lock<std::mutex>lock(queue_mutex);
		log_queue.push(in_word);
	}
	work_notify.notify_all();
}

std::string log::getTime() {
	auto now = std::chrono::system_clock::now();
	auto now_time_t = std::chrono::system_clock::to_time_t(now);
	auto now_tm = *localtime(&now_time_t);
	auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
	std::ostringstream oss;
	oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << now_ms.count();
	return oss.str();
}

void log::shutdown() {
	stop.store(true);
	delete instance;
}

log::~log()
{
	std::cout << "shut down and delete\n";
}

void log::worker()
{
	while (1) {
		{
			std::unique_lock<std::mutex>lock(queue_mutex);
			if (!stop.load() && log_queue.empty()) {
				work_notify.wait(lock);
			}
			else if (!log_queue.empty()) {
				auto cur = log_queue.front();
				log_queue.pop();
				fileopen << cur;
			}
			else if (stop.load()) {
				break;
			}
		}
	}
}