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
 * $Id: mkglobals.h 1421 2016-06-22 07:46:32Z klugeflo $
 * @file mkglobals.h
 * @brief Global definitions for all (m,k) evaluations
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_MKGLOBALS_H
#define MKEVAL_MKGLOBALS_H

#include <taskmodels/mktask.h>
#include <utils/bitmap.h>

#include <vector>

namespace tmssim {


  /**
   * @name Program options
   * Currently not used - I remember compiling failed due to some reason.
   * Some day, someone should try this again.
   * @{
   */

#define PO_HELP ("help,h", "produce help message")

#define PO_CONFIG(string_parm) ("config,c", value<string>(&(string_parm))->required(), "KvFile with settings for the task set generator (mandatory)")

#define PO_SEED(unsigned_parm) ("seed,S", value<unsigned>(&(unsigned_parm)), "Seed [default=time(NULL)]")

#define PO_TS_SIZE(unsigned_parm) (",t", value<unsigned>(&(unsigned_parm))->default_value(5), "TASKSETSIZE")

#define PO_NSTEPS(unsigned_parm) (",n", value<unsigned>(&(unsigned_parm))->default_value(100), "SIMULATIONSTEPS")

#define PO_N_TS(unsigned_parm) (",T", value<unsigned>(&(unsigned_parm))->default_value(100), "NTASKSETS")

#define PO_THREADS(unsigned_parm) (",m", value<unsigned>(&(unsigned_parm))->default_value(thread::hardware_concurrency()), "Simulation threads")

#define PO_ALLOCATORS(string_vector_parm) (",a", value<vector<string>>(&(string_vector_parm))->required(), "Scheduler/Task allocators, for valid options see below")

#define PO_LOG_PREFIX(string_parm) (",p", value<string>(&(string_parm))->required(), "Log prefix")

#define PO_EXEC_LOGS(string_vector_parm) (",l", value<vector<string>>(&(string_vector_parm)), "Activate execution logs, for valid options see below")



  /**
   * Type for bitmaps
   * @todo implement bitmap class?
   */
  typedef unsigned long bitmap_t;

  
  /**
   * @}
   */  
  

  typedef UtilityCalculator* (*UCMKAllocator)();
  typedef UtilityAggregator* (*UAMKAllocator)(unsigned int m, unsigned int k);

  UtilityCalculator* DefaultUCMKAllocator();
  UtilityAggregator* DefaultUAMKAllocator(unsigned int m, unsigned int k);


  struct MkTaskset {
    MkTaskset();
    ~MkTaskset();

    /// check whether tasks are identical
    bool operator==(const MkTaskset&rhs);
    bool operator!=(const MkTaskset&rhs) {
      return !(*this == rhs);
    }

    unsigned int seed;
    /// @todo replace by list?
    std::vector<MkTask*> tasks;
    double realUtilisation;
    double mkUtilisation;
    double seedUtilisation;
    double targetUtilisation;
    bool suffMKSched;
  };


  /**
   * Results of a full (m,k) evaluation
   */
  struct MkResults {
  MkResults()
  : successes(0), activations(0), completions(0), cancellations(0),
      execCancellations(0), misses(0), preemptions(0), usum(0), esum(0),
      mkfails(0), mksuccs(0) {}
    unsigned int successes;
    double activations;
    double completions;
    double cancellations;
    double execCancellations;
    double misses;
    double preemptions;
    double usum;
    double esum;
    unsigned int mkfails;
    unsigned int mksuccs;
  };


  struct BdResultData {
  BdResultData()
  : breakdownUtilisation(0), realUtilisation(0), mkUtilisation(0) {}
  BdResultData(double bdu, double ru, double mku, bool succ=false)
  : breakdownUtilisation(bdu), realUtilisation(ru), mkUtilisation(mku), success(succ) {}
    
    double breakdownUtilisation;
    double realUtilisation;
    double mkUtilisation;
    bool success;
  };


  struct BdResultSet {
  /**
   * @param _seed task set seed
   * @param _n number of allocators
   */
  BdResultSet(unsigned _seed, size_t _n)
  : seed(_seed) {
    data = new BdResultData[_n];
  }
    ~BdResultSet() { delete[] data; }
    
    unsigned seed;
    BdResultData* data;
  };


  /**
   * Results of Design space exploration
   */
  struct DseResultSet {
    /**
     * @param _seed task set seed
     * @param _n number of allocators
     */
  DseResultSet(unsigned _seed, size_t _n, size_t nUtils)
  : seed(_seed), n(_n) {
      data = new Bitmap*[n];
      for (size_t i = 0; i < n; ++i) {
	data[i] = new Bitmap(nUtils);
      }
    }

    ~DseResultSet() {
      for (size_t i = 0; i < n; ++i) {
	delete data[i];
      }
      delete[] data;
    }

    unsigned seed;
    size_t n;
    Bitmap** data;
    
  };

  
} // NS tmssim

#endif // !MKEVAL_MKGLOBALS_H
