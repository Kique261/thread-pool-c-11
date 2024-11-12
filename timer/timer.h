#include <chrono>
#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <atomic>
#include <vector>
class Timer {
	std::chrono::steady_clock::time_point expire_time;//期待时间
	std::function<void()> callback;//回调函数
public:
	Timer(std::chrono::steady_clock::time_point expire_time, std::function<void()> callback)
		: expire_time(expire_time), callback(callback) {};

	std::chrono::steady_clock::time_point get_expire_time() const { return expire_time; }

	void execute() const { callback(); }
};
