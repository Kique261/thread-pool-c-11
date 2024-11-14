#include "heads.h"
#include "timer.h"
#include "thread_pool/thread_pool.h"

#include <iostream>

class TimerContainer {
public:
    TimerContainer(const TimerContainer&) = delete;
    TimerContainer& operator=(const TimerContainer&) = delete;
    static TimerContainer* getInstance() {
        std::call_once(init_flag, &TimerContainer::init);
        return instance;
    }
    
    static void shutdown(){
        stop.store(true);
        //std::cout << "stop" << std::endl;
        delete instance;
    }

    void add_timer(const std::shared_ptr<Timer> &timer);
    void delete_timer(const std::shared_ptr<Timer>& timer);

    template<typename Callback, typename... Args>
    std::shared_ptr<Timer> make_timer(int delay_time_ms, Callback callback, Args&&... args) {
        auto expire_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(delay_time_ms);
        auto timer_ptr=std::make_shared<Timer>(expire_time, std::bind(callback, std::forward<Args>(args)...));
        return timer_ptr;
    }

private:
    static std::once_flag init_flag;
    struct Compare {
        bool operator()(const std::shared_ptr<Timer>& lhs, const std::shared_ptr<Timer>& rhs) const {
            return lhs->get_expire_time() > rhs->get_expire_time();
        }
    };

    std::priority_queue<std::shared_ptr<Timer>, std::vector<std::shared_ptr<Timer>>, Compare> timer_queue;
    std::priority_queue<std::shared_ptr<Timer>, std::vector<std::shared_ptr<Timer>>, Compare> delete_queue;
    std::mutex queue_mutex;
    std::mutex delete_mutex;
    std::thread timer_manager;
    static std::atomic<bool> stop;
    std::condition_variable work_notify;
    static TimerContainer* instance;
    void work();

    static void init() {
        instance = new TimerContainer();
        stop.store(false);
    }

    TimerContainer();
    ~TimerContainer();
};