#pragma once

#include "coco_core.h"
#include "server.hpp"

namespace restart
{
  class restart_db;

  class restart_app : public coco::coco_core, public network::server
  {
  public:
    restart_app(restart_db &db, const std::string &dashboard_host = DASHBOARD_HOST, const unsigned short dashboard_port = DASHBOARD_PORT);
  };
} // namespace restart
