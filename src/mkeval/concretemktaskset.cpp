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
 * $Id: concretemktaskset.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file concretemktaskset.cpp
 * @brief A concrete (m,k)-taskset for multiple simulations
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/concretemktaskset.h>

#include <iostream>

namespace tmssim {

  ConcreteMkTaskset::ConcreteMkTaskset(MkTaskset* _mkTaskset,
				       const vector<const MkEvalAllocatorPair*>& _allocators,
				       const SchedulerConfiguration& scc)
    : mkTaskset(_mkTaskset), nSimulations(_allocators.size()), mkSimulations(_allocators.size())
  {
    //cout << "C_ConcreteMkTaskset" << endl;
    //mkSimulations = new MkSimulation*[nSimulations];
    //finished = new bool[nSimulations];
    
    for (size_t i = 0; i < nSimulations; ++i) {
      const MkEvalAllocatorPair* ap = _allocators[i];
      list<MkTask*> tasks;
      for (MkTask* task: mkTaskset->tasks) {
	tasks.push_back(ap->taskAlloc(task));
      }
      mkSimulations[i] = ap->simAlloc(tasks, ap->schedAlloc(scc), ap->id);
      //finished[i] = false;
      simIds[mkSimulations[i]] = i;
    }
    finishedMap = ~( (1ULL << _allocators.size()) - 1 );
  }


  ConcreteMkTaskset::ConcreteMkTaskset(MkTaskset* _mkTaskset,
				       const vector<const MkEvalAllocatorPair*>& _allocators)
    : mkTaskset(_mkTaskset), nSimulations(1), mkSimulations(_allocators.size()), finishedMap(0) {
    //cout << "C_ConcreteMkTaskset_SPC" << endl;
  }

  
  ConcreteMkTaskset::~ConcreteMkTaskset() {
    //cout << "~ConcreteMkTaskset" << endl;
    /*
    for (size_t i = 0; i < nSimulations; ++i) {
      delete mkSimulations[i];
      }*/
    for (MkSimulation* mksim : mkSimulations) {
      delete mksim;
    }
    //delete mkSimulations;
    //delete mkTaskset;
  }
  
  
  void testConcreteMkTaskset(void) {
    const MkAllocators& mka = MkAllocators::instance();
    vector<const MkEvalAllocatorPair*> allocators;
    allocators.push_back(mka.getAllocatorPair("DBP"));
    allocators.push_back(mka.getAllocatorPair("MKU"));
    allocators.push_back(mka.getAllocatorPair("MKC"));
    allocators.push_back(mka.getAllocatorPair("GDPA"));
    allocators.push_back(mka.getAllocatorPair("GMUA-MK"));
    ConcreteMkTaskset cts(new MkTaskset(), allocators, SchedulerConfiguration());
    size_t n = allocators.size();
    
    cout << "n: " << n << " fm: " << strBitString(cts.getFinishedMap(), n) << endl;
    
    for (size_t i = 0; i < allocators.size(); ++i) {
      cts.notifyFinished(i);
      cout << "\ti: " << i << " fm: " << strBitString(cts.getFinishedMap(), n)
	   << " finished: " << cts.isFinished()
	   << endl;
    }

    cout << "Finished: " << cts.isFinished() << endl;
  }

} // NS tmssim
