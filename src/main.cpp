#include "restart.hpp"
#include <mongocxx/instance.hpp>

int main(int argc, char **argv)
{
    mongocxx::instance inst{}; // This should be done only once.

    std::string server_host = SERVER_HOST;
    std::size_t server_port = SERVER_PORT;

    std::string db_host = MONGODB_HOST;
    std::string db_port = MONGODB_PORT;

    restart::restart server(server_host, server_port, "mongodb://" + db_host + ":" + db_port);
    server.start();

    return 0;
}