#include "heads.h"
class Timer {
	std::chrono::steady_clock::time_point expire_time;
	std::function<void()> callback;
public:
	Timer(std::chrono::steady_clock::time_point expire_time, const std::function<void()>& callback)
		: expire_time(expire_time), callback(callback) {}

	std::chrono::steady_clock::time_point get_expire_time() const { return expire_time; }

	void execute() const { callback(); }
};
