#include "heads.h"

class log {
public:
	log(const log&) = delete;
	log& operator=(const log&) = delete;


	static log* getInstance();
	void log_in(const std::string& words);
	void shutdown();
private:
	std::fstream& fileopen;
	static std::once_flag init_flag;
	static log* instance;
	std::mutex queue_mutex;
	std::condition_variable work_notify;
	std::atomic<bool> stop;
	std::queue<std::string>log_queue;
	log();
	~log();
	void worker();
	std::string getTime();
};