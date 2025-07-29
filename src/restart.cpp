#include "restart.hpp"
#include "coco.hpp"
#include "coco_type.hpp"
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

            [[maybe_unused]] auto &cd_type = cc.create_type("CognitiveDomain", {}, json::json{{"name", {"type", "string"}}}, json::json());
            // auto &memory_cd = cc.create_item(cd_type, json::json{{"name", "Memory"}});
            // auto &attention_cd = cc.create_item(cd_type, json::json{{"name", "Attention"}});
            // auto &executive_functions_cd = cc.create_item(cd_type, json::json{{"name", "ExecutiveFunctions"}});
            // auto &general_cd = cc.create_item(cd_type, json::json{{"name", "General"}});

            [[maybe_unused]] auto &ct_type = cc.create_type("CognitiveTest", {}, json::json{{"name", {"type", "string"}}, {"domain", {{"type", "item"}, {"domain", "CognitiveDomain"}}}}, json::json());
            // [[maybe_unused]] auto &moca_ct = cc.create_item(ct_type, json::json{{"name", "MoCA"}, {"domain", general_cd.get_id()}});
            // [[maybe_unused]] auto &attention_matrices_ct = cc.create_item(ct_type, json::json{{"name", "AttentionMatrices"}, {"domain", attention_cd.get_id()}});
            // [[maybe_unused]] auto &trial_making_test_a_ct = cc.create_item(ct_type, json::json{{"name", "TrialMakingTestA"}, {"domain", executive_functions_cd.get_id()}});
            // [[maybe_unused]] auto &trial_making_test_b_ct = cc.create_item(ct_type, json::json{{"name", "TrialMakingTestB"}, {"domain", executive_functions_cd.get_id()}});
            // [[maybe_unused]] auto &trial_making_test_b_a_ct = cc.create_item(ct_type, json::json{{"name", "TrialMakingTestBA"}, {"domain", executive_functions_cd.get_id()}});
            // [[maybe_unused]] auto &semantic_fluency_ct = cc.create_item(ct_type, json::json{{"name", "SemanticFluency"}, {"domain", executive_functions_cd.get_id()}});
            // [[maybe_unused]] auto &phonological_fluency_ct = cc.create_item(ct_type, json::json{{"name", "PhonologicalFluency"}, {"domain", executive_functions_cd.get_id()}});
            // [[maybe_unused]] auto &modified_wisconsin_card_sorting_test_ct = cc.create_item(ct_type, json::json{{"name", "ModifiedWisconsinCardSortingTest"}, {"domain", executive_functions_cd.get_id()}});
            // [[maybe_unused]] auto &short_story_ct = cc.create_item(ct_type, json::json{{"name", "ShortStory"}, {"domain", memory_cd.get_id()}});

            [[maybe_unused]] auto &ce_type = cc.create_type("CognitiveExercise", {}, json::json{{"name", {"type", "string"}}, {"duration", {{"type", "int"}, {"min", 0}, {"max", 60}}}, {"domain", {{"type", "item"}, {"domain", "CognitiveTest"}}}}, json::json());
            // [[maybe_unused]] auto &visual_memory_ce = cc.create_item(ce_type, json::json{{"name", "VisualMemory"}, {"duration", 5}, {"domain", memory_cd.get_id()}});
            // [[maybe_unused]] auto &attention_ce = cc.create_item(ce_type, json::json{{"name", "Attention"}, {"duration", 5}, {"domain", attention_cd.get_id()}});
            // [[maybe_unused]] auto &executive_functions_ce = cc.create_item(ce_type, json::json{{"name", "ExecutiveFunctions"}, {"duration", 5}, {"domain", executive_functions_cd.get_id()}});

            [[maybe_unused]] auto &usr_type = cc.create_type("User", {}, json::json{{"name", {"type", "string"}}}, json::json());
            [[maybe_unused]] auto &t_done_type = cc.create_type("TestDone", {}, json::json{{"user", {{"type", "item"}, {"domain", "User"}}}, {"test", {{"type", "item"}, {"domain", "CognitiveTest"}}}, {"score", {{"type", "int"}, {"min", 0}, {"max", 6}}}}, json::json());
            [[maybe_unused]] auto &ex_done_type = cc.create_type("ExerciseDone", {}, json::json{{"user", {{"type", "item"}, {"domain", "User"}}}, {"exercise", {{"type", "item"}, {"domain", "CognitiveExercise"}}}, {"done", {{"type", "int"}, {"min", 0}, {"default", 0}}}, {"level", {{"type", "int"}, {"min", 0}, {"max", 6}}}, {"score", {{"type", "float"}, {"min", 0}, {"max", 1}}}}, json::json());

            auto &robot_type = cc.create_type("Robot", {}, json::json{{"name", {"type", "string"}}}, json::json{{"current_command", {{"type", "symbol"}, {"values", {"welcome", "rot", "training", "goodbye"}}}}, {"current_modality", {{"type", "symbol"}, {"values", std::vector<json::json>{"formal", "informal"}}}}, {"command_completed", {{"type", "symbol"}, {"values", {"welcome", "rot", "training", "goodbye"}}}}, {"current_user", {{"type", "item"}, {"domain", "User"}}}, {"current_exercise", {{"type", "item"}, {"domain", "CognitiveExercise"}}}, {"current_level", {{"type", "int"}, {"min", 0}, {"max", 10}}}, {"current_score", {{"type", "float"}, {"min", 0}, {"max", 1}}}});
            [[maybe_unused]] auto &robot = cc.create_item(robot_type, json::json{{"name", "RESTART"}});

            cc.create_reactive_rule("robot_session", start_session_rule);
            cc.create_reactive_rule("robot_rot", start_rot_rule);
            {
                std::ifstream file("src/start_training.clp");
                std::stringstream buffer;
                buffer << file.rdbuf();
                cc.create_reactive_rule("start_training", buffer.str());
            }
            {
                std::ifstream file("src/exercise_done.clp");
                std::stringstream buffer;
                buffer << file.rdbuf();
                cc.create_reactive_rule("exercise_done", buffer.str());
            }
            cc.create_reactive_rule("robot_end_session", end_session_rule);

            // create_user("TestUser", json::json{{"MoCA", 2}, {"AttentionMatrices", 1}, {"TrialMakingTestA", 4}, {"TrialMakingTestB", 3}, {"TrialMakingTestBA", 0}, {"SemanticFluency", 2}, {"PhonologicalFluency", 4}, {"ModifiedWisconsinCardSortingTest", 1}, {"ShortStory", 3}});
        }
    }

    std::vector<domain> restart::get_domains() noexcept
    {
        std::lock_guard<std::recursive_mutex> lock(get_mtx());
        std::vector<domain> domains;
        for (auto &cd : get_coco().get_items(get_coco().get_type("CognitiveDomain")))
            domains.emplace_back(cd.get().get_id(), cd.get().get_properties()["name"].get<std::string>());
        return domains;
    }
    std::string restart::create_domain(std::string_view name, bool infere)
    {
        std::lock_guard<std::recursive_mutex> lock(get_mtx());
        auto &cd = get_coco().create_item(get_coco().get_type("CognitiveDomain"), json::json{{"name", name.data()}});

        if (infere)
            Run(get_env(), -1);

        return cd.get_id();
    }

    std::vector<user> restart::get_users() noexcept
    {
        std::lock_guard<std::recursive_mutex> lock(get_mtx());
        std::vector<user> users;
        for (auto &usr : get_coco().get_items(get_coco().get_type("User")))
        {
            std::string query = "(find-all-facts ((?test-done-user TestDone_user)) (eq ?test-done-user:user " + usr.get().get_id() + "))";
            CLIPSValue test_facts;
            Eval(get_env(), query.c_str(), &test_facts);
            json::json tests;
            for (std::size_t i = 0; i < test_facts.multifieldValue->length; ++i)
            {
                CLIPSValue test_id;
                GetFactSlot(test_facts.multifieldValue->contents[i].factValue, "item_id", &test_id);
                auto &test_itm = get_coco().get_item(test_id.lexemeValue->contents);
                tests[test_itm.get_properties()["name"].get<std::string>()] = test_itm.get_properties()["score"].get<int64_t>();
            }
            users.emplace_back(usr.get().get_id(), usr.get().get_properties()["name"].get<std::string>(), std::move(tests));
        }
        return users;
    }

    std::string restart::create_user(std::string_view name, json::json &&tests, bool infere)
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
            auto &ex_done = get_coco().create_item(ex_done_type, json::json{{"user", usr.get_id()}, {"exercise", ce.get().get_id()}, {"done", 0}, {"level", 0}, {"score", 0.0}});

        if (infere)
            Run(get_env(), -1);

        return usr.get_id();
    }

    std::vector<test> restart::get_tests() noexcept
    {
        std::lock_guard<std::recursive_mutex> lock(get_mtx());
        std::vector<test> tests;
        for (auto &ct : get_coco().get_items(get_coco().get_type("CognitiveTest")))
            tests.emplace_back(ct.get().get_id(), ct.get().get_properties()["name"].get<std::string>(), get_coco().get_item(ct.get().get_properties()["domain"].get<std::string>()).get_id());
        return tests;
    }
    std::string restart::create_test(std::string_view name, std::string_view domain, bool infere)
    {
        std::lock_guard<std::recursive_mutex> lock(get_mtx());
        auto &test = get_coco().create_item(get_coco().get_type("CognitiveTest"), json::json{{"name", name.data()}, {"domain", get_domain(domain).get_id()}});

        if (infere)
            Run(get_env(), -1);

        return test.get_id();
    }

    std::vector<exercise> restart::get_exercises() noexcept
    {
        std::lock_guard<std::recursive_mutex> lock(get_mtx());
        std::vector<exercise> exercises;
        for (auto &ce : get_coco().get_items(get_coco().get_type("CognitiveExercise")))
            exercises.emplace_back(ce.get().get_id(), ce.get().get_properties()["name"].get<std::string>(), get_coco().get_item(ce.get().get_properties()["domain"].get<std::string>()).get_id(), ce.get().get_properties()["duration"].get<int64_t>());
        return exercises;
    }
    std::string restart::create_exercise(std::string_view name, std::string_view domain, int duration, bool infere)
    {
        std::lock_guard<std::recursive_mutex> lock(get_mtx());
        auto &exercise = get_coco().create_item(get_coco().get_type("CognitiveExercise"), json::json{{"name", name.data()}, {"domain", get_domain(domain).get_id()}, {"duration", duration}});

        if (infere)
            Run(get_env(), -1);

        return exercise.get_id();
    }

    coco::item &restart::get_domain(std::string_view domain)
    {
        std::string query = "(find-fact ((?cd CognitiveDomain_name)) (eq ?cd:name \"" + std::string(domain) + "\"))";
        CLIPSValue cd_fact;
        Eval(get_env(), query.c_str(), &cd_fact);
        Fact *f;
        switch (cd_fact.header->type)
        {
        case SYMBOL_TYPE:
            if (strcmp(cd_fact.lexemeValue->contents, "FALSE") == 0)
                throw std::invalid_argument("Cognitive domain not found: " + std::string(domain));
            break;
        case FACT_ADDRESS_TYPE:
            f = cd_fact.factValue;
            break;
        case MULTIFIELD_TYPE:
            if (cd_fact.multifieldValue->length == 0)
                throw std::invalid_argument("Cognitive domain not found: " + std::string(domain));
            f = cd_fact.multifieldValue->contents[0].factValue;
            break;
        default:
            throw std::invalid_argument("Unexpected result type from find-fact for domain: " + std::string(domain));
        }
        CLIPSValue cd_id;
        GetFactSlot(f, "item_id", &cd_id);
        auto &cd_itm = get_coco().get_item(cd_id.lexemeValue->contents);
        assert(cd_itm.get_type().get_name() == "CognitiveDomain");
        return cd_itm;
    }

    domain::domain(std::string_view id, std::string_view name) noexcept : id(id), name(name) {}
    json::json domain::to_json() const noexcept { return json::json{{"id", id}, {"name", name}}; }

    user::user(std::string_view id, std::string_view name, json::json &&tests) : id(id), name(name), tests(std::move(tests)) {}
    json::json user::to_json() const noexcept { return json::json{{"name", name}, {"tests", tests}}; }

    test::test(std::string_view id, std::string_view name, std::string_view domain) noexcept : id(id), name(name), domain(domain) {}
    json::json test::to_json() const noexcept { return json::json{{"id", id}, {"name", name}, {"domain", domain}}; }

    exercise::exercise(std::string_view id, std::string_view name, std::string_view domain, int64_t duration) noexcept : id(id), name(name), domain(domain), duration(duration) {}
    json::json exercise::to_json() const noexcept { return json::json{{"id", id}, {"name", name}, {"domain", domain}, {"duration", duration}}; }
} // namespace restart
