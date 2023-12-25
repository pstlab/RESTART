#pragma once

#include "coco_core.h"
#include "server.hpp"

namespace restart
{
  class restart_db;

  class restart_app : public coco::coco_core, public network::server
  {
  public:
    restart_app(restart_db &db);
  };
} // namespace restart
