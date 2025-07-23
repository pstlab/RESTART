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

    try
    {
        [[maybe_unused]] auto &usr = cc.get_type("CognitiveDomain");
    }
    catch (const std::exception &e)
    {
        LOG_WARN("Initializing RESTART database");

        auto &cd_type = cc.create_type("CognitiveDomain", {}, json::json{{"name", {"type", "string"}}}, json::json());
        auto &memory_cd = cc.create_item(cd_type, json::json{{"name", "Memory"}});
        auto &attention_cd = cc.create_item(cd_type, json::json{{"name", "Attention"}});
        auto &executive_functions_cd = cc.create_item(cd_type, json::json{{"name", "ExecutiveFunctions"}});
        auto &general_cd = cc.create_item(cd_type, json::json{{"name", "General"}});

        auto &ct_type = cc.create_type("CognitiveTest", {}, json::json{{"name", {"type", "string"}}, {"domain", {{"type", "item"}, {"domain", "CognitiveDomain"}}}}, json::json());
        auto &moca_ct = cc.create_item(ct_type, json::json{{"name", "MoCA"}, {"domain", general_cd.get_id()}});
        auto &attention_matrices_ct = cc.create_item(ct_type, json::json{{"name", "AttentionMatrices"}, {"domain", attention_cd.get_id()}});
        auto &trial_making_test_a_ct = cc.create_item(ct_type, json::json{{"name", "TrialMakingTestA"}, {"domain", executive_functions_cd.get_id()}});
        auto &trial_making_test_b_ct = cc.create_item(ct_type, json::json{{"name", "TrialMakingTestB"}, {"domain", executive_functions_cd.get_id()}});
        auto &trial_making_test_b_a_ct = cc.create_item(ct_type, json::json{{"name", "TrialMakingTestBA"}, {"domain", executive_functions_cd.get_id()}});
        auto &semantic_fluency_ct = cc.create_item(ct_type, json::json{{"name", "SemanticFluency"}, {"domain", executive_functions_cd.get_id()}});
        auto &phonological_fluency_ct = cc.create_item(ct_type, json::json{{"name", "PhonologicalFluency"}, {"domain", executive_functions_cd.get_id()}});
        auto &modified_wisconsin_card_sorting_test_ct = cc.create_item(ct_type, json::json{{"name", "ModifiedWisconsinCardSortingTest"}, {"domain", executive_functions_cd.get_id()}});
        auto &short_story_ct = cc.create_item(ct_type, json::json{{"name", "ShortStory"}, {"domain", memory_cd.get_id()}});

        auto &ce_type = cc.create_type("CognitiveExercise", {}, json::json{{"name", {"type", "string"}}, {"duration", {{"type", "int"}, {"min", 0}, {"max", 60}}}, {"domain", {{"type", "item"}, {"domain", "CognitiveTest"}}}}, json::json());
        auto &visual_memory_ce = cc.create_item(ce_type, json::json{{"name", "VisualMemory"}, {"duration", 5}, {"domain", memory_cd.get_id()}});
        auto &attention_ce = cc.create_item(ce_type, json::json{{"name", "Attention"}, {"duration", 5}, {"domain", attention_cd.get_id()}});
        auto &executive_functions_ce = cc.create_item(ce_type, json::json{{"name", "ExecutiveFunctions"}, {"duration", 5}, {"domain", executive_functions_cd.get_id()}});
    }

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
