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
 * $Id: mkdsesimulationset.h 1421 2016-06-22 07:46:32Z klugeflo $
 * @file mkdsesimulationset.h
 * @brief Set of DSE simulations for a single AbstractMkTaskset
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVALS_MKDSESIMULATIONSET_H
#define MKEVALS_MKDSESIMULATIONSET_H

#include <mkeval/abstractmktaskset.h>
#include <mkeval/concretemktaskset.h>
#include <mkeval/mkallocators.h>

//#include <bitset>
#include <map>
#include <vector>

#include <utils/bitmap.h>

namespace tmssim {

  class MkDseSimulationSet {
  public:
    MkDseSimulationSet(AbstractMkTaskset* _ats, double _u0, double _uStep,
		       const vector<const MkEvalAllocatorPair*>& _allocators,
		       const SchedulerConfiguration& _scc);


    ~MkDseSimulationSet();


    const std::vector<ConcreteMkTaskset*>& getCtss() const { return ctss; }


    void notifyFinished(ConcreteMkTaskset* cts) {
      size_t i = ctsId.at(cts);
      //finishedMap |= 1ULL << i;
      (*finishedMap)[i] = 1;
    }


    bool isFinished() const {
      //return finishedMap == (uint64_t)-1;
      return finishedMap->all();
    }

    
    /*uint64_t getFinishedMap() const {
      return finishedMap;
      }*/
    std::string getMapStr() const {
      return finishedMap->str();
    }


    const AbstractMkTaskset* getAts() const { return ats; }
    
  private:
    AbstractMkTaskset* ats;
    double u0;
    double uStep;
    const vector<const MkEvalAllocatorPair*>& allocators;
    const SchedulerConfiguration& scc;
 
    size_t nCts;
   
    std::vector<MkTaskset*> mktss;
    std::vector<ConcreteMkTaskset*> ctss;

    std::map<ConcreteMkTaskset*, size_t> ctsId;

    Bitmap* finishedMap;
  };


  void testMkDseSimulationSet(void);

  
} // NS tmssim

#endif // !MKEVALS_MKDSESIMULATIONSET_H
