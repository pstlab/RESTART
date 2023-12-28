#include "restart_app.h"
#include "restart_db.h"

namespace restart
{
    restart_app::restart_app(restart_db &db, const std::string &dashboard_host, const unsigned short dashboard_port) : coco_core(db), server(dashboard_host, dashboard_port)
    {
    }
} // namespace restart
