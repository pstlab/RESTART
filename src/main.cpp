#include "restart.hpp"
#include <mongocxx/instance.hpp>

int main(int argc, char **argv)
{
    mongocxx::instance inst{}; // This should be done only once.

    std::string server_host = SERVER_HOST;
    std::size_t server_port = SERVER_PORT;

    std::string db_host = MONGODB_HOST;
    std::string db_port = MONGODB_PORT;

    std::string transformer_host = SERVER_HOST;
    std::size_t transformer_port = SERVER_PORT;

    auto env_server_host = std::getenv("SERVER_HOST");
    if (env_server_host)
        server_host = env_server_host;
    auto env_server_port = std::getenv("SERVER_PORT");
    if (env_server_port)
        server_port = std::stoi(env_server_port);

    auto env_db_host = std::getenv("MONGODB_HOST");
    if (env_db_host)
        db_host = env_db_host;
    auto env_db_port = std::getenv("MONGODB_PORT");
    if (env_db_port)
        db_port = env_db_port;

    auto env_transformer_host = std::getenv("TRANSFORMER_HOST");
    if (env_transformer_host)
        transformer_host = env_transformer_host;
    auto env_transformer_port = std::getenv("TRANSFORMER_PORT");
    if (env_transformer_port)
        transformer_port = std::stoi(env_transformer_port);

    restart::restart server(server_host, server_port, "mongodb://" + db_host + ":" + db_port, transformer_host, transformer_port);
    server.start();

    return 0;
}