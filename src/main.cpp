#include "restart_db.h"
#include "restart_app.h"

int main(int argc, char **argv)
{
    mongocxx::instance inst{}; // This should be done only once.
    std::string root = COCO_NAME;
    std::string mongodb_host = MONGODB_HOST;
    std::string mongodb_port = MONGODB_PORT;
    std::vector<std::string> rules;
    rules.push_back("extern/coco/rules/rules.clp");
    rules.push_back("rules/rules.clp");

    restart::restart_db db(root, "mongodb://" + mongodb_host + ":" + mongodb_port);
    restart::restart_app app(db);

    app.load_rules(rules);

    return 0;
}