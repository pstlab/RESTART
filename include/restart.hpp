#pragma once

#include "coco_module.hpp"

namespace restart
{
  class restart : public coco::coco_module
  {
  public:
    restart(coco::coco &cc) noexcept;
  };
} // namespace restart
