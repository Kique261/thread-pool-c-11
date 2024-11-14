#include "conn_pool.h"

std::once_flag Mysql_pool::init_flag;
Mysql_pool* Mysql_pool::instance = nullptr;

void Mysql_pool::init(){
    instance=new Mysql_pool();
    instance->user="root";
    instance->sql_password="123456";
    instance->dbname="account";
    instance->host="127.0.0.1";
    instance->port=3306;
    instance->max_conn.store(8);
    {
        std::unique_lock<std::mutex>lock(conn_mutex);
        for(int i=0;i<max_conn.load();i++){
            MYSQL* conn=mysql_init(nullptr);
            if(conn==nullptr){
                throw std::runtime_error("Mysql_conn init default.");
            }
            conn=mysql_real_connect(conn,host.c_str(),user.c_str(),sql_password.c_str(),dbname.c_str(),port,nullptr,0);
            if(conn==nullptr){
                throw std::runtime_error("Mysql_conn init default! Can't get real conn.");
            }
            conn_pool.push(conn);
            free_conn++;
            cur_conn++;
        }
    }
}

Mysql_pool* Mysql_pool::getInstance(){
    call_once(init_flag,&Mysql_pool::init);
    return instance;
}

Mysql_pool::Mysql_pool(){

}