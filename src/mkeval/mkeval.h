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
 * $Id: mkeval.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file mkeval.h
 * @brief Store for (m,k) evaluations
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_MKEVAL_H
#define MKEVAL_MKEVAL_H 1

#include <core/scheduler.h>
#include <core/simulation.h>
#include <core/task.h>
#include <mkeval/mkgenerator.h>
#include <mkeval/mkallocators.h>
#include <taskmodels/mktask.h>

namespace tmssim {

  /**
   * How would the eval/successmap look like if all evals are successfull
   * @param n total number of evals
   */
#define EVAL_MAP_FULL(n) ((unsigned)( ( 1 << (n) ) - 1 ))

  /**
   * One bit for the eval/successmap
   * @param n number of the evaluation
   */
#define EVAL_MAP_BIT(n) ( 1 << (n) )


  /**
   * @brief Storage for simulation results.
   */  
  struct MKSimulationResults : public SimulationResults {
    MKSimulationResults();
    MKSimulationResults(const SimulationResults& simStats);
    MKSimulationResults(const MKSimulationResults& rhs);

    MKSimulationResults& operator=(const MKSimulationResults& rhs) {
      //SimulationResults(rhs);
      *((SimulationResults*)this) = rhs;
      mkfail = rhs.mkfail;
      return *this;
    }
    
    /**
     * This value is set to true if the (m,k)-constraint of a task was
     * violated.
     */
    bool mkfail;

    friend std::ostream& operator<< (std::ostream& out, const MKSimulationResults& stat);
  };

  std::ostream& operator<< (std::ostream& out, const MKSimulationResults& stat);

  /**
   * @brief Storage space for the evaluation of one task set with several
   * schedulers.
   *
   */
  class MkEval {
  public:
    /**
     * @brief C'tor.
     *
     * @param _taskset Class takes ownership of object
     * @param _nSchedulers number of schedulers that shall be executed
     * @param _allocators Array with _nSchedulers elements, contains allocator
     * functions for schedulers and tasks.
     * @param _steps How many steps shall be simulated for each scheduler.
     */
    MkEval(MkTaskset* _taskset, size_t _nSchedulers,
	   const vector<const MkEvalAllocatorPair*>& _allocators,
	   const SchedulerConfiguration& _schedulerConfiguration,
	   TmsTime _steps);

    ~MkEval();

    /**
     * @brief Perform the actual evaluation.
     */
    void run();

    /**
     * @return an array containing the simulation results for each scheduler
     * that was specified in the constructor (MkEval::MkEval)
     */
    const MKSimulationResults* getResults() const;

    /**
     * @brief Get the successMap
     * @return the successMap of this evaluation
     */
    unsigned int getSuccessMap() const;

    /**
     * @return taskset that was simulated
     */
    const MkTaskset* getTaskset() const;

    /**
     * @brief get the simulated task set (with its states)
     *
     * Call this function only after running all simulations!
     *
     * @param i number of the simulation
     * @return The MkTasks, or NULL
     */
    const std::vector<MkTask*>* getSimTasks(size_t i) const;
    
  private:
    /**
     * @brief Prepare the simulation object for one scheduler execution
     * @param num in 0...nSchedulers-1
     */
    void prepareEval(unsigned int num);

    /**
     * @brief Perform one simulation (with one scheduler).
     * @param num in 0...nSchedulers-1
     */
    void runEval(unsigned int num);
    

    /// The taskset that is evaluated
    MkTaskset* taskset;
    /// Number of schedulers/allocator
    size_t nSchedulers;
    /// The scheduler and task allocators
    const vector<const MkEvalAllocatorPair*>& allocators;
    // Scheduler configuration
    SchedulerConfiguration schedulerConfiguration;
    /// number of simulation steps
    TmsTime steps;
    /// The simulation objects, one for each scheduler
    Simulation** simulations;


    /**
     * Store task sets also as MkTasks (need this to check for fulfillment
     * of (m,k)-condition
     */
    std::vector<MkTask*>** mkts;

    /// Simulation results
    MKSimulationResults* results;

    /**
     * The lower nSchedulers bits stand each for one simulation run.
     * A 1 indicates that the simulation was successful.
     */
    unsigned int successMap;

  };

} // NS tmssim

#endif // !MKEVAL_MKEVAL_H
