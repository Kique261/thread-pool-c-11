#include "heads.h"

class epoller{
public:
    epoller(int max_event_size=1024);
    ~epoller();
    bool addfd(int fd,uint32_t event);
    bool modfd(int fd,uint32_t event);
    bool delfd(int fd);
    int wait(int timeout_limit=-1);
    int get_eventfd(size_t i) const;
    uint32_t get_event(size_t i) const;

private:
    std::vector<struct epoll_event>events;
    int epollfd;
};