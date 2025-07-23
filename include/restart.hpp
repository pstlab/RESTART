#pragma once

#include "coco_module.hpp"
#include <functional>

namespace restart
{
  class user;

  class restart : public coco::coco_module
  {
  public:
    restart(coco::coco &cc) noexcept;

    [[nodiscard]] std::vector<std::reference_wrapper<user>> get_users() noexcept;
    void create_user(std::string_view name, json::json &&tests = json::json(), bool infere = true);
  };

  class user final
  {
  public:
    user(std::string_view name, json::json &&tests = json::json());

    [[nodiscard]] const std::string &get_name() const { return name; }
    [[nodiscard]] const json::json &get_tests() const { return tests; }

    [[nodiscard]] json::json to_json() const noexcept;

  private:
    std::string name; // The name of the user
    json::json tests; // The tests associated with the user
  };
} // namespace restart
