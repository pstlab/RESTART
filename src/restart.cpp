#include "restart.hpp"
#include "coco.hpp"

namespace restart
{
    restart::restart(coco::coco &cc) noexcept : coco_module(cc) {}

    std::vector<std::reference_wrapper<user>> restart::get_users() noexcept
    {
        std::vector<std::reference_wrapper<user>> users;
        return users;
    }

    void restart::create_user(std::string_view name, json::json &&tests, bool infere)
    {
        std::lock_guard<std::recursive_mutex> lock(get_mtx());
        auto &usr_type = get_coco().get_type("User");
        auto &ct_type = get_coco().get_type("CognitiveTest");
    }

    user::user(std::string_view name, json::json &&tests) : name(name), tests(std::move(tests)) {}
    json::json user::to_json() const noexcept { return json::json{{"name", name}, {"tests", tests}}; }
} // namespace restart
