#include "restart_db.h"
#include "restart_app.h"

void create_test_data(restart::restart_db &db)
{
    db.drop();
    std::vector<coco::parameter_ptr> sn_pars;
    sn_pars.push_back(std::make_unique<coco::integer_parameter>("temperature", -100, 100));
    sn_pars.push_back(std::make_unique<coco::integer_parameter>("humidity", 0, 100));
    sn_pars.push_back(std::make_unique<coco::integer_parameter>("pressure", 0, 1000));
    auto sn_type_id = db.create_sensor_type("sensor_node", "A sensor that measures some environmental parameters", std::move(sn_pars));
    auto sn_id = db.create_sensor("env1", db.get_sensor_type(sn_type_id), std::make_unique<coco::location>(0, 0));
}

int main(int argc, char **argv)
{
    mongocxx::instance inst{}; // This should be done only once.
    std::string root = COCO_NAME;
    std::string dashboard_host = DASHBOARD_HOST;
    unsigned short dashboard_port = DASHBOARD_PORT;
    std::string mongodb_host = MONGODB_HOST;
    std::string mongodb_port = MONGODB_PORT;
    std::vector<std::string> rules;
    rules.push_back("extern/coco/rules/rules.clp");
    rules.push_back("rules/rules.clp");

    // we parse the command line arguments..
    for (int i = 1; i < argc - 1; i++)
        if (strcmp(argv[i], "-dashboard_host") == 0)
            dashboard_host = std::string(argv[++i]);
        else if (strcmp(argv[i], "-dashboard_port") == 0)
            dashboard_port = std::stoi(argv[++i]);
        else if (strcmp(argv[i], "-mongodb_host") == 0)
            mongodb_host = std::string(argv[++i]);
        else if (strcmp(argv[i], "-mongodb_port") == 0)
            mongodb_port = std::stoi(argv[++i]);
        else if (strcmp(argv[i], "-root") == 0)
            root = std::string(argv[++i]);
        else if (strcmp(argv[i], "-rules") == 0)
        {
            rules.clear();
            while (i < argc - 1 && argv[i + 1][0] != '-')
                rules.push_back(std::string(argv[++i]));
        }

    restart::restart_db db(root, "mongodb://" + mongodb_host + ":" + mongodb_port);
    if (true)
        create_test_data(db);
    restart::restart_app app(db, dashboard_host, dashboard_port);

    app.load_rules(rules);

    app.server::start();

    return 0;
}