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
    cc.init();

    try
    {
        [[maybe_unused]] auto &usr = cc.get_type("CognitiveDomain");
    }
    catch (const std::exception &e)
    {
        LOG_WARN("Initializing RESTART database");

        // Create the types for RESTART
        [[maybe_unused]] auto &cd_type = cc.create_type("CognitiveDomain", {}, json::json{{"name", {"type", "string"}}}, json::json());
        [[maybe_unused]] auto &ct_type = cc.create_type("CognitiveTest", {}, json::json{{"name", {"type", "string"}}, {"domain", {{"type", "item"}, {"domain", "CognitiveDomain"}}}}, json::json());
        [[maybe_unused]] auto &ce_type = cc.create_type("CognitiveExercise", {}, json::json{{"name", {"type", "string"}}, {"duration", {{"type", "int"}, {"min", 0}, {"max", 60}}}, {"domain", {{"type", "item"}, {"domain", "CognitiveTest"}}}}, json::json());
        [[maybe_unused]] auto &usr_type = cc.create_type("User", {}, json::json{{"name", {"type", "string"}}}, json::json());
        [[maybe_unused]] auto &t_done_type = cc.create_type("TestDone", {}, json::json{{"user", {{"type", "item"}, {"domain", "User"}}}, {"test", {{"type", "item"}, {"domain", "CognitiveTest"}}}, {"score", {{"type", "int"}, {"min", 0}, {"max", 6}}}}, json::json());
        [[maybe_unused]] auto &ex_done_type = cc.create_type("ExerciseDone", {}, json::json{{"user", {{"type", "item"}, {"domain", "User"}}}, {"exercise", {{"type", "item"}, {"domain", "CognitiveExercise"}}}, {"done", {{"type", "int"}, {"min", 0}, {"default", 0}}}, {"level", {{"type", "int"}, {"min", 0}, {"max", 6}}}, {"score", {{"type", "float"}, {"min", 0}, {"max", 1}}}}, json::json());
        [[maybe_unused]] auto &robot_type = cc.create_type("Robot", {}, json::json{{"name", {"type", "string"}}}, json::json{{"current_command", {{"type", "symbol"}, {"values", {"welcome", "rot", "training", "goodbye"}}}}, {"current_modality", {{"type", "symbol"}, {"values", std::vector<json::json>{"formal", "informal"}}}}, {"command_completed", {{"type", "symbol"}, {"values", {"welcome", "rot", "training", "goodbye"}}}}, {"current_user", {{"type", "item"}, {"domain", "User"}}}, {"current_exercise", {{"type", "item"}, {"domain", "CognitiveExercise"}}}, {"current_level", {{"type", "int"}, {"min", 0}, {"max", 10}}}, {"current_score", {{"type", "float"}, {"min", 0}, {"max", 1}}}});
        // Create the initial robot item
        [[maybe_unused]] auto &robot = cc.create_item(robot_type, json::json{{"name", "RESTART"}});

        cc.create_reactive_rule("robot_session", restart::start_session_rule);
        cc.create_reactive_rule("robot_rot", restart::start_rot_rule);
        {
            std::ifstream file("rules/start_training.clp");
            std::stringstream buffer;
            buffer << file.rdbuf();
            cc.create_reactive_rule("start_training", buffer.str());
        }
        {
            std::ifstream file("rules/exercise_done.clp");
            std::stringstream buffer;
            buffer << file.rdbuf();
            cc.create_reactive_rule("exercise_done", buffer.str());
        }
        cc.create_reactive_rule("robot_end_session", restart::end_session_rule);
    }

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
