基于c++11-20的新特性写的简单线程池

使用call_once进行唯一一次实例化

使用锁与原子进行同步

使用package_task包装函数,推送到工作队列进行处理，future绑定接受返回值，jthread自动回收
