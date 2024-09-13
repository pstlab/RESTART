#include "restart.hpp"

#include "restart_db.hpp"

namespace restart
{
  restart::restart(const std::string &host, unsigned short port, const std::string &mongodb_uri, const std::string &transformer_host, unsigned short transformer_port) : coco_server(host, port, std::make_unique<restart_db>(mongodb_uri), transformer_host, transformer_port) {}
} // namespace restart
