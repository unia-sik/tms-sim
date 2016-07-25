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
 * $Id: mkglobals.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file mkglobals.cpp
 * @brief Global definitions for all (m,k) evaluations
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/mkglobals.h>

#include <utility/ucfirm.h>
#include <utility/uawmk.h>

namespace tmssim {

  UtilityCalculator* DefaultUCMKAllocator() {
    return new UCFirmRT;
  }


  UtilityAggregator* DefaultUAMKAllocator(unsigned int m, unsigned int k) {
    return new UAWMK(m, k);
  }


  MkTaskset::MkTaskset()
    : realUtilisation(0.0), mkUtilisation(0.0), seedUtilisation(0.0),
      targetUtilisation(0.0), suffMKSched(false) {
    //cout << "C_MkTaskset" << endl;
  }

  
  MkTaskset::~MkTaskset() {
    //cout << "~MkTaskset" << endl;
    for (MkTask* mt: tasks) {
      delete mt;
    }
    tasks.clear();
  }


  bool MkTaskset::operator==(const MkTaskset&rhs) {
    if (tasks.size() != rhs.tasks.size())
      return false;
    
    for (size_t i = 0; i < tasks.size(); ++i) {
      if (*tasks[i] != *rhs.tasks[i])
	return false;
    }
    return true;
  }
  

} // NS tmssim
