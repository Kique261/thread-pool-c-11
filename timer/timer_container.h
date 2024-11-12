#include "heads.h"
#include "timer.h"

class TimerContainer {
public:
    TimerContainer(const TimerContainer&) = delete;
    TimerContainer& operator=(const TimerContainer&) = delete;
    static TimerContainer& getInstance() {
        std::call_once(init_flag, &TimerContainer::init);
        return *instance;
    }

    void shutdown(){
        stop.store(true);
    }

    template<typename Callback, typename... Args>
    void add_timer(int delay_time_ms, Callback callback, Args&&... args) {
        auto expire_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(delay_time_ms);
        Timer timer(expire_time, std::bind(callback, std::forward<Args>(args)...));
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            timer_queue.emplace(timer);
            work_notify.notify_all();
        }
    }

private:
    static std::once_flag init_flag;
    struct Compare {
        bool operator()(const Timer& lhs, const Timer& rhs) {
            return lhs.get_expire_time() > rhs.get_expire_time();
        }
    };

    std::priority_queue<Timer, std::vector<Timer>, Compare> timer_queue;
    std::mutex queue_mutex;
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