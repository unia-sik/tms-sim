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
 * $Id: mkallocators.cpp 1432 2016-07-15 10:43:04Z klugeflo $
 * @file mkallocators.cpp
 * @brief Scheduler and Task allocators for (m,k) evals
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/mkallocators.h>

#include <schedulers/schedulers.h>

#include <taskmodels/mktask.h>
#include <taskmodels/mkptask.h>
#include <taskmodels/dbptask.h>

#include <mkeval/gstsimulation.h>
#include <mkeval/mkpsimulation.h>

namespace tmssim {

  MkAllocators* MkAllocators::_instance = NULL;
  
#define ADD_ALLOCATOR(_id, _schedAlloc, _taskAlloc, _simAlloc)			\
  allocators[_id] = MkEvalAllocatorPair((_id), (_schedAlloc), (_taskAlloc), (_simAlloc))
  
  MkAllocators::MkAllocators() {
    ADD_ALLOCATOR("DBP", FPPSchedulerAllocator, DbpTaskAllocator, GstSimulationAllocator); // Hamdaoui & Ramanathan 1995
    ADD_ALLOCATOR("MKP", FPPSchedulerAllocator, MkpTaskAllocator, MkpSimulationAllocator); // Ramanathan 1999
    ADD_ALLOCATOR("MKP-S", FPPSchedulerAllocator, MkpTaskWithSpinAllocator, MkpSimulationAllocator); // Ramanathan 1999 + Quan & Hu 2000
    ADD_ALLOCATOR("rMKP", FPPSchedulerAllocator, RelaxedMkpTaskAllocator, GstSimulationAllocator); // Ramanathan 1999
    ADD_ALLOCATOR("rMKP-S", FPPSchedulerAllocator, RelaxedMkpTaskWithSpinAllocator, GstSimulationAllocator); // Ramanathan 1999 + Quan & Hu 2000
    ADD_ALLOCATOR("GDPA", GDPASchedulerAllocator, MkTaskAllocator, GstSimulationAllocator); // Cho et al. 2010
    ADD_ALLOCATOR("GDPA-S", GDPASSchedulerAllocator, MkTaskAllocator, GstSimulationAllocator); // Cho et al. 2010
    ADD_ALLOCATOR("MKU", MKUEDFSchedulerAllocator, MkTaskAllocator, GstSimulationAllocator); // Kluge et al.
    ADD_ALLOCATOR("DMU", DBPEDFSchedulerAllocator, MkTaskAllocator, GstSimulationAllocator); // com
    ADD_ALLOCATOR("GMUA-MK", GMUAMKSchedulerAllocator, MkTaskAllocator, GstSimulationAllocator); // Rhu et al. 2011
  }

  
  MkAllocators::~MkAllocators() {
  }

  
  const MkAllocators& MkAllocators::instance() {
    static MkAllocators::Guard guard;
    if (MkAllocators::_instance == NULL) {
      MkAllocators::_instance = new MkAllocators;
    }
    return *MkAllocators::_instance;
  }

  
  const MkEvalAllocatorPair* MkAllocators::getAllocatorPair(std::string allocatorId) const {
    try {
      return &allocators.at(allocatorId);
    }
    catch (out_of_range o) {
      return NULL;
    }
  }


  list<string> MkAllocators::listAllocators() const {
    list<string> al;
    for (const pair<string, MkEvalAllocatorPair>& p: allocators) {
      al.push_back(p.second.id);
    }
    return al;
  }
    


} // NS tmssim
