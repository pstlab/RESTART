#pragma once

#include "mongo_db.hpp"

namespace restart {

  class restart_db : public coco::mongo_db {
  public:
    restart_db();

  };
} // namespace restart
