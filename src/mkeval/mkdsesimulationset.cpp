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
 * $Id: mkdsesimulationset.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file mkdsesimulationset.cpp
 * @brief Set of DSE simulations for a single AbstractMkTaskset
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/mkdsesimulationset.h>

#include <mkeval/dummymkcts.h>

#include <iostream>
using namespace std;

namespace tmssim {

  MkDseSimulationSet::MkDseSimulationSet(AbstractMkTaskset* _ats, double _u0, double _uStep,
					 const vector<const MkEvalAllocatorPair*>& _allocators,
					 const SchedulerConfiguration& _scc)
    : ats(_ats), u0(_u0), uStep(_uStep), allocators(_allocators), scc(_scc),
      nCts(0)
  {
    bool valid = true;
    double u = u0;
    MkTaskset* previousTasks = NULL;
    ConcreteMkTaskset* previousCts = NULL;
    do {
      //cout << "Target utilisation: " << u << endl;
      ats->setUtilisation(u);
      if (ats->getMkUtilisation() <= 1.0) {
	MkTaskset* mktasks = ats->getMkTasks();
	// @todo compare with previous, if identical, do not create new
	// CTS, but record instead.
	mktss.push_back(mktasks);
	if (previousTasks != NULL && *previousTasks == *mktasks) {
	  //cout << "duplicate @ " << mktasks->targetUtilisation << endl;
	  DummyConcreteMkTaskset* dcts = new DummyConcreteMkTaskset(mktasks, allocators, previousCts);
	  ctss.push_back(dcts);
	  ctsId[dcts] = nCts;
	  //delete mktasks;
	}
	else {
	  //cout << "normal ts" << endl;
	  ConcreteMkTaskset* cts = new ConcreteMkTaskset(mktasks, allocators, scc);
	  previousTasks = mktasks;
	  previousCts = cts;
	  
	  ctss.push_back(cts);
	  ctsId[cts] = nCts;
	}

	++nCts;
	u += uStep;
      }
      else {
	valid = false;
      }
    } while (valid);
    //finishedMap = ~( (1ULL << nCts) - 1 );
    finishedMap = new Bitmap(nCts);
    //cout << "Have " << nCts << " utils, " << ctss.size() << " cts" << endl;
  }

  
  MkDseSimulationSet::~MkDseSimulationSet() {
    //cout << "~MkDseSimulationSet" << endl;
    for (MkTaskset* mkTaskset: mktss) {
      delete mkTaskset;
    }
    for (ConcreteMkTaskset* cts: ctss) {
      delete cts;
    }
    delete ats; //?
    delete finishedMap;
  }


  void testMkDseSimulationSet(void) {
    const MkAllocators& mka = MkAllocators::instance();
    vector<const MkEvalAllocatorPair*> allocators;
    allocators.push_back(mka.getAllocatorPair("DBP"));
    allocators.push_back(mka.getAllocatorPair("MKU"));
    allocators.push_back(mka.getAllocatorPair("MKC"));
    allocators.push_back(mka.getAllocatorPair("GDPA"));
    allocators.push_back(mka.getAllocatorPair("GMUA-MK"));
    KvFile* cfg = new KvFile("../cfg/long.mkg");
    AbstractMkTaskset* ats = new AbstractMkTaskset(314, 5, cfg, 0.05, 1.05);

    MkDseSimulationSet* dss = new MkDseSimulationSet(ats, 1.05, 0.1, allocators, SchedulerConfiguration());

    cout << "Start finish state of simset: " << dss->isFinished() << endl;
    const std::vector<ConcreteMkTaskset*> ctss = dss->getCtss();
    for (ConcreteMkTaskset* cts: ctss) {
      const vector<MkSimulation*> sims = cts->getMkSimulations();
      for (MkSimulation* sim: sims) {
	cts->notifyFinished(sim);
      }
      if (cts->isFinished()) {
	dss->notifyFinished(cts);
      }
      else {
	cout << "[ERROR] CTS has unfinished simulations!" << endl;
      }
    }
    cout << "End finish state of simset: " << dss->isFinished() << endl;
    
    delete dss;
    delete cfg;
  }
  
  
} // NS tmssim
