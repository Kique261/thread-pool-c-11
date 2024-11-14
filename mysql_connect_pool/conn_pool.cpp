#include "conn_pool.h"

std::once_flag Mysql_pool::init_flag;
Mysql_pool* Mysql_pool::instance = nullptr;

void Mysql_pool::init(){
    instance=new Mysql_pool();
}

Mysql_pool* Mysql_pool::getInstance(){
    call_once(init_flag,&Mysql_pool::init);
    return instance;
}

std::future<bool> Mysql_pool::command_in(std::shared_ptr<Mysql_command> &command)
{
    if(check(command)){
        std::unique_lock<std::mutex>lock(command_mutex);
        command_queue.push(command);
        conn_notify.notify_all();
    }else{
        throw std::runtime_error("illegal input");
    }
    return command->result.get_future();
}

Mysql_pool::Mysql_pool(){
    instance->user="root";
    instance->sql_password="123456";
    instance->dbname="web_server";
    instance->host="127.0.0.1";
    instance->port=3306;
    instance->max_conn.store(8);
    instance->acute_words={"select *","insert","drop"};
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

Mysql_pool::~Mysql_pool()
{
    std::cout<<"delete succeed!\n";
}


MYSQL* Mysql_pool::getConnection() {
    std::unique_lock<std::mutex> lock(conn_mutex);
    MYSQL* conn=nullptr;
    if(free_conn){
        conn = conn_pool.front();
        conn_pool.pop();
        free_conn--;
    }else{
        conn=mysql_init(nullptr);
        if(conn==nullptr){
            throw std::runtime_error("Mysql_conn init default.");
        }
        conn=mysql_real_connect(conn,host.c_str(),user.c_str(),sql_password.c_str(),dbname.c_str(),port,nullptr,0);
        if(conn==nullptr){
            throw std::runtime_error("Mysql_conn init default! Can't get real conn.");
        }
        max_conn++;
    }
    return  conn;
}

void Mysql_pool::releaseConnection(MYSQL* conn) {
    std::lock_guard<std::mutex> lock(conn_mutex);
    conn_pool.push(conn);
    ++free_conn;
    conn_notify.notify_one();
}

void Mysql_pool::work()
{
    while(true){
        std::unique_lock<std::mutex>lock(command_mutex);
        if(stop.load()&&command_queue.empty()) break;
        else if(command_queue.empty()){
            command_notify.wait(lock);
        }
        else{
            auto cur=command_queue.front();
            command_queue.pop();
            lock.unlock();
            MYSQL* work_conn=getConnection();
            std::string query;
            bool success=false;
            if(work_conn){
                switch (cur->type)
                {
                    case command_type::INSERT:
                        query = "INSERT INTO account (name, password) VALUES ('" + cur->id + "', '" + cur->password + "')";
                        if (mysql_query(work_conn, query.c_str()) == 0) {
                            success = true;
                        }
                        break;
                    case command_type::DELETE:
                        query = "DELETE FROM account WHERE name='" + cur->id + "' AND password='" + cur->password + "'";
                        if (mysql_query(work_conn, query.c_str()) == 0) {
                            success = true;
                        }
                        break;
                    case command_type::SELECT:
                        query = "INSERT INTO account (name, password) VALUES ('" + cur->id + "', '" + cur->password + "')";
                        if(mysql_query(work_conn,query.c_str())){
                            std::cerr << "SELECT query failed: " << mysql_error(work_conn) << std::endl;
                        }
                        MYSQL_RES* result = mysql_store_result(work_conn);
                        if(mysql_num_rows(result)){
                            success=true;
                        }
                        break;
                }
                cur->result.set_value(success);
                releaseConnection(work_conn);
            }
       }
    }
}

void Mysql_pool::shutdown()
{
    stop.store(false);
    command_notify.notify_all();
    delete instance;
}

bool Mysql_pool::check(const std::shared_ptr<Mysql_command> &command)
{
    return true;
}
