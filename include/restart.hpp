#pragma once

#include "coco_module.hpp"
#include <functional>
#include <memory>

namespace restart
{
  constexpr const char *next_id_defglobal = "(defglobal ?*next-id* = 1)";
  constexpr const char *exercise_deftemplate = "(deftemplate exercise (slot id) (slot type (type SYMBOL)) (slot level (type INTEGER) (range 0 6)))";
  constexpr const char *enqueue_exercise_deffunction = "(deffunction enqueue-exercise (?type ?level) (bind ?id ?*next-id*) (bind ?*next-id* (+ ?*next-id* 1)) (assert (exercise (id ?id) (type ?type) (level ?level))))";
  constexpr const char *start_session_rule = "(defrule robot_session (Robot_has_current_user (item_id ?robot) (current_user ?user)) => (add_data ?robot (create$ current_command current_modality) (create$ welcome formal)))";
  constexpr const char *start_rot_rule = "(defrule robot_rot (Robot_has_command_completed (item_id ?robot) (command_completed welcome)) => (add_data ?robot (create$ current_command) (create$ rot)))";
  constexpr const char *end_session_rule = "(defrule robot_end_session (Robot_has_command_completed (item_id ?robot) (command_completed goodbye)) => (add_data ?robot (create$ command_completed current_command current_exercise current_level current_score current_user) (create$ nil nil nil nil nil nil)))";

  class domain;
  class user;
  class test;
  class exercise;

  class restart : public coco::coco_module
  {
  public:
    restart(coco::coco &cc) noexcept;

    [[nodiscard]] std::vector<domain> get_domains() noexcept;
    std::string create_domain(std::string_view name, bool infere = true);

    [[nodiscard]] std::vector<user> get_users() noexcept;
    std::string create_user(std::string_view name, json::json &&tests = json::json(), bool infere = true);

    [[nodiscard]] std::vector<test> get_tests() noexcept;
    std::string create_test(std::string_view name, std::string_view domain, bool infere = true);

    [[nodiscard]] std::vector<exercise> get_exercises() noexcept;
    std::string create_exercise(std::string_view name, std::string_view domain, int duration, bool infere = true);
  };

  class domain final
  {
  public:
    domain(std::string_view id, std::string_view name) noexcept;

    [[nodiscard]] const std::string &get_id() const { return id; }
    [[nodiscard]] const std::string &get_name() const { return name; }

    [[nodiscard]] json::json to_json() const noexcept;

  private:
    std::string id;   // The unique identifier for the domain
    std::string name; // The name of the domain
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

  class test final
  {
  public:
    test(std::string_view id, std::string_view name, std::string_view domain) noexcept;

    [[nodiscard]] const std::string &get_id() const { return id; }
    [[nodiscard]] const std::string &get_name() const { return name; }
    [[nodiscard]] const std::string &get_domain() const { return domain; }

    [[nodiscard]] json::json to_json() const noexcept;

  private:
    std::string id;     // The unique identifier for the test
    std::string name;   // The name of the test
    std::string domain; // The domain of the test
  };

  class exercise final
  {
  public:
    exercise(std::string_view id, std::string_view name, std::string_view domain, int64_t duration) noexcept;

    [[nodiscard]] const std::string &get_id() const { return id; }
    [[nodiscard]] const std::string &get_name() const { return name; }
    [[nodiscard]] const std::string &get_domain() const { return domain; }
    [[nodiscard]] int64_t get_duration() const { return duration; }

    [[nodiscard]] json::json to_json() const noexcept;

  private:
    std::string id;     // The unique identifier for the exercise
    std::string name;   // The name of the exercise
    std::string domain; // The domain of the exercise
    int64_t duration;   // The duration of the exercise in seconds
  };
} // namespace restart
