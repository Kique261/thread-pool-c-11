#include "epoller.h"

epoller::epoller(int max_event_size)
:epollfd(epoll_create(512)), events(max_event_size)
{
    assert(epollfd>=0&&events.size()>0);
}

epoller::~epoller()
{
    close(epollfd);
}

bool epoller::addfd(int fd, uint32_t event)
{
    if(fd<0) return false;
    epoll_event ev{0};
    ev.data.fd=fd;
    ev.events=event;
    return 0==epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
}

bool epoller::modfd(int fd, uint32_t event)
{
    if(fd<0) return false;
    epoll_event ev{0};
    ev.data.fd=fd;
    ev.events=event;
    return 0==epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
}

bool epoller::delfd(int fd)
{
    if(fd<0) return false;
    epoll_event ev{0};
    ev.data.fd=fd;
    return 0==epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
}

int epoller::wait(int timeout_limit)
{
    return epoll_wait(epollfd, &events[0], static_cast<int>(events.size()), timeout_limit);
}

int epoller::get_eventfd(size_t i) const
{
    assert(i<events.size()&&i>=0);
    return events[i].data.fd;
}

uint32_t epoller::get_event(size_t i) const
{
    assert(i<events.size()&&i>=0);
    return events[i].events;
}
