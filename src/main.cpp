#include "restart.hpp"
#include <mongocxx/instance.hpp>

int main(int argc, char **argv)
{
    mongocxx::instance inst{}; // This should be done only once.

    restart::restart server;
    server.start();

    return 0;
}