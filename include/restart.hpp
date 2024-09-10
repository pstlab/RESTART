#pragma once

#include "coco_server.hpp"

namespace restart {

  class restart : public coco::coco_server {
  public:
    restart(const std::string &host = SERVER_HOST, unsigned short port = SERVER_PORT, const std::string &mongodb_uri = "mongodb://" MONGODB_HOST ":" MONGODB_PORT);

  };
} // namespace restart
