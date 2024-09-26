#pragma once

#include "mongo_db.hpp"

namespace restart
{
  class restart_db : public coco::mongo_db
  {
  public:
    restart_db(const std::string &mongodb_uri = MONGODB_URI(MONGODB_HOST, MONGODB_PORT));
  };
} // namespace restart
