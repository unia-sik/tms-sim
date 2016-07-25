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
 * $Id: simulation.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file simulation.cpp
 * @brief Implementation of simulation class
 * @author Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <core/simulation.h>
//#include <core/stat.h>
#include <utils/tlogger.h>

#include <cassert>
#include <iostream>
#include <vector>
#include <sstream>


using namespace std;

namespace tmssim {

  
  SimulationResults::SimulationResults(Taskset* ts)
    : simulatedTime(0),
      success(false),
      activations(0),
      completions(0),
      cancellations(0),
      execCancellations(0),
      ecPerformanceLost(0),
      misses(0),
      preemptions(0),
      usum(0),
      esum(0),
      cancelSteps(0),
      idleSteps(0),
      taskset(ts),
      schedulerId("")
  {
  }


  SimulationResults::SimulationResults(const SimulationResults& rhs)
    : simulatedTime(rhs.simulatedTime),
      success(rhs.success),
      activations(rhs.activations),
      completions(rhs.completions),
      cancellations(rhs.cancellations),
      execCancellations(rhs.execCancellations),
      ecPerformanceLost(rhs.ecPerformanceLost),
      misses(rhs.misses),
      preemptions(rhs.preemptions),
      usum(rhs.usum),
      esum(rhs.esum),
      cancelSteps(rhs.cancelSteps),
      idleSteps(rhs.idleSteps),
      taskset(rhs.taskset),
      schedulerId(rhs.schedulerId)
  {
  }


  std::ostream& operator<< (std::ostream& out, const SimulationResults& stats) {
    out << "Act: " << stats.activations
	<< " Compl: " << stats.completions
	<< " Canc: " << stats.cancellations
	<< " Ecanc: " << stats.execCancellations
	<< " ecLost: " << stats.ecPerformanceLost
	<< " Miss: " << stats.misses
	<< " Preempt: " << stats.preemptions
	<< " CancelS: " << stats.cancelSteps
	<< " IdleS: " << stats.idleSteps << endl;
    out << "U_Sys/ [" << stats.usum
	<< "/" << stats.esum
	<< "=" << ((float)stats.usum / stats.esum) << "]" << endl;
    return out;
  }


  Simulation::Simulation(Taskset* _taskset, Scheduler* _scheduler, ExitCondition _exitCondition) :
    taskset(_taskset), scheduler(_scheduler), exitCondition(_exitCondition), //steps(_steps),
    stats(_taskset), now(0), finalised(false)
    
  {
    cancelSteps = 0;
    idleSteps = 0;
    stats.schedulerId = scheduler->getId();

    for (Task* t : *taskset) {
      LOG(LOG_CLASS_SIMULATION) << "Task: " << *t;
    }

    initCounters();

    for (size_t taskNum = 0; taskNum< (*taskset).size(); taskNum++) {
      (*taskset)[taskNum]->start(0);
    }
    now = 0;
  }


  Simulation::~Simulation() {
    delete scheduler;
    for (Taskset::iterator it = taskset->begin();
	 it != taskset->end(); ++it) {
      delete *it;
    }
    delete taskset;
  }

  
  Simulation::ExitCondition Simulation::run(TmsTimeInterval steps) {
    if (finalised) {
      throw SimulationException("Cannot run simulation as it is finalised already!");
    }

    /*
    LOG(LOG_CLASS_SIMULATION) << "######## Continuing simulation with scheduler " << scheduler->getId();
    LOG(LOG_CLASS_SIMULATION) << "\tAim are " << steps << " iterations";
    */

    Simulation::ExitCondition ec = 0;
    TmsTime start = now;
    TmsTime end = start + steps;

    LOG(LOG_CLASS_SIMULATION) << "Simulate from " << start << " for " << steps << " steps until " << end;
      
    for ( ; now < end; ++now) {
      LOG(LOG_CLASS_SIMULATION) << "T : " << now;

      ec = initStep();
      if (ec != 0) {
	LOG(LOG_CLASS_SIMULATION) << "InitStep failed in regular time step " << now << " (ec: " << ec << ")";
	break;
      }

      doActivations();

      ec = doExecutions();
      if (ec != 0) {
	LOG(LOG_CLASS_SIMULATION) << "Executions failed in regular time step " << now << " (ec: " << ec << ")";
	break;
      }
    }
    LOG(LOG_CLASS_SIMULATION) << "Totally simulated time: " << (now - start);
    
    
    // Save the statistics in a other object
    //storeStatistics(statistics);

    /*
    calculateStatistics();
    stats.simulatedTime = now;
    */
    // store this one if intermediate results are requested
    stats.success = ec == 0;
    return ec;
  }


  Simulation::ExitCondition Simulation::finalise() {
    if (finalised) {
      throw SimulationException("Simulation is already finalised!");
    }
    
    Simulation::ExitCondition ec = 0;
    while (scheduler->hasPendingJobs()) {
      ec = initStep();
      if (ec != 0) {
	LOG(LOG_CLASS_SIMULATION) << "Executions failed in regular time step " << now;
	break;
      }
      ec = doExecutions();
      ++now;
      if (ec != 0) {
	LOG(LOG_CLASS_SIMULATION) << "Executions failed in during cleanup in step " << now;
	break;
      }
    }
    
    printExecStats(scheduler->getId());
    printDelayCounters();

    finalised = true;
    
    return ec;
  }


  const SimulationResults Simulation::getResults() {
    calculateStatistics();
    stats.simulatedTime = now;
    return stats;
  }


  const Taskset* Simulation::getTaskset() const {
    return taskset;
  }

  
  void Simulation::initCounters(void) {
    /*for (int i = 0; i < CANCEL_SLOTS; ++i) {
      cancelCtr[i] = 0;
      }*/
    cancelSteps = 0;
    idleSteps = 0;
    cancelStepList.clear();
  }


  Simulation::ExitCondition Simulation::initStep() {
    ScheduleStat scStat;
    //LOG(LOG_CLASS_SIMULATION) << "initStep";
    Simulation::ExitCondition myRv = 0;
    int scrv = scheduler->initStep(now, scStat);
    if (scrv != 0 && ((exitCondition & Simulation::EC_INIT_STEP) != 0)) {
      myRv = Simulation::EC_INIT_STEP;
    }
    if (myRv == 0 && scStat.cancelled.size() > 0) {
      bool rv = true;
      LOG(LOG_CLASS_SIMULATION) << "Have initStep cancellations!";
      rv = performCancellations(scStat);
      if (!rv && ((exitCondition & Simulation::EC_CANCEL) != 0)) {
	myRv = Simulation::EC_CANCEL;
      }
    }
    //LOG(LOG_CLASS_SIMULATION) << "/initStep";
    return myRv;
  }

  
  void Simulation::doActivations() {
    tDebug() << "\nActivations [" << now << "]";
    //bool rv = false;
    list<Job*> actList;
    for (size_t i = 0; i < taskset->size(); i++) {
      Job* job = NULL;
      job = (*taskset)[i]->spawnJob(now);
      if (job != NULL) {
	scheduler->enqueueJob(job);
	actList.push_back(job);
	//rv = true;
      }
    }
    if (actList.size() > 0) {
      ostringstream oss;
      oss << "A@" << now << " :";
      ostringstream osb;
      osb << "\t";
      //log << "A@" << now << " :";
      for (list<Job*>::iterator it = actList.begin(); it != actList.end(); ++it) {
	oss << " {" << *(*it) << "}";
	osb << *it << " ";
      }
      LOG(LOG_CLASS_EXEC) << oss.str();
      tDebug() << osb.str();
    }
    //return rv;
  }

  
  Simulation::ExitCondition Simulation::doExecutions() {
    tDebug() << "Executions [" << now << "]";

    // Schedule
    ScheduleStat scStat;
    int scrv = scheduler->schedule(now, scStat);
    if (scrv != 0) {
      LOG(LOG_CLASS_SIMULATION) << "Schedule failed: " << scrv;
      //return false;
      if ((exitCondition & Simulation::EC_SCHEDULE) != 0)
	return Simulation::EC_SCHEDULE;
    }

    
    if (scStat.cancelled.size() > 0) {
      bool rv = true;
      LOG(LOG_CLASS_SIMULATION) << "Have schedule cancellations!";
      rv = performCancellations(scStat);
      /*
      bool rv = true;
      int ctr = 0;
      ostringstream oss;
      oss << "C@" << now << " :";
      for (list<Job*>::iterator it = scStat.cancelled.begin(); it != scStat.cancelled.end(); ++it) {
	Job* cjob = *it;
	//cout << "\tcanceling job " << cjob << " " << *cjob;
	oss << " {" << *cjob << "}";
	Task *task = cjob->getTask();
	rv &= task->cancelJob(cjob);
	ctr++;
      }
      LOG(LOG_CLASS_EXEC) << oss.str();
      //assert(ctr <= CANCEL_SLOTS);
      //++cancelCtr[ctr - 1];
      ++cancelSteps;
      //statPtr->addToCancelStepList(now);
      cancelStepList.push_back(now);
      */
      if (!rv && ((exitCondition & Simulation::EC_CANCEL) != 0)) {
	return Simulation::EC_CANCEL;
      }
    }
    
    // Dispatch
    Job* job = NULL;
    DispatchStat dispStat;
    job = scheduler->dispatch(now, dispStat);
    ostringstream oss;
    oss << "E@" << now << " : ";
    if (dispStat.idle) {
      oss << "I";
      ++idleSteps;
    }
    else {
      if (dispStat.executed != NULL)
	oss << "{" << *dispStat.executed << "} ";
      else
	oss << "EXEC FAIL";
    }
    
    if ((long int) job < 0) {
      oss << "\tDispatching failed: " << (long int) job;
      if ((exitCondition & Simulation::EC_DISPATCH) != 0)
	return Simulation::EC_DISPATCH;
    }
    else if (job != NULL) { // equiv to dispStat->finished != NULL
      assert(job == dispStat.finished);
      // TODO: Task-Specific notification symbols
      // TODO: match with \(([A-Z])(,[A-Z])*\)
      oss << "(F";
      if (dispStat.dlMiss) {
	oss << ",M";
      }
      oss << ") ";
      LOG(LOG_CLASS_EXEC) << oss.str();
      Task *task = job->getTask();
      task->completeJob(job, now);
      return 0;
    }
    else {
      LOG(LOG_CLASS_EXEC) << oss.str();
      return 0;
    }
    return 0;
  }


  bool Simulation::performCancellations(const ScheduleStat& scStat) {
    bool rv = true;
    int ctr = 0;
    ostringstream oss;
    oss << "C@" << now << " :";
    for (list<Job*>::const_iterator it = scStat.cancelled.begin(); it != scStat.cancelled.end(); ++it) {
      Job* cjob = *it;
      //cout << "\tcanceling job " << cjob << " " << *cjob;
      oss << " {" << *cjob << "}";
      Task *task = cjob->getTask();
      rv &= task->cancelJob(cjob);
      ctr++;
    }
    LOG(LOG_CLASS_EXEC) << oss.str();
    //assert(ctr <= CANCEL_SLOTS);
    //++cancelCtr[ctr - 1];
    ++cancelSteps;
    //statPtr->addToCancelStepList(now);
    cancelStepList.push_back(now);
    return rv;
    /*if (!rv) { // && ((exitCondition & Simulation::EC_CANCEL) != 0)) {
      return Simulation::EC_CANCEL;
    }
    else {
      return 0;
      }*/
  }
  

  void Simulation::calculateStatistics() {
    stats.activations = 0;
    stats.completions = 0;
    stats.cancellations = 0;
    stats.execCancellations = 0;
    stats.ecPerformanceLost = 0;
    stats.misses = 0;
    stats.preemptions = 0;
    stats.usum = 0;
    stats.esum = 0;
    for (size_t i = 0; i < taskset->size(); i++) {
      Task* task = (*taskset)[i];
      stats.activations += task->getActivations();
      stats.completions += task->getCompletions();
      stats.cancellations += task->getCancellations();
      stats.execCancellations += task->getExecCancellations();
      stats.ecPerformanceLost += task->getEcPerformanceLost();
      stats.misses += task->getMisses();
      stats.preemptions += task->getPreemptions();
      stats.usum += task->getUA()->getTotal();
      stats.esum += task->getUA()->getCount();
    }
    stats.cancelSteps = cancelSteps;
    stats.idleSteps = idleSteps;
  }
  
  /*
    // don't use this anymore
  void Simulation::storeStatistics(Stat* stat) {
    stat->setSchedulerId(scheduler->getId());
    
    // foreach task store statistics
    for (size_t i = 0; i < taskset->size(); i++) {
      Task* task = (*taskset)[i];
      stat->addTaskId(task->getIdString());
      stat->setActivations(task->getIdString(),task->getActivations());
      stat->setCompletions(task->getIdString(),task->getCompletions());
      stat->setCancellations(task->getIdString(),task->getCancellations());
      stat->setExecCancellations(task->getIdString(),task->getExecCancellations());
      stat->setMisses(task->getIdString(),task->getMisses());
      stat->setPreemptions(task->getIdString(),task->getPreemptions());
      // TODO: SET task->getUA()
      stat->setUtilitySums(task->getIdString(),task->getUA()->getTotal());
      stat->setUtiltyCounts(task->getIdString(), task->getUA()->getCount());
      stat->setMeanUtilities(task->getIdString(),task->getUA()->getMeanUtility());
      stat->setHistoryValues(task->getIdString(),task->getHistoryValue());
      
      // TODO: Check if these values equal sum of the utilities
      stat->setSumOfUtilities(task->getUA()->getTotal());
      stat->setSumOfUtilityCounts(task->getUA()->getCount());
      
      // TODO: Implement
      vector<int> test = task->getCounters();
    }
    
    // TODO: Make better (not just store it in simulation object)
    stat->setIdleSteps(this->idleSteps);
  }
  */
  
  void Simulation::printExecStats(const string& head) {
    int activations = 0, completions = 0, cancellations = 0, execCancellations = 0, misses = 0, preemptions = 0;
    TmsTime ecPerformanceLost = 0;
    double usum = 0;
    int esum = 0;
    LOG(LOG_CLASS_SIMULATION) << head;
    LOG(LOG_CLASS_SIMULATION) << "# Task [hv] {act|compl|canc/ecanc|miss|preempt} [UA]";
    for (size_t i = 0; i < taskset->size(); i++) {
      Task* task = (*taskset)[i];
      LOG(LOG_CLASS_SIMULATION) << task->getIdString()
				<< " {" << task->getActivations()
				<< "|" << task->getCompletions()
				<< "|" << task->getCancellations()
				<< "/" << task->getExecCancellations()
				<< "(" << task->getEcPerformanceLost() << ")"
				<< "|" << task->getMisses()
				<< "|" << task->getPreemptions()
				<< "} " << *(task->getUA())
	;
      activations += task->getActivations();
      completions += task->getCompletions();
      cancellations += task->getCancellations();
      execCancellations += task->getExecCancellations();
      ecPerformanceLost += task->getEcPerformanceLost();
      misses += task->getMisses();
      preemptions += task->getPreemptions();
      usum += task->getUA()->getTotal();
      esum += task->getUA()->getCount();
    }
    LOG(LOG_CLASS_SIMULATION) << "Act: " << activations
			      << " Compl: " << completions
			      << " Canc: " << cancellations
			      << " Ecanc: " << execCancellations
			      << "(" << ecPerformanceLost << ")"
			      << " Miss: " << misses
			      << " Preempt: " << preemptions;
    LOG(LOG_CLASS_SIMULATION) << "U_Sys/ [" << usum << "/" << esum << "="
			      << (usum / esum) << "]";
    
    LOG(LOG_CLASS_SIMULATION) << "Cancel statistics (how often got $i tasks cancelled in one time step?):";
    /*    
    ostringstream osscs;
    for (int i = 0; i < CANCEL_SLOTS; ++i) {
      osscs << "CC[" << (i + 1) << "]=" << cancelCtr[i] << " ";
    }
    LOG(LOG_CLASS_SIMULATION) << osscs.str();
    */
    LOG(LOG_CLASS_SIMULATION) << "Total steps with cancellations: " << cancelSteps;

    /*
    ostringstream osscc;
    osscc << "CancelSteps: ";
    if (cancelStepList.size() > 0)
      for (list<int>::iterator it = cancelStepList.begin(); it != cancelStepList.end(); ++it) {
	osscc << *it << " ";
	}
    LOG(LOG_CLASS_SIMULATION) << osscc.str();
    */
    LOG(LOG_CLASS_SIMULATION) << "Idle steps: " << idleSteps;
  }

  
  void Simulation::printDelayCounters(void) {
    LOG(LOG_CLASS_SIMULATION) << "DelayCounters:";
    for (size_t i = 0; i < taskset->size(); i++) {
      const vector<int>& delayCounters = (*taskset)[i]->getCounters();
      
      ostringstream oss;
      oss << "Task " << (*taskset)[i]->getId() << ": ";
      
      if(delayCounters.size() > 0) {
	
	for (size_t j = 0; j < delayCounters.size(); ++j) {
	  if (delayCounters[j] != 0) {
	    oss << " (" << (j + 1) << "," << delayCounters[j] * (j + 1) << ")";
	  }
            }
	oss << " ";
	oss << "MAX: " << delayCounters.size();
	
      } else {
	oss << "None";
      }
      
      LOG(LOG_CLASS_SIMULATION) << oss.str();
    }
  }

  
} // NS tmssim
