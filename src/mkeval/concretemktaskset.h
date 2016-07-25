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
 * $Id: concretemktaskset.h 1421 2016-06-22 07:46:32Z klugeflo $
 * @file concretemktaskset.h
 * @brief A concrete (m,k)-taskset for multiple simulations
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_CONCRETEMKTASKSET_H
#define MKEVAL_CONCRETEMKTASKSET_H

#include <mkeval/mkallocators.h>
#include <mkeval/mkglobals.h>
#include <mkeval/mksimulation.h>
#include <utils/bitstrings.h>

#include <map>

namespace tmssim {
  
  class ConcreteMkTaskset {
  public:
    ConcreteMkTaskset(MkTaskset* _mkTaskset,
		      const vector<const MkEvalAllocatorPair*>& _allocators,
		      const SchedulerConfiguration& scc);
    
    virtual ~ConcreteMkTaskset();
    
    const vector<MkSimulation*>& getMkSimulations() const {
      return mkSimulations;
    }
    
    void notifyFinished(size_t i) {
      finishedMap |= 1ULL << i;
    }


    void notifyFinished(MkSimulation* mkSimulation) {
      size_t i = simIds.at(mkSimulation);
      finishedMap |= 1ULL << i;
    }

    
    bool isFinished() const {
      return finishedMap == (uint64_t)-1;
    }
    
    // only for testing
    uint64_t getFinishedMap() const {
      return finishedMap;
    }

    const MkTaskset* getMkTaskset() const { return mkTaskset; }

  protected:
    /**
     * @brief use this constructor only if you really know what you're doing!
     *
     * This constructor is a quite dirty hack to save some time/memory during
     * the creation of a DummyConcreteMkTaskset. For that one, no actual
     * Simulation objects need to be created. The constructor does NOT result
     * in a working CTS! This hack is (currently) needed for skipping
     * "duplicate" CTSs during actual simulation and still retain full results.
     *
     * This hack is REALLY dirty, it took me >1h to find the memory leak,
     * so we REALLY have to find a better way to implement this stuff.
     *
     * @todo revise class hierarchies!
     */
    ConcreteMkTaskset(MkTaskset* _mkTaskset,
		      const vector<const MkEvalAllocatorPair*>& _allocators);


  private:
    MkTaskset* mkTaskset;
    const size_t nSimulations;
    //bool* finished;
  protected:
    /// Also dirty hack, should actually be private
    std::vector<MkSimulation*> mkSimulations;
    /// Also dirty hack, should actually be private
    uint64_t finishedMap;
    /// Also dirty hack, should actually be private
    std::map<MkSimulation*, size_t> simIds;
  };


  void testConcreteMkTaskset(void);

} // NS tmssim

#endif // !MKEVAL_CONCRETEMKTASKSET_H
