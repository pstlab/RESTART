#include "restart.hpp"
#include "coco.hpp"
#include "coco_item.hpp"
#include "logging.hpp"
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
