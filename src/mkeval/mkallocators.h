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
 */

/**
 * $Id: mkallocators.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file mkallocators.cpp
 * @brief Scheduler and Task allocators for (m,k) evals
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_MKALLOCATORS_H
#define MKEVAL_MKALLOCATORS_H

#include <core/scheduler.h>
#include <taskmodels/mktask.h>
#include <mkeval/mksimulation.h>

#include <map>


namespace tmssim {

  typedef Scheduler* (*SchedulerAllocator)(const SchedulerConfiguration&);
  typedef MkTask* (*TaskAllocator)(MkTask*);
  typedef MkSimulation* (*MkSimulationAllocator)(std::list<MkTask*> _mkTasks,
						Scheduler* _scheduler,
						const std::string& _allocId);
  
  
  /**
   * @brief Convenience Store
   */
  struct MkEvalAllocatorPair {
    // default constructor needed for allocation in map
    MkEvalAllocatorPair() {}
  MkEvalAllocatorPair(const string _id, SchedulerAllocator _schedAlloc,
		      TaskAllocator _taskAlloc, MkSimulationAllocator _simAlloc)
  : id(_id), schedAlloc(_schedAlloc), taskAlloc(_taskAlloc),
      simAlloc(_simAlloc) {
  }
    
    
  MkEvalAllocatorPair(const MkEvalAllocatorPair& rhs)
  : id(rhs.id), schedAlloc(rhs.schedAlloc), taskAlloc(rhs.taskAlloc),
      simAlloc(rhs.simAlloc) {
  }
    
    
    MkEvalAllocatorPair& operator=(const MkEvalAllocatorPair& rhs) {
      id = rhs.id;
      schedAlloc = rhs.schedAlloc;
      taskAlloc = rhs.taskAlloc;
      simAlloc = rhs.simAlloc;
      return *this;
    }
      
      string id;
    SchedulerAllocator schedAlloc;
    TaskAllocator taskAlloc;
    MkSimulationAllocator simAlloc;
  };
  
  
  class MkAllocators {
  public:

    static const MkAllocators& instance();
    const MkEvalAllocatorPair* getAllocatorPair(std::string allocatorId) const;
    std::list<std::string> listAllocators() const;
    
  private:
    static MkAllocators* _instance;

    /**
     * Guard for cleaning up memory
     * idea plucked from http://www.oop-trainer.de/Themen/Singleton.html
     */
    class Guard {
    public:
      ~Guard() {
	if (MkAllocators::_instance != NULL) {
	  delete MkAllocators::_instance;
	}
      }
    };
    friend class Guard;

    MkAllocators();
    ~MkAllocators();
    MkAllocators(__attribute__((unused)) const MkAllocators& rhs) {}
    

    std::map<std::string, MkEvalAllocatorPair> allocators;
  }; // class MkAllocators

} // NS tmssim

#endif // !MKEVAL_MKALLOCATORS_H

