/**
 * $Id: dummy.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file dummy.cpp
 * @brief Dummy scheduler that does nothing
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/dummy.h>

namespace tmssim {

  static const std::string myId = "DUMMYcheduler";

  const std::string& DummyScheduler::getId(void) const {
    return myId;
  }

} // NS tmssim
