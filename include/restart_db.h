#include "mongo_db.h"

namespace restart
{
  class restart_db : public coco::mongo_db
  {
  public:
    restart_db(const std::string &root = COCO_NAME, const std::string &mongodb_uri = MONGODB_URI(MONGODB_HOST, MONGODB_PORT));
  };
} // namespace restart
