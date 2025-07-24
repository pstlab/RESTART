#pragma once

#include "coco_module.hpp"
#include <functional>
#include <memory>

namespace restart
{
  constexpr const char *next_id_defglobal = "(defglobal ?*next-id* = 1)";
  constexpr const char *exercise_deftemplate = "(deftemplate exercise (slot id) (slot exercise-type (type SYMBOL)) (slot exercise-level (type INTEGER) (range 0 6)))";
  constexpr const char *enqueue_exercise_deffunction = "(deffunction enqueue-exercise (?type ?level) (bind ?id ?*next-id*) (bind ?*next-id* (+ ?*next-id* 1)) (assert (exercise (id ?id) (exercise-type ?type) (exercise-level ?level))))";
  constexpr const char *start_session_rule = "(defrule robot_session (Robot_has_current_user (item_id ?robot) (current_user ?user)) => (add_data ?robot (create$ current_command current_modality) (create$ welcome formal)))";
  constexpr const char *start_rot_rule = "(defrule robot_rot (Robot_has_command_completed (item_id ?robot) (command_completed welcome)) => (add_data ?robot (create$ current_command) (create$ rot)))";

  class user;

  class restart : public coco::coco_module
  {
  public:
    restart(coco::coco &cc) noexcept;

    [[nodiscard]] std::vector<std::reference_wrapper<user>> get_users() noexcept;
    user &create_user(std::string_view name, json::json &&tests = json::json(), bool infere = true);

  private:
    std::unordered_map<std::string, std::unique_ptr<user>> users; // Map of user ID to user object
  };

  class user final
  {
  public:
    user(std::string_view id, std::string_view name, json::json &&tests = json::json());

    [[nodiscard]] const std::string &get_id() const { return id; }
    [[nodiscard]] const std::string &get_name() const { return name; }
    [[nodiscard]] const json::json &get_tests() const { return tests; }

    [[nodiscard]] json::json to_json() const noexcept;

  private:
    std::string id;   // The unique identifier for the user
    std::string name; // The name of the user
    json::json tests; // The tests associated with the user
  };
} // namespace restart
