#include "restart_server.hpp"
#include "logging.hpp"

namespace restart
{
    restart_server::restart_server(coco::coco_server &srv) noexcept : server_module(srv)
    {
        srv.add_route(network::Post, "^/users$", std::bind(&restart_server::new_user, this, network::placeholders::request));
        srv.add_route(network::Post, "^/exercises$", std::bind(&restart_server::new_exercise, this, network::placeholders::request));

        // Define OpenAPI paths for restart endpoints
        get_schemas()["/users"] = {"post",
                                   {{"summary", "Add a new user."},
                                    {"description", "Endpoint to add a new user."},
                                    {"requestBody",
                                     {{"required", true},
                                      {"content", {{"application/json", {{"schema", {{"type", "object"}, {"properties", {{"name", {{"type", "string"}, {"description", "User name"}}}}}, {"required", {"name"}}, {"additionalProperties", true}}}}}}}}},
                                    {"responses",
                                     {{"204",
                                       {{"description", "User added successfully."}}},
                                      {"400",
                                       {{"description", "Invalid request."}}}}}}};
    }

    std::unique_ptr<network::response> restart_server::new_user(const network::request &req) {}
    std::unique_ptr<network::response> restart_server::new_exercise(const network::request &req) {}
} // namespace restart
