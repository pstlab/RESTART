#pragma once

#include "coco_server.hpp"
#include "restart.hpp"

namespace restart
{
  class restart_server : public coco::server_module
  {
  public:
    restart_server(coco::coco_server &srv, restart &rst) noexcept;

  private:
    std::unique_ptr<network::response> get_domains(const network::request &req);
    std::unique_ptr<network::response> new_domain(const network::request &req);

    std::unique_ptr<network::response> get_users(const network::request &req);
    std::unique_ptr<network::response> new_user(const network::request &req);

    std::unique_ptr<network::response> get_tests(const network::request &req);
    std::unique_ptr<network::response> new_test(const network::request &req);

    std::unique_ptr<network::response> get_exercises(const network::request &req);
    std::unique_ptr<network::response> new_exercise(const network::request &req);

  private:
    restart &rst; // Reference to the restart module
  };
} // namespace restart
