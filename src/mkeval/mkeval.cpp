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
 * $Id: mkeval.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file mkeval.cpp
 * @brief Store for (m,k) evaluations
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/mkeval.h>
#include <mkeval/mkallocators.h>
//#include <core/stat.h>

#include <utils/tlogger.h>
#include <xmlio/tasksetwriter.h>

#include <cassert>
#include <sstream>
using namespace std;

namespace tmssim {

  MKSimulationResults::MKSimulationResults()
    : SimulationResults(), mkfail(false) {
  }


  MKSimulationResults::MKSimulationResults(const SimulationResults& simStats)
    : SimulationResults(simStats), mkfail(false) {
  }


  MKSimulationResults::MKSimulationResults(const MKSimulationResults& rhs)
    : SimulationResults(rhs), mkfail(rhs.mkfail) {
  }


  std::ostream& operator<< (std::ostream& out, const MKSimulationResults& stat) {
    out << (SimulationResults) stat;
    out << "(m,k)-fail: " << stat.mkfail << endl;
    return out;
    }


  MkEval::MkEval(MkTaskset* _taskset, size_t _nSchedulers,
		 const vector<const MkEvalAllocatorPair*>& _allocators,
		 const SchedulerConfiguration& _schedulerConfiguration,
		 TmsTime _steps)
    : taskset(_taskset), nSchedulers(_nSchedulers), allocators(_allocators),
      schedulerConfiguration(_schedulerConfiguration), steps(_steps) {
    assert(taskset != NULL);
    simulations = new Simulation*[nSchedulers];
    mkts = new std::vector<MkTask*>*[nSchedulers];
    results = new MKSimulationResults[nSchedulers];
    for (unsigned i = 0; i < nSchedulers; ++i) {
      simulations[i] = NULL;
    }
    successMap = 0;
  }


  MkEval::~MkEval() {
    // Dealloc taskset
    delete taskset;
    for (unsigned i = 0; i < nSchedulers; ++i) {
      if (simulations[i] != NULL)
	delete simulations[i];
      if (mkts[i] != NULL) {
	delete mkts[i];
	// we don't need to delete the tasks, this was already done by the simulations
      }
    }
    delete[] simulations;
    delete[] mkts;
    delete[] results;
  }


  void MkEval::run() {
    for (unsigned int i = 0; i < nSchedulers; ++i) {
      prepareEval(i);
      runEval(i);
    }
  }


  const MKSimulationResults* MkEval::getResults() const {
    return results;
  }


  unsigned int MkEval::getSuccessMap() const {
    return successMap;
  }

  const MkTaskset* MkEval::getTaskset() const {
    return taskset;
  }


  const vector<MkTask*>* MkEval::getSimTasks(size_t i) const {
    if (i >= nSchedulers)
      return NULL;
    else
      return mkts[i];
  }


  void MkEval::prepareEval(unsigned int num) {
    assert(num < nSchedulers);
    // copy taskset
    vector<Task*>* ts = new vector<Task*>;
    mkts[num] = new vector<MkTask*>;
    for (MkTask* mt: taskset->tasks) {
      if (mt == NULL) {
	cerr << "No task found!\n";
	abort();
      }
      MkTask* task = allocators[num]->taskAlloc(mt);
      ts->push_back(task);
      mkts[num]->push_back(task);
    }
    simulations[num] = new Simulation(ts, allocators[num]->schedAlloc(schedulerConfiguration)); // FIXME: take from exec config
  }


  void MkEval::runEval(unsigned int num) {
    assert(num < nSchedulers);
    Simulation::ExitCondition rres;
    rres = simulations[num]->run(steps);
    if (rres == 0) {
      rres = simulations[num]->finalise();
    }
    else {
      //cerr << "Sim failed!" << endl;
    }
    results[num] = simulations[num]->getResults();

    if (results[num].simulatedTime < steps || !results[num].success) {
      //cerr << "Sim failed (2)!" << endl;
      results[num].mkfail = true;
    }

    /*
    if (results[num].simulatedTime < steps || !results[num].success) {
      ostringstream oss;
      oss << "results/tsfail-" << taskset->seed << "-" << allocators[num].id << ".xml";
      string filename = oss.str();
      TasksetWriter* writerPtr = TasksetWriter::getInstance();
      if (!writerPtr->write(filename, *results[num].taskset)) {
	tError() << "Error while writing tasks to file: " << filename << "!";
      }
    }
    else {
    */
    //results[num].success = true;
    successMap |= EVAL_MAP_BIT(num);
    //}
    for (vector<MkTask*>::iterator it = mkts[num]->begin();
	 it != mkts[num]->end(); ++it) {
      if ((*it)->getMonitor().getViolations() > 0) {
	results[num].mkfail = true;
	break;
      }
    }

  }


} // NS tmssim
