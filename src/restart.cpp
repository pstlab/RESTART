#include "restart.hpp"
#include "coco.hpp"
#include "coco_item.hpp"
#include "logging.hpp"
#include <fstream>
#include <sstream>
#include <cassert>

namespace restart
{
    restart::restart(coco::coco &cc) noexcept : coco_module(cc)
    {
        LOG_TRACE(next_id_defglobal);
        [[maybe_unused]] auto next_id_dg_err = Build(get_env(), next_id_defglobal);
        assert(next_id_dg_err == BE_NO_ERROR);
        LOG_TRACE(exercise_deftemplate);
        [[maybe_unused]] auto build_exercise_dt_err = Build(get_env(), exercise_deftemplate);
        assert(build_exercise_dt_err == BE_NO_ERROR);
        LOG_TRACE(enqueue_exercise_deffunction);
        [[maybe_unused]] auto build_enqueue_exercise_df_err = Build(get_env(), enqueue_exercise_deffunction);
        assert(build_enqueue_exercise_df_err == BE_NO_ERROR);

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
            [[maybe_unused]] auto &moca_ct = cc.create_item(ct_type, json::json{{"name", "MoCA"}, {"domain", general_cd.get_id()}});
            [[maybe_unused]] auto &attention_matrices_ct = cc.create_item(ct_type, json::json{{"name", "AttentionMatrices"}, {"domain", attention_cd.get_id()}});
            [[maybe_unused]] auto &trial_making_test_a_ct = cc.create_item(ct_type, json::json{{"name", "TrialMakingTestA"}, {"domain", executive_functions_cd.get_id()}});
            [[maybe_unused]] auto &trial_making_test_b_ct = cc.create_item(ct_type, json::json{{"name", "TrialMakingTestB"}, {"domain", executive_functions_cd.get_id()}});
            [[maybe_unused]] auto &trial_making_test_b_a_ct = cc.create_item(ct_type, json::json{{"name", "TrialMakingTestBA"}, {"domain", executive_functions_cd.get_id()}});
            [[maybe_unused]] auto &semantic_fluency_ct = cc.create_item(ct_type, json::json{{"name", "SemanticFluency"}, {"domain", executive_functions_cd.get_id()}});
            [[maybe_unused]] auto &phonological_fluency_ct = cc.create_item(ct_type, json::json{{"name", "PhonologicalFluency"}, {"domain", executive_functions_cd.get_id()}});
            [[maybe_unused]] auto &modified_wisconsin_card_sorting_test_ct = cc.create_item(ct_type, json::json{{"name", "ModifiedWisconsinCardSortingTest"}, {"domain", executive_functions_cd.get_id()}});
            [[maybe_unused]] auto &short_story_ct = cc.create_item(ct_type, json::json{{"name", "ShortStory"}, {"domain", memory_cd.get_id()}});

            auto &ce_type = cc.create_type("CognitiveExercise", {}, json::json{{"name", {"type", "string"}}, {"duration", {{"type", "int"}, {"min", 0}, {"max", 60}}}, {"domain", {{"type", "item"}, {"domain", "CognitiveTest"}}}}, json::json());
            [[maybe_unused]] auto &visual_memory_ce = cc.create_item(ce_type, json::json{{"name", "VisualMemory"}, {"duration", 5}, {"domain", memory_cd.get_id()}});
            [[maybe_unused]] auto &attention_ce = cc.create_item(ce_type, json::json{{"name", "Attention"}, {"duration", 5}, {"domain", attention_cd.get_id()}});
            [[maybe_unused]] auto &executive_functions_ce = cc.create_item(ce_type, json::json{{"name", "ExecutiveFunctions"}, {"duration", 5}, {"domain", executive_functions_cd.get_id()}});

            [[maybe_unused]] auto &usr_type = cc.create_type("User", {}, json::json{{"name", {"type", "string"}}}, json::json());
            [[maybe_unused]] auto &t_done_type = cc.create_type("TestDone", {}, json::json{{"user", {{"type", "item"}, {"domain", "User"}}}, {"test", {{"type", "item"}, {"domain", "CognitiveTest"}}}, {"score", {{"type", "int"}, {"min", 0}, {"max", 6}}}}, json::json());
            [[maybe_unused]] auto &ex_done_type = cc.create_type("ExerciseDone", {}, json::json{{"user", {{"type", "item"}, {"domain", "User"}}}, {"exercise", {{"type", "item"}, {"domain", "CognitiveExercise"}}}, {"level", {{"type", "int"}, {"min", 0}, {"max", 6}}}, {"done", {{"type", "int"}, {"min", 0}, {"default", 0}}}, {"performance", {{"type", "float"}, {"min", 0}, {"max", 1}}}}, json::json());

            auto &robot_type = cc.create_type("Robot", {}, json::json{{"name", {"type", "string"}}}, json::json{{"current_command", {{"type", "symbol"}, {"values", {"welcome", "rot", "training", "goodbye"}}}}, {"current_modality", {{"type", "symbol"}, {"values", std::vector<json::json>{"formal", "informal"}}}}, {"command_completed", {{"type", "symbol"}, {"values", {"welcome", "rot", "training", "goodbye"}}}}, {"current_user", {{"type", "item"}, {"domain", "User"}}}, {"current_exercise", {{"type", "item"}, {"domain", "CognitiveExercise"}}}, {"current_performance", {{"type", "float"}, {"min", 0}, {"max", 1}}}});
            [[maybe_unused]] auto &robot = cc.create_item(robot_type, json::json{{"name", "RESTART"}});

            cc.create_reactive_rule("robot_session", start_session_rule);
            cc.create_reactive_rule("robot_rot", start_rot_rule);
            {
                std::ifstream file("src/robot_training.clp");
                std::stringstream buffer;
                buffer << file.rdbuf();
                cc.create_reactive_rule("robot_training", buffer.str());
            }

            create_user("TestUser", json::json{{"MoCA", 2}, {"AttentionMatrices", 1}, {"TrialMakingTestA", 4}, {"TrialMakingTestB", 3}, {"TrialMakingTestBA", 0}, {"SemanticFluency", 2}, {"PhonologicalFluency", 4}, {"ModifiedWisconsinCardSortingTest", 1}, {"ShortStory", 3}});
        }
    }

    std::vector<std::reference_wrapper<user>> restart::get_users() noexcept
    {
        std::vector<std::reference_wrapper<user>> users;
        return users;
    }

    user &restart::create_user(std::string_view name, json::json &&tests, bool infere)
    {
        std::lock_guard<std::recursive_mutex> lock(get_mtx());
        auto &usr_type = get_coco().get_type("User");
        auto &ct_type = get_coco().get_type("CognitiveTest");
        auto &t_done_type = get_coco().get_type("TestDone");
        auto &ex_type = get_coco().get_type("CognitiveExercise");
        auto &ex_done_type = get_coco().get_type("ExerciseDone");

        auto &usr = get_coco().create_item(usr_type, json::json{{"name", name.data()}});
        for (const auto &[test_name, test_score] : tests.as_object())
        {
            std::string test_id;
            for (const auto &ct : get_coco().get_items(ct_type))
                if (ct.get().get_properties()["name"] == test_name)
                {
                    test_id = ct.get().get_id();
                    break;
                }
            [[maybe_unused]] auto &t_done = get_coco().create_item(t_done_type, json::json{{"user", usr.get_id()}, {"test", test_id}, {"score", test_score.get<int>()}});
        }
        for (const auto &ce : get_coco().get_items(ex_type))
            [[maybe_unused]]
            auto &ex_done = get_coco().create_item(ex_done_type, json::json{{"user", usr.get_id()}, {"exercise", ce.get().get_id()}});

        users.emplace(usr.get_id(), std::make_unique<user>(usr.get_id(), name, std::move(tests)));

        if (infere)
            Run(get_env(), -1);

        return *users.at(usr.get_id());
    }

    user::user(std::string_view id, std::string_view name, json::json &&tests) : id(id), name(name), tests(std::move(tests)) {}
    json::json user::to_json() const noexcept { return json::json{{"name", name}, {"tests", tests}}; }
} // namespace restart
