#include "heads.h"

enum class command_type{
    INSERT,
    DELETE,
    SELECT,
};

class Mysql_command{
public:
    command_type type;
    std::string id;
    std::string password;
    std::promise<bool> result;
    Mysql_command(const command_type& type, const std::string id, const std::string password)
        :type(type),id(id),password(password){}
    ~Mysql_command()=default;
};

class Mysql_pool{
public:
    static Mysql_pool* getInstance();
    std::future<bool> command_in(std::shared_ptr<Mysql_command> command);
    Mysql_pool(const Mysql_pool& other) = delete;
    Mysql_pool& operator=(const Mysql_pool& other) = delete;

private:
    static Mysql_pool* instance;
    static std::once_flag init_flag;
    std::mutex command_mutex;
    std::mutex conn_mutex;
    std::condition_variable conn_notify;
    std::condition_variable command_notify;

    std::string host;
    std::string user;
    std::string sql_password;
    std::string dbname;
    int port;

    std::queue<std::shared_ptr<Mysql_command>> command_queue;
    std::queue<MYSQL*> conn_pool;
    std::atomic<bool> stop;

    static void init();
    Mysql_pool();
    ~Mysql_pool();
    void work();
    void shutdown();
    bool check(const std::shared_ptr<Mysql_command>& command);
    MYSQL* getConnection();
    void releaseConnection(MYSQL* conn);
    std::vector<std::string> acute_words;
    std::atomic<int> cur_conn;
    std::atomic<int> max_conn;
    std::atomic<int> free_conn;
};

