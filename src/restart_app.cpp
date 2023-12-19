#include "restart_app.h"
#include "restart_db.h"

namespace restart
{
    restart_app::restart_app(restart_db &db) : coco_core(db)
    {
    }
} // namespace restart
