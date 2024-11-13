#include "thread_pool.h"

#include <threads.h>

std::once_flag thread_pool::init_flag;
thread_pool* thread_pool::instance = nullptr;

thread_pool::thread_pool(){
    thread_pool::stop=false;
    thread_pool::threads_num=std::thread::hardware_concurrency();
    for(size_t i=0;i<threads_num;++i){
        thread_pool::workers.emplace_back([this]{
            while(!this->stop.load()){
                std::function<void()>task;
                {
                    std::unique_lock<std::mutex>lock(this->queue_mutex);
                    if(!this->tasks.empty()){
                        task=std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    if(task){
                        task();
                    }
                }
            }
        });
    }
}

void thread_pool::init_pool() {
    instance = new thread_pool();
}

thread_pool* thread_pool::getInstance(){
    call_once(init_flag, &thread_pool::init_pool);
	return instance;
}

thread_pool::~thread_pool(){
    bool expected=false;
    if(stop.compare_exchange_weak(expected,true)&&!expected){
        for(auto& worker:workers){
            if(worker.joinable()){
                worker.join();
            }
        }
    }
}