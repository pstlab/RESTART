#pragma once

#include "coco_server.hpp"

namespace restart
{
  class restart_server : public coco::server_module
  {
  public:
    restart_server(coco::coco_server &srv) noexcept;

  private:
    std::unique_ptr<network::response> new_user(const network::request &req);
    std::unique_ptr<network::response> new_exercise(const network::request &req);
  };
} // namespace restart
