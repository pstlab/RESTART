#include "restart_server.hpp"
#include "logging.hpp"

namespace restart
{
    restart_server::restart_server(coco::coco_server &srv, restart &rst) noexcept : server_module(srv), rst(rst)
    {
        srv.add_route(network::Get, "^/users$", std::bind(&restart_server::get_users, this, network::placeholders::request));
        srv.add_route(network::Post, "^/users$", std::bind(&restart_server::new_user, this, network::placeholders::request));
        srv.add_route(network::Get, "^/tests$", std::bind(&restart_server::get_tests, this, network::placeholders::request));
        srv.add_route(network::Post, "^/tests$", std::bind(&restart_server::new_test, this, network::placeholders::request));
        srv.add_route(network::Post, "^/exercises$", std::bind(&restart_server::new_exercise, this, network::placeholders::request));

        // Define OpenAPI paths for intents and entities
        add_path("/users", {{"get",
                             {"summary", "Get all users."},
                             {"description", "Endpoint to retrieve all users."},
                             {"responses",
                              {{"200", {{"description", "List of users."}, {"content", {{"application/json", {{"schema", {{"type", "array"}, {"items", {{"type", "object"}, {"properties", {{"id", {{"type", "string"}, {"format", "uuid"}, {"description", "The UUID of the user."}}}, {"name", {{"type", "string"}, {"description", "The name of the user."}}}, {"tests", {{"type", "object"}, {"description", "An object where each key is a test name and each value is the score for that test."}}}}}}}}}}}}}}},
                               {"401", {{"description", "Unauthorized."}}}}}},
                            {"post",
                             {{"summary", "Create a new user."},
                              {"description", "Endpoint to create a new user."},
                              {"requestBody",
                               {{"required", true},
                                {"content", {{"application/json", {{"schema", {{"type", "object"}, {"properties", {{"name", {{"type", "string"}, {"description", "User name"}}}, {"tests", {{"type", "object"}, {"description", "An object where each key is a test name and each value is the score for that test."}}}}}, {"required", std::vector<json::json>{"name", "tests"}}}}}}}}}},
                              {"responses",
                               {{"201",
                                 {{"description", "User created successfully."},
                                  {"content", {{"text/plain", {{"schema", {{"type", "string"}, {"format", "uuid"}, {"description", "The UUID of the newly created user."}}}}}}}}},
                                {"400", {{"description", "Invalid request."}}}}}}}});
        add_path("/tests", {{"get",
                             {"summary", "Get all cognitive tests."},
                             {"description", "Endpoint to retrieve all cognitive tests."},
                             {"responses",
                              {{"200", {{"description", "List of cognitive tests."}, {"content", {{"application/json", {{"schema", {{"type", "array"}, {"items", {{"type", "object"}, {"properties", {{"id", {{"type", "string"}, {"format", "uuid"}, {"description", "The UUID of the cognitive test."}}}, {"name", {{"type", "string"}, {"description", "The name of the cognitive test."}}}, {"domain", {{"type", "string"}, {"description", "The name of the cognitive test domain."}}}}}}}}}}}}}}},
                               {"401", {{"description", "Unauthorized."}}}}}},
                            {"post",
                             {{"summary", "Create a new cognitive test."},
                              {"description", "Endpoint to create a new cognitive test."},
                              {"requestBody",
                               {{"required", true},
                                {"content", {{"application/json", {{"schema", {{"type", "object"}, {"properties", {{"name", {{"type", "string"}, {"description", "User name"}}}, {"tests", {{"type", "object"}, {"description", "An object where each key is a test name and each value is the score for that test."}}}}}, {"required", std::vector<json::json>{"name", "tests"}}}}}}}}}},
                              {"responses",
                               {{"201",
                                 {{"description", "User created successfully."},
                                  {"content", {{"text/plain", {{"schema", {{"type", "string"}, {"format", "uuid"}, {"description", "The UUID of the newly created cognitive test."}}}}}}}}},
                                {"400", {{"description", "Invalid request."}}}}}}}});
    }

    std::unique_ptr<network::response> restart_server::get_users(const network::request &req)
    {
        json::json users(json::json_type::array);
        for (const auto &user : rst.get_users())
            users.push_back(user.to_json());
        return std::make_unique<network::json_response>(std::move(users));
    }
    std::unique_ptr<network::response> restart_server::new_user(const network::request &req)
    {
        auto &body = static_cast<const network::json_request &>(req).get_body();
        if (!body.is_object() || !body.contains("name") || !body["name"].is_string())
            return std::make_unique<network::json_response>(json::json({{"message", "Invalid request"}}), network::status_code::bad_request);

        std::string name = body["name"];
        json::json tests;
        for (const auto &[test_name, test_score] : body.as_object())
            if (test_name != "name")
                tests[test_name] = test_score.get<int>();
        try
        {
            auto usr_id = rst.create_user(name, std::move(tests));
            return std::make_unique<network::string_response>(std::move(usr_id), network::status_code::created);
        }
        catch (const std::exception &e)
        {
            return std::make_unique<network::json_response>(json::json({{"message", e.what()}}), network::status_code::conflict);
        }
    }

    std::unique_ptr<network::response> restart_server::get_tests(const network::request &req)
    {
        json::json tests(json::json_type::array);
        for (const auto &test : rst.get_tests())
            tests.push_back(test.to_json());
        return std::make_unique<network::json_response>(std::move(tests));
    }
    std::unique_ptr<network::response> restart_server::new_test(const network::request &req)
    {
        auto &body = static_cast<const network::json_request &>(req).get_body();
        if (!body.is_object() || !body.contains("name") || !body["name"].is_string() || !body.contains("domain") || !body["domain"].is_string())
            return std::make_unique<network::json_response>(json::json({{"message", "Invalid request"}}), network::status_code::bad_request);

        std::string name = body["name"];
        std::string domain = body["domain"];
        try
        {
            auto test_id = rst.create_test(name, domain);
            return std::make_unique<network::string_response>(std::move(test_id), network::status_code::created);
        }
        catch (const std::exception &e)
        {
            return std::make_unique<network::json_response>(json::json({{"message", e.what()}}), network::status_code::conflict);
        }
    }

    std::unique_ptr<network::response> restart_server::get_exercises(const network::request &req)
    {
        json::json exercises(json::json_type::array);
        for (const auto &exercise : rst.get_exercises())
            exercises.push_back(exercise.to_json());
        return std::make_unique<network::json_response>(std::move(exercises));
    }
    std::unique_ptr<network::response> restart_server::new_exercise(const network::request &req)
    {
        auto &body = static_cast<const network::json_request &>(req).get_body();
        if (!body.is_object() || !body.contains("name") || !body["name"].is_string() || !body.contains("domain") || !body["domain"].is_string() || !body.contains("duration") || !body["duration"].is_number())
            return std::make_unique<network::json_response>(json::json({{"message", "Invalid request"}}), network::status_code::bad_request);

        std::string name = body["name"];
        std::string domain = body["domain"];
        int64_t duration = body["duration"].get<int64_t>();
        try
        {
            auto exercise_id = rst.create_exercise(name, domain, duration);
            return std::make_unique<network::string_response>(std::move(exercise_id), network::status_code::created);
        }
        catch (const std::exception &e)
        {
            return std::make_unique<network::json_response>(json::json({{"message", e.what()}}), network::status_code::conflict);
        }
    }
} // namespace restart
