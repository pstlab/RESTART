#include "restart_server.hpp"
#include "logging.hpp"

namespace restart
{
    restart_server::restart_server(coco::coco_server &srv, restart &rst) noexcept : server_module(srv), rst(rst)
    {
        // Register routes for cognitive domains, users, tests, and exercises
        srv.add_route(network::Get, "^/cognitive_domains$", std::bind(&restart_server::get_domains, this, network::placeholders::request));
        srv.add_route(network::Post, "^/cognitive_domains$", std::bind(&restart_server::new_domain, this, network::placeholders::request));
        srv.add_route(network::Get, "^/users$", std::bind(&restart_server::get_users, this, network::placeholders::request));
        srv.add_route(network::Post, "^/users$", std::bind(&restart_server::new_user, this, network::placeholders::request));
        srv.add_route(network::Get, "^/cognitive_tests$", std::bind(&restart_server::get_tests, this, network::placeholders::request));
        srv.add_route(network::Post, "^/cognitive_tests$", std::bind(&restart_server::new_test, this, network::placeholders::request));
        srv.add_route(network::Get, "^/cognitive_exercises$", std::bind(&restart_server::get_exercises, this, network::placeholders::request));
        srv.add_route(network::Post, "^/cognitive_exercises$", std::bind(&restart_server::new_exercise, this, network::placeholders::request));

        // Define OpenAPI paths for intents and entities
        add_path("/cognitive_domains", {{"get",
                                         {{"summary", "Get all cognitive domains."},
                                          {"description", "Endpoint to retrieve all cognitive domains."},
                                          {"responses",
                                           {{"200", {{"description", "List of cognitive domains."}, {"content", {{"application/json", {{"schema", {{"type", "array"}, {"items", {{"type", "object"}, {"properties", {{"id", {{"type", "string"}, {"format", "uuid"}, {"description", "The UUID of the cognitive domain."}}}, {"name", {{"type", "string"}, {"description", "The name of the cognitive domain."}}}}}}}}}}}}}}},
                                            {"401", {{"description", "Unauthorized."}}}}}}},
                                        {"post",
                                         {{"summary", "Create a new cognitive domain."},
                                          {"description", "Endpoint to create a new cognitive domain."},
                                          {"requestBody",
                                           {{"required", true},
                                            {"content", {{"application/json", {{"schema", {{"type", "object"}, {"properties", {{"name", {{"type", "string"}, {"description", "Cognitive domain name"}}}}}, {"required", {"name"}}}}}}}}}},
                                          {"responses",
                                           {{"201",
                                             {{"description", "Cognitive domain created successfully."},
                                              {"content", {{"text/plain", {{"schema", {{"type", "string"}, {"format", "uuid"}, {"description", "The UUID of the newly created cognitive domain."}}}}}}}}},
                                            {"400", {{"description", "Invalid request."}}}}}}}});
        add_path("/users", {{"get",
                             {{"summary", "Get all users."},
                              {"description", "Endpoint to retrieve all users."},
                              {"responses",
                               {{"200", {{"description", "List of users."}, {"content", {{"application/json", {{"schema", {{"type", "array"}, {"items", {{"type", "object"}, {"properties", {{"id", {{"type", "string"}, {"format", "uuid"}, {"description", "The UUID of the user."}}}, {"name", {{"type", "string"}, {"description", "The name of the user."}}}, {"tests", {{"type", "object"}, {"description", "An object where each key is a test name and each value is the score for that test."}}}}}}}}}}}}}}},
                                {"401", {{"description", "Unauthorized."}}}}}}},
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
        add_path("/cognitive_tests", {{"get",
                                       {{"summary", "Get all cognitive tests."},
                                        {"description", "Endpoint to retrieve all cognitive tests."},
                                        {"responses",
                                         {{"200", {{"description", "List of cognitive tests."}, {"content", {{"application/json", {{"schema", {{"type", "array"}, {"items", {{"type", "object"}, {"properties", {{"id", {{"type", "string"}, {"format", "uuid"}, {"description", "The UUID of the cognitive test."}}}, {"name", {{"type", "string"}, {"description", "The name of the cognitive test."}}}, {"domain", {{"type", "string"}, {"description", "The name of the cognitive test domain."}}}}}}}}}}}}}}},
                                          {"401", {{"description", "Unauthorized."}}}}}}},
                                      {"post",
                                       {{"summary", "Create a new cognitive test."},
                                        {"description", "Endpoint to create a new cognitive test."},
                                        {"requestBody",
                                         {{"required", true},
                                          {"content", {{"application/json", {{"schema", {{"type", "object"}, {"properties", {{"name", {{"type", "string"}, {"description", "Cognitive test name"}}}, {"domain", {{"type", "string"}, {"description", "Cognitive test domain"}}}}}, {"required", std::vector<json::json>{"name", "domain"}}}}}}}}}},
                                        {"responses",
                                         {{"201",
                                           {{"description", "Cognitive test created successfully."},
                                            {"content", {{"text/plain", {{"schema", {{"type", "string"}, {"format", "uuid"}, {"description", "The UUID of the newly created cognitive test."}}}}}}}}},
                                          {"400", {{"description", "Invalid request."}}}}}}}});
        add_path("/cognitive_exercises", {{"get",
                                           {{"summary", "Get all cognitive exercises."},
                                            {"description", "Endpoint to retrieve all cognitive exercises."},
                                            {"responses",
                                             {{"200", {{"description", "List of cognitive exercises."}, {"content", {{"application/json", {{"schema", {{"type", "array"}, {"items", {{"type", "object"}, {"properties", {{"id", {{"type", "string"}, {"format", "uuid"}, {"description", "The UUID of the cognitive exercise."}}}, {"name", {{"type", "string"}, {"description", "The name of the cognitive exercise."}}}, {"domain", {{"type", "string"}, {"description", "The name of the cognitive exercise domain."}}}, {"duration", {{"type", "number"}, {"description", "The duration of the cognitive exercise."}}}}}}}}}}}}}}},
                                              {"401", {{"description", "Unauthorized."}}}}}}},
                                          {"post",
                                           {{"summary", "Create a new cognitive exercise."},
                                            {"description", "Endpoint to create a new cognitive exercise."},
                                            {"requestBody",
                                             {{"required", true},
                                              {"content", {{"application/json", {{"schema", {{"type", "object"}, {"properties", {{"name", {{"type", "string"}, {"description", "Cognitive exercise name"}}}, {"domain", {{"type", "string"}, {"description", "Cognitive domain name"}}}, {"duration", {{"type", "number"}, {"description", "Cognitive exercise duration"}}}}}, {"required", std::vector<json::json>{"name", "domain", "duration"}}}}}}}}}},
                                            {"responses",
                                             {{"201",
                                               {{"description", "Cognitive exercise created successfully."},
                                                {"content", {{"text/plain", {{"schema", {{"type", "string"}, {"format", "uuid"}, {"description", "The UUID of the newly created cognitive exercise."}}}}}}}}},
                                              {"400", {{"description", "Invalid request."}}}}}}}});
    }

    std::unique_ptr<network::response> restart_server::get_domains(const network::request &)
    {
        json::json domains(json::json_type::array);
        for (const auto &domain : rst.get_domains())
            domains.push_back(domain.to_json());
        return std::make_unique<network::json_response>(std::move(domains));
    }
    std::unique_ptr<network::response> restart_server::new_domain(const network::request &req)
    {
        auto &body = static_cast<const network::json_request &>(req).get_body();
        if (!body.is_object() || !body.contains("name") || !body["name"].is_string())
            return std::make_unique<network::json_response>(json::json({{"message", "Invalid request"}}), network::status_code::bad_request);

        std::string name = body["name"];
        try
        {
            auto domain_id = rst.create_domain(name);
            return std::make_unique<network::string_response>(std::move(domain_id), network::status_code::created);
        }
        catch (const std::exception &e)
        {
            return std::make_unique<network::json_response>(json::json({{"message", e.what()}}), network::status_code::conflict);
        }
    }

    std::unique_ptr<network::response> restart_server::get_users(const network::request &)
    {
        json::json users(json::json_type::array);
        for (const auto &user : rst.get_users())
            users.push_back(user.to_json());
        return std::make_unique<network::json_response>(std::move(users));
    }
    std::unique_ptr<network::response> restart_server::new_user(const network::request &req)
    {
        auto &body = static_cast<const network::json_request &>(req).get_body();
        if (!body.is_object() || !body.contains("name") || !body["name"].is_string() || !body.contains("tests") || !body["tests"].is_object())
            return std::make_unique<network::json_response>(json::json({{"message", "Invalid request"}}), network::status_code::bad_request);

        std::string name = body["name"];
        json::json tests;
        for (const auto &[test_name, test_score] : body["tests"].as_object())
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

    std::unique_ptr<network::response> restart_server::get_tests(const network::request &)
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

    std::unique_ptr<network::response> restart_server::get_exercises(const network::request &)
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
