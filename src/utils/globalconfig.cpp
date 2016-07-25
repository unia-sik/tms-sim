/*
 * This file is part of tms-sim.
 *
 * Copyright 2014 University of Augsburg
 *
 * tms-sim is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tms-sim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tms-sim.  If not, see <http://www.gnu.org/licenses/>.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the copyright holder.
 */

/**
 * $Id: globalconfig.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file globalconfig.cpp
 * @brief Global configuration of tms-sim through KV-File
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utils/globalconfig.h>

#include <utils/guard.h>

#include <utils/tmsexception.h>

namespace tmssim {

  GlobalConfig* GlobalConfig::_instance = NULL;

  GlobalConfig::GlobalConfig(std::string path) :
    KvFile(path)
  {
  }

  
  /*GlobalConfig::GlobalConfig(GlobalConfig& rhs) {
    throw TMSException("Copying of tmssim::GlobalConfig is not allowed!");
    }*/
  

  int GlobalConfig::init(__attribute__((unused)) std::string path) {
    return 0;
  }

  
  const GlobalConfig* GlobalConfig::instance() {
    //static GlobalConfig::Guard<GlobalConfig> guard;
    DEFINE_GUARD(GlobalConfig);
    if (_instance == NULL) {
      throw TMSException("tmssim::GlobalConfig must be initialised before first use!");
    }
    return _instance;
  }
  


} // NS tmssim
