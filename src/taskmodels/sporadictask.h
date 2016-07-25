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
 * $Id: sporadictask.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file sporadictask.h
 * @brief Sporadic task class
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TASKMODELS_SPORADICTASK_H
#define TASKMODELS_SPORADICTASK_H 1

#include <core/scobjects.h>

namespace tmssim {

  class SporadicTask : public Task {
  public:
    SporadicTask(unsigned int _id, TmsTimeInterval _minPeriod,
		 TmsTimeInterval _et, unsigned int _seed,
		 TmsTimeInterval _ct,
		 UtilityCalculator* __uc, UtilityAggregator* __ua,
		 double _probability=0.5, int _offset=0, TmsPriority _prio=1);
    SporadicTask(unsigned int _id, TmsTimeInterval _minPeriod,
		 TmsTimeInterval _et, unsigned int _seed,
		 UtilityCalculator* __uc, UtilityAggregator* __ua,
		 TmsPriority _prio);
    SporadicTask(const SporadicTask &rhs);
    
    virtual std::ostream& print(std::ostream& ost) const;
    
    virtual double getLastExecValue(void) const;
    virtual double getHistoryValue(void) const;
    //virtual double getPossibleExecValue(const Job *job, int startTime) const;
    //virtual double getPossibleHistoryValue(const Job *job, int startTime) const;
    
    /**
     * Serializes this object with the given xmlTextWriter to a xml-document
     * @param writer A pointer to the xmlWriter that should be used for serialization
     */
    //void write(xmlTextWriterPtr writer) const;
    
    /**
     * Creates a deep-copy of the current task
     * @return A pointer to a new object with the same properties of this object
     */
    virtual Task* clone() const;
    
    /**
     * @name XML
     * @{
     */
    virtual const std::string& getClassElement();
    virtual int writeData(xmlTextWriterPtr writer);
    /**
     * @}
     */

  protected:
    virtual TmsTime startHook(TmsTime now);
    virtual Job* spawnHook(TmsTime now);
    virtual TmsTimeInterval getNextActivationOffset(TmsTime now);
    virtual bool completionHook(Job *job, TmsTime now);
    virtual bool cancelHook(Job *job);
    virtual double calcExecValue(const Job *job, TmsTime complTime) const;
    virtual double calcHistoryValue(const Job *job, TmsTime complTime) const;
    virtual double calcFailHistoryValue(const Job *job) const;

    virtual void advanceHistory(void);
    
    virtual std::string getShortId(void) const;
    
    TmsTimeInterval minPeriod;
    
    //int ct; ///< critical time relative to last activation
    TmsTimeInterval offset; ///< offset for first activation
    double lastUtility;
    double historyUtility;
    bool activationPending;
    unsigned int seed;
    double probability;
    unsigned int baseSeed;
    TmsPriority _priority;
  };
  
} // NS tmssim

#endif /* !TASKMODELS_SPORADICTASK_H */
