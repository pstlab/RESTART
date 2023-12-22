#include "coco_listener.h"
#include "restart_app.h"

namespace restart
{
  class restart_listener : public coco_listener
  {
  public:
    restart_listener(restart_app &app) : coco_listener(app) {}
    virtual ~restart_listener() = default;
  };
} // namespace restart
