#pragma once

#include "coco_core.h"

namespace restart
{
  class restart_db;

  class restart_app : public coco::coco_core
  {
  public:
    restart_app(restart_db &db);
  };
} // namespace restart
