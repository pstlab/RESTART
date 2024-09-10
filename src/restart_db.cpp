#include "restart_db.hpp"

namespace restart
{
  restart_db::restart_db(const std::string &mongodb_uri) : mongo_db({{"name", COCO_NAME}}, mongodb_uri) {}
} // namespace restart
