/**
 * $Id: dummymkcts.cpp 1432 2016-07-15 10:43:04Z klugeflo $
 * @file dummymkcts.cpp
 * @brief Dummy CTS that does not perform any simulation, but uses the
 * results of another CTS.
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/dummymkcts.h>

#include <mkeval/mksimulation.h>
#include <schedulers/dummy.h>

#include <mkeval/gstsimulation.h>

#include <iostream>
using namespace std;

namespace tmssim {

  class DummySimulation: public MkSimulation {
  public:
    DummySimulation(std::list<MkTask*> _mkTasks, MkSimulation* _baseSim)
      : MkSimulation(_mkTasks, new DummyScheduler, "DUMMY"), baseSim(_baseSim) {
      //cout << "C_DummySimulation" << endl;
    }

    virtual ~DummySimulation() {
      //cout << "~DummySimulation()" << endl;
    }

    
    bool simulate() {
      //cout << "Simulate Dummy" << endl;
      return true;
    }

    
    std::string getInfoMessage() {
      return baseSim->getInfoMessage();
    }

    
    std::string getSimulationMessage() {
      return baseSim->getSimulationMessage();
    }

    
    bool getSuccess() {
      return baseSim->getSuccess();
    }

  private:
    MkSimulation* baseSim;
  };
  

  DummyConcreteMkTaskset::DummyConcreteMkTaskset(MkTaskset* _mkTaskset,
						 const vector<const MkEvalAllocatorPair*>& _allocators,
						 ConcreteMkTaskset* _cts)
    : ConcreteMkTaskset(_mkTaskset, _allocators), cts(_cts) {
    if (*_mkTaskset != *(cts->getMkTaskset())) {
      throw TMSException("Tasksets are not identical!");
    }
    //cout << "C_DummyConcreteMkTaskset" << endl;

    size_t i = 0;
    for (MkSimulation* mksim: cts->getMkSimulations()) {
      //cout << "\tnew dummy sim" << endl;
      list<MkTask*> tasks;
      for (MkTask* task: _mkTaskset->tasks) {
	tasks.push_back(new MkTask(task));
      }
      mkSimulations[i] = new DummySimulation(tasks, mksim);

      simIds[mkSimulations[i]] = i;
      i++;
    }
    finishedMap = ~( (1ULL << cts->getMkSimulations().size()) - 1 );

    
  }
      
  DummyConcreteMkTaskset::~DummyConcreteMkTaskset() {
    //cout << "~DummyConcreteMkTaskset" << endl;
  }

} // NS tmssim
