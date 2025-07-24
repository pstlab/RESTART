#include "coco.hpp"
#include "coco_type.hpp"
#include "coco_item.hpp"
#include "mongo_db.hpp"
#include "coco_server.hpp"
#include "coco_noauth.hpp"
#include "restart.hpp"
#include "restart_server.hpp"
#include "logging.hpp"
#include <mongocxx/instance.hpp>
#include <thread>

int main()
{
    mongocxx::instance inst{}; // This should be done only once.
    coco::mongo_db db;
    coco::coco cc(db);
    auto &rst = cc.add_module<restart::restart>(cc);

    coco::coco_server srv(cc);
    srv.add_module<coco::server_noauth>(srv);
    srv.add_module<restart::restart_server>(srv, rst);
    auto srv_ft = std::async(std::launch::async, [&srv]
                             { srv.start(); });

#ifdef INTERACTIVE_TEST
    std::string user_input;
    std::cin >> user_input;
    if (user_input == "d")
    {
        db.drop();
        srv.stop();
    }
#endif

    return 0;
}
