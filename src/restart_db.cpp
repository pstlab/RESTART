#include "restart_db.h"

namespace restart
{
  restart_db::restart_db(const std::string &root, const std::string &mongodb_uri) : mongo_db(root, mongodb_uri)
  {
  }
} // namespace restart