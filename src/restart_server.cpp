#include "restart_server.hpp"
#include "logging.hpp"

namespace restart
{
    restart_server::restart_server(coco::coco_server &srv, restart &rst) noexcept : server_module(srv), rst(rst)
    {
        srv.add_route(network::Post, "^/users$", std::bind(&restart_server::new_user, this, network::placeholders::request));
        srv.add_route(network::Post, "^/exercises$", std::bind(&restart_server::new_exercise, this, network::placeholders::request));

        // Define OpenAPI paths for intents and entities
        get_paths()["/users"] = {{"post",
                                  {{"summary", "Create a new user."},
                                   {"description", "Endpoint to create a new user."},
                                   {"requestBody",
                                    {{"required", true},
                                     {"content", {{"application/json", {{"schema", {{"type", "object"}, {"properties", {{"name", {{"type", "string"}, {"description", "User name"}}}}}, {"required", {"name"}}, {"additionalProperties", true}}}}}}}}},
                                   {"responses",
                                    {{"201",
                                      {{"description", "User created successfully."},
                                       {"content", {{"text/plain", {{"schema", {{"type", "string"}, {"format", "uuid"}, {"description", "The UUID of the newly created user."}}}}}}}}},
                                     {"400", {{"description", "Invalid request."}}}}}}}};
    }

    std::unique_ptr<network::response> restart_server::new_user(const network::request &req) {}
    std::unique_ptr<network::response> restart_server::new_exercise(const network::request &req) {}
} // namespace restart
