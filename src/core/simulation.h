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
 * $Id: simulation.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file simulation.h
 * @brief Encapsulation of simulations
 * @author Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 *
 * @todo
 * - revise logging
 */

#ifndef CORE_SIMULATION_H
#define CORE_SIMULATION_H 1

#include <core/primitives.h>
#include <core/task.h>
#include <core/scheduler.h>
#include <utils/logger.h>

#include <vector>
#include <sstream>


// Cancel Statistics:
// how often got $i tasks cancelled in one time step
// CANCEL_SLOTS sets the size of the different time steps
// A Value of 10 would result that even jobs are reported that are cancelled 10 times in a row
/**
 * @todo This seems to be BS, are we really interested in these data?
 * Maybe it might be better to use a map as cancelCtrl?
 */
//#define CANCEL_SLOTS 1000

namespace tmssim {

  // Forward declaration
  class Stat;


  struct SimulationResults {
    SimulationResults(Taskset* ts = NULL);
    SimulationResults(const SimulationResults& rhs);

    TmsTime simulatedTime;
    bool success; ///< simulation has run until the end
    unsigned int activations;
    unsigned int completions;
    unsigned int cancellations;
    unsigned int execCancellations;
    TmsTime ecPerformanceLost;
    unsigned int misses;
    unsigned int preemptions;
    unsigned int usum;
    unsigned int esum;
    unsigned int cancelSteps;
    unsigned int idleSteps;
    Taskset* taskset;
    std::string schedulerId;

    friend std::ostream& operator<< (std::ostream& out, const SimulationResults& stat);
  };

  std::ostream& operator<< (std::ostream& out, const SimulationResults& stat);
  
  class Simulation {
  public:

    /**
     * Bit field with exit conditions for simulation
     */
    typedef uint32_t ExitCondition;
    
    /**
     * @brief Exit conditions for 
     */
    enum EcBit {
      EC_INIT_STEP = 0x1,
      EC_SCHEDULE = 0x2,
      EC_CANCEL = 0x4,
      EC_DISPATCH = 0x8
    };
         
    /**
     * @brief C'tor
     * @param _taskset The taskset on which the simulations operates (takes
     * ownership)
     * @param _scheduler The scheduler that should be used for this simulation
     * (takes ownership)
     */
    Simulation(Taskset* _taskset, Scheduler* _scheduler, ExitCondition _exitCondition = 0xffffffff);

    
    /**
     * @brief D'tor
     */
    ~Simulation();

    
    /**
     * Starts the simulation
     * @param steps The maximum number of time-steps that simulations
     * should run
     * @return 0, if all steps could be simulated (without encountering
     * an exit condition specified in the constructor); else, the encountered
     * exit condition is returned.
     */
    Simulation::ExitCondition run(TmsTimeInterval steps);

    /**
     * Finalise the simulation by executing all currently active jobs.
     * No new jobs are activated in this phase.
     * @return 0, if all steps could be simulated (without encountering
     * an exit condition specified in the constructor); else, the encountered
     * exit condition is returned.
     */
    Simulation::ExitCondition finalise();
    
    /**
     * Get the current simulation results
     * @return the simulation results
     */
    const SimulationResults getResults();

    
    /**
     * @brief Get the taskset, e.g. to read task statistics
     * @return Pointer to the simulation's taskset - is only valid as long
     * as the Simulation object exists
     */
    const Taskset* getTaskset() const;

    TmsTime getTime() const { return now; }


    class SimulationException {
    public:
      SimulationException(std::string _msg = "")
	: msg(_msg) {}
      const std::string& getMessage() const { return msg; }
    private:
      std::string msg;
    };


  private:   
    
    /**
     * Methods
     */

    /**
     * @brief Prepare scheduler for current step.
     *
     */
    ExitCondition initStep();
    
    /**
     * @brief Activate pending jobs of a taskset.
     *
     * This functions does not return any value.
     * @param time The time-step at which you want to activate
     * @param sched Pointer to the scheduler that provides the
     * scheduling algorithm
     * @param log Outputstream to store log information in
     */
    void doActivations();
    // @return true, if there are jobs to be executed, false otherwise
    // @todo return more elaborate error codes
    
    /**
     * Applies the scheduling algorithm to the pending jobs and tries to
     * complete them
     * @param time The current time-step
     * @param sched Pointer to the scheduler that provides the
     * scheduling algorithm
     * @param log Outputstream to store log information in
     * @param statPtr Pointer to a statistics object that should store
     * the cancelStepList
     * @return Errors that occurred during execution
     * @todo return more elaborate error codes
     */
    ExitCondition doExecutions();


    /**
     * @brief Perfrom cancellations a scheduler decided on.
     *
     * @param scStat
     * @return Cancellation errors
     */
    bool performCancellations(const ScheduleStat& scStat);
    
    /**
     * Prints the executions status of all tasks in the simulations
     * taskset to the console
     * @param head custom header of the output
     */
    void printExecStats(const std::string& head);
    
    /**
     * Inits the delay counters variables. Should be called before using them.
     */
    void initCounters(void);
    
    /**
     * Prints information about the delay counters to the console.
     * Delay counters mean how many times a job gets delayed (and not
     * dispatched) in row
     */
    void printDelayCounters(void);
    
    /**
     * stores information about this simulation into a statistics object
     * that can be used independently of this simulation
     * @param statistics Pointer to a statistics object where to
     * information gets stored into
     */
    void storeStatistics(Stat* statistics);
    

    /**
     * @brief Calculate execution statistics
     */
    void calculateStatistics();

    
    /**
     * @brief The taskset on which the simulation will run
     */
    Taskset* taskset;
    
    /**
     * @brief The scheduler which provides the scheduling algorithm
     */
    Scheduler* scheduler;

    /**
     * @brief Exit conditions
     */
    ExitCondition exitCondition;
    
    /**
     * @brief The amount of time-ticks to simulate
     */
    //int steps;

    
    /**
     * @brief Logging/tracing of execution
     */
    //Logger* logger;

    /**
     * cancelCtr[i] = k means there were k time steps in which exactly i
     * jobs were cancelled
     */
    //int cancelCtr[CANCEL_SLOTS];

    /**
     * count in how many time steps jobs were cancelled
     */
    int cancelSteps;

    /**
     * count in how many time steps no job was executed
     */
    int idleSteps;

    /**
     * Numbers of the steps in which cancellations happened
     */
    std::list<int> cancelStepList;

    SimulationResults stats;

    /// Current time of simulation
    TmsTime now;
    
    bool finalised;
    
  };
  
} // NS tmssim

#endif /* CORE_SIMULATION_H */
