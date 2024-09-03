#include "restart.hpp"

#include "restart_db.hpp"

namespace restart {

  restart::restart() : coco_server(std::make_unique<restart_db>()) {
  }
} // namespace restart
