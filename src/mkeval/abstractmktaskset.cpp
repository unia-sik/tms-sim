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
 * $Id: abstractmktaskset.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file abstractmktaskset.cpp
 * @brief Abstract (m,k)-task for generation of actual MkTasks
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/abstractmktaskset.h>

#include <mkeval/intervalperiodgenerator.h>
#include <mkeval/mkgenerator.h>

#include <utils/tlogger.h>
#include <utils/tmsmath.h>

#include <cassert>
#include <cmath>
#include <cstdlib>

#include <iostream>
using namespace std;

namespace tmssim {


  
  
  AbstractMkTaskset::AbstractMkTaskset(unsigned int _seed, size_t _size,
				       const KvFile* _cfg,
				       double _uDev, double _uMin,
				       PeriodGenerator* _pg,
				       UCMKAllocator _ucAlloc, UAMKAllocator _uaAlloc)
    : size(_size), uMin(_uMin), uCurrent(_uMin),
      ucAlloc(_ucAlloc), uaAlloc(_uaAlloc) {

    tDebug() << "Creating task set (" << size << " tasks) with uMin=" << uMin
	     << " uDev=" << _uDev;

    unsigned int tsSeed = _seed;
    // 1340065553
    bool tsValid = false;

    float minMPerc;
    if (_cfg->containsKey("minM")) {
      minMPerc = _cfg->getFloat("minM");
    }
    else {
      minMPerc = 0;
    }

    PeriodGenerator* pg;
    if (_pg == NULL) {
      pg = new IntervalPeriodGenerator(_cfg->getUInt32("minPeriod"), _cfg->getUInt32("maxPeriod"));
    }
    else {
      pg = _pg;
    }
    
    while (!tsValid) {
      theSeed = tsSeed;

      for (size_t i = 0; i < size; ++i) {
	AbstractMkTask* mt = new AbstractMkTask;
	//mt->period = produceInt(&tsSeed, _cfg->getUInt32("minPeriod"), _cfg->getUInt32("maxPeriod"));
	unsigned int pseed = rand_r(&tsSeed);
	mt->period = pg->getNumber(pseed);

	mt->k = produceInt(&tsSeed, _cfg->getUInt32("minK"), _cfg->getUInt32("maxK"));
	if (minMPerc == 0) {
	  mt->m = produceInt(&tsSeed, 1, mt->k);
	}
	else {
	  long mm = lround ((float)(mt->k) * minMPerc);
	  if (mm == 0) {
	    mm = 1;
	  }
	  if (mm == mt->k) {
	    mt->m = mt->k;
	  }
	  else {
	    mt->m = produceInt(&tsSeed, mm, mt->k);
	  }
	  
	}
	mt->wc = produceInt(&tsSeed, 1, _cfg->getUInt32("maxWC"));

	// insert in rate-monotonic order (increasing periods)
	list<AbstractMkTask*>::iterator it = abstractTasks.begin();
	while (it != abstractTasks.end()
	       && (*it)->period < mt->period) {
	  ++it;
	}
	abstractTasks.insert(it, mt);
      }

      calculateExecutionTimes(); // calculates uMK and uReal
      tDebug() << "uMk: " << uMk << " uReal: " << uReal << " seed: " << theSeed;
      
      if (uMk > 1 || uReal < (_uMin - _uDev) || uReal > (_uMin + _uDev) ) {
	tsValid = false;
	clearAbstractTasks();	
	tsSeed = rand_r(&_seed);
      }
      else {
	tsValid = true;
      }
    } // while(!tsValid)
    unsigned int p = 0;
    for (AbstractMkTask* mt: abstractTasks) {
      mt->priority = p;
      ++p;
    }

    hyperPeriod = 1;
    mkFeasibilityMultiplicator = 1;
    for (AbstractMkTask* mt: abstractTasks) {
      hyperPeriod = calculateLcm(hyperPeriod, mt->period);
      unsigned m = mt->m;
      unsigned k = mt->k;

      TmsTimeInterval taskFm = 0;
      for (size_t j = m; j <= k; ++j) {
	taskFm += binomialCoefficient(k, j);
      }
      //cout << "taskFm: " << taskFm << endl;
      mkFeasibilityMultiplicator *= taskFm;
    }

  }
  
  
  AbstractMkTaskset::~AbstractMkTaskset() {
    clearAbstractTasks();
  }

  
  void AbstractMkTaskset::setUtilisation(double u) {
    uCurrent = u;
    calculateExecutionTimes();
    tDebug() << "uMk: " << uMk << " uReal: " << uReal << " seed: " << theSeed;
  }

  
  double AbstractMkTaskset::getUtilisation() const {
    return uCurrent;
  }

  
  double AbstractMkTaskset::getRealUtilisation() const {
    return uReal;
  }

  
  double AbstractMkTaskset::getMkUtilisation() const {
    return uMk;
  }


  unsigned int AbstractMkTaskset::getSeed() const {
    return theSeed;
  }


  //! @todo calculate rotation/spin values!
  MkTaskset* AbstractMkTaskset::getMkTasks() const {
    MkTaskset* ts = new MkTaskset;
    ts->seed = theSeed;
    unsigned int i = 0;
    for (const AbstractMkTask* mt: abstractTasks) {
      MkTask* task = new MkTask(i, mt->period, mt->c, mt->period, ucAlloc(),
				uaAlloc(mt->m, mt->k), mt->priority, mt->m,
				mt->k);
      ts->tasks.push_back(task);
      ++i;
    }
    ts->realUtilisation = uReal;
    ts->mkUtilisation = uMk;
    ts->seedUtilisation = uMin;
    ts->targetUtilisation = uCurrent;
    MkGenerator::calcRotationValues(ts->tasks);
    return ts;
  }


  TmsTime AbstractMkTaskset::getMkpFeasibilityPeriod() const {
    TmsTime feasibilityPeriod = 1;
    for (const AbstractMkTask* mt: abstractTasks) {
      feasibilityPeriod = calculateLcm(feasibilityPeriod, mt->period);
      feasibilityPeriod = calculateLcm(feasibilityPeriod, mt->k);
    }
    return feasibilityPeriod;
  }
  

  bool AbstractMkTaskset::checkCfg(const KvFile* cfg) {
    return cfg->containsKey("minPeriod")
      && cfg->containsKey("maxPeriod")
      && cfg->containsKey("minK")
      && cfg->containsKey("maxK")
      && cfg->containsKey("maxWC");
  }


  void AbstractMkTaskset::calculateExecutionTimes() {
    unsigned int sumWC = 0;
    for (const AbstractMkTask* mt: abstractTasks) {
      sumWC += mt->wc;
    }
    double uw = uCurrent / sumWC;
    tDebug() << "\tuw=" << uw;
    uReal = 0;
    uMk = 0;
    for (AbstractMkTask* mt: abstractTasks) {
      double ch = uw * mt->period * mt->wc;
      mt->c = lround(ch);
      if (mt->c == 0)
	mt->c = 1;
      double uMt = (double)mt->c / (double)mt->period;
      uReal += uMt;
      uMk += uMt * (double)mt->m / (double)mt->k;
      tDebug() << "\t" << mt->wc << " -> (" << mt->c << "," << mt->period << ")";
    }
  }
  /*
    Problem: different utilisation may still yield the same task set due to
    rounding, e.g.:
    $ bin/mkexteval -c share/tms-sim/cfg/default.mkg -t 5 -u1.1 -u1.2 -d0.05 -u1.5 -u1.7 -s 1071768609 -u1.9 -u1.6 -u1.8 -u 1.4

Setting utilisation 1.5
        uw=0.00761421
        31 -> (1,5)
        64 -> (3,6)
        28 -> (2,9)
        66 -> (6,11)
        8 -> (1,14)
uMk: 0.637049 uReal: 1.53911 seed: 1071768609

Setting utilisation 1.6
        uw=0.00812183
        31 -> (1,5)
        64 -> (3,6)
        28 -> (2,9)
        66 -> (6,11)
        8 -> (1,14)
uMk: 0.637049 uReal: 1.53911 seed: 1071768609

Setting utilisation 1.7
        uw=0.00862944
        31 -> (1,5)
        64 -> (3,6)
        28 -> (2,9)
        66 -> (6,11)
        8 -> (1,14)
uMk: 0.637049 uReal: 1.53911 seed: 1071768609

Setting utilisation 1.8
        uw=0.00913706
        31 -> (1,5)
        64 -> (4,6)
        28 -> (2,9)
        66 -> (7,11)
        8 -> (1,14)
uMk: 0.730988 uReal: 1.79668 seed: 1071768609

Setting utilisation 1.9
        uw=0.00964467
        31 -> (1,5)
        64 -> (4,6)
        28 -> (2,9)
        66 -> (7,11)
        8 -> (1,14)
uMk: 0.730988 uReal: 1.79668 seed: 1071768609
   */

  
  void AbstractMkTaskset::clearAbstractTasks() {
    for (AbstractMkTask* mt: abstractTasks) {
      delete mt;
    }
    abstractTasks.clear();
  }


  unsigned int AbstractMkTaskset::produceInt(unsigned int* seed, unsigned int min, unsigned int max) {
    assert(min < max);
    unsigned int len = max - min + 1; // due to modulo arithmetics
    unsigned long long int interval = RAND_MAX / len;
    unsigned long long int rnd = rand_r(seed);
    unsigned int number = (uint32_t) ((uint64_t)rnd / interval);
    return number + min;
  }


  std::ostream& operator << (std::ostream& ost, const AbstractMkTask& task) {
    /*
    TmsTimeInterval period;
    unsigned int m;
    unsigned int k;
    unsigned int wc;
    TmsTimeInterval c;
    TmsPriority priority;
    */
    ost << "[" << task.period << ",<" << task.wc << ">,(" << task.m << "," << task.k << ")]";
    return ost;
  }

  
  std::ostream& operator << (std::ostream& ost, const AbstractMkTaskset& ts) {
    for (AbstractMkTask *task : ts.abstractTasks) {
      ost << *task;
    }
    return ost;
  }

} // NS tmssim
