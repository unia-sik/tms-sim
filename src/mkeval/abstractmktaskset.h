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
 * $Id: abstractmktaskset.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file abstractmktaskset.h
 * @brief Abstract (m,k)-taskset for generation of actual MkTasks
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_ABSTRACTMKTASKSET_H
#define MKEVAL_ABSTRACTMKTASKSET_H

#include <core/primitives.h>
#include <mkeval/mkglobals.h>
#include <mkeval/periodgenerator.h>

#include <taskmodels/mktask.h>
#include <utils/kvfile.h>

namespace tmssim {

  struct AbstractMkTask {
  AbstractMkTask() : period(0), m(0), k(0), wc(0), c(0), priority(0) {}
    TmsTimeInterval period;
    unsigned int m;
    unsigned int k;
    unsigned int wc;
    TmsTimeInterval c;
    TmsPriority priority;
  };

  std::ostream& operator << (std::ostream& ost, const AbstractMkTask& task);

  class AbstractMkTaskset {
  public:
    AbstractMkTaskset(unsigned int _seed, size_t _size, const KvFile* _cfg,
		      double _uDev, double _uMin,
		      PeriodGenerator* _pg = NULL,
		      UCMKAllocator _ucAlloc = DefaultUCMKAllocator,
		      UAMKAllocator _uaAlloc = DefaultUAMKAllocator);
    ~AbstractMkTaskset();

    void setUtilisation(double u);
    double getUtilisation() const;
    double getRealUtilisation() const;
    double getMkUtilisation() const;
    unsigned int getSeed() const;

    /**
     * @brief clean up after use
     */
    MkTaskset* getMkTasks() const;

    TmsTime getHyperPeriod() const { return hyperPeriod; }
    TmsTime getMkFeasibilityMultiplicator() const { return mkFeasibilityMultiplicator; }

    TmsTime getMkpFeasibilityPeriod() const;
    
    static bool checkCfg(const KvFile* cfg);

    friend std::ostream& operator << (std::ostream& ost, const AbstractMkTaskset& ts);
    
  private:
    void calculateExecutionTimes();
    void clearAbstractTasks();
    unsigned int produceInt(unsigned int* seed, unsigned int min, unsigned int max);
    
    unsigned int theSeed;
    size_t size;
    double uMin;
    double uCurrent;
    double uReal;
    double uMk;
    TmsTime hyperPeriod;
    TmsTimeInterval mkFeasibilityMultiplicator;
    UCMKAllocator ucAlloc;
    UAMKAllocator uaAlloc;

    list<AbstractMkTask*> abstractTasks;
  };


} // NS tmssim

#endif // !MKEVAL_ABSTRACTMKTASKSET_H
